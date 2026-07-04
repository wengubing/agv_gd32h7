
# ============================================================================
# capture-keil-ui.ps1
#
# 用途：自动化打开 Keil uVision 工程，支持三种模式下的主窗口/菜单/调试/OS Support/线程等截图批量采集。
#
# 主要参数：
#   -Mode basic             # 默认，依次截图主窗口、调试窗口、运行窗口
#   -Mode debug-menu-walk   # 遍历 Debug 菜单下拉各项并截图
#   -Mode os-support-sweep  # 遍历 Debug->OS Support 子菜单各项并截图
#   -DebugMenuOffsetX       # Debug 菜单栏 X 偏移（像素，默认232）
#   -MenuBarOffsetY         # 菜单栏 Y 偏移（像素，默认41）
#   -DebugMenuWalkCount     # Debug 菜单遍历项数（默认27）
#   -OsSupportDownCount     # 下移到 OS Support 所需 DOWN 次数（默认15）
#   -OsSupportItemCount     # OS Support 子菜单项数（默认9）
#   -KeepOpen               # 运行后不自动关闭 Keil
#
# 调用示例：
#   powershell -ExecutionPolicy Bypass -File tools/capture-keil-ui.ps1 -Mode basic
#   powershell -ExecutionPolicy Bypass -File tools/capture-keil-ui.ps1 -Mode debug-menu-walk -DebugMenuWalkCount 27
#   powershell -ExecutionPolicy Bypass -File tools/capture-keil-ui.ps1 -Mode os-support-sweep -OsSupportDownCount 15 -OsSupportItemCount 9
#
# 产物：所有截图和元数据均输出到 ztemp 目录
# ============================================================================
param(
    [string]$Uv4Path = "C:/Keil_v5/UV4/UV4.exe",           # Keil UV4 路径
    [string]$ProjectFile = "Project/gd32h7.uvprojx",       # 工程文件
    [string]$Target = "Project",                           # 目标 Target 名称
    [int]$RunSeconds = 6,                                   # 运行态截图等待秒数
    [switch]$KeepOpen,                                      # 是否调试后保留 Keil 窗口
    [ValidateSet("basic", "debug-menu-walk", "os-support-sweep")]
    [string]$Mode = "basic",                               # 运行模式
    [int]$DebugMenuOffsetX = 232,                           # Debug 菜单 X 偏移
    [int]$MenuBarOffsetY = 41,                              # 菜单栏 Y 偏移
    [int]$DebugMenuWalkCount = 27,                          # Debug 菜单遍历项数
    [int]$OsSupportDownCount = 15,                          # OS Support 下移步数
    [int]$OsSupportItemCount = 9                            # OS Support 子菜单项数
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Resolve-AbsPath {
    # 路径转绝对路径，支持相对工程根目录
    param([string]$PathValue)
    if ([System.IO.Path]::IsPathRooted($PathValue)) {
        return [System.IO.Path]::GetFullPath($PathValue)
    }
    $repoRoot = Join-Path -Path $PSScriptRoot -ChildPath ".."
    return [System.IO.Path]::GetFullPath((Join-Path -Path $repoRoot -ChildPath $PathValue))
}

if (-not ("Uv4WindowApi" -as [type])) {
    # Win32 API 封装：窗口坐标、前台、鼠标点击
Add-Type -AssemblyName System.Drawing
Add-Type -TypeDefinition @"
using System;
using System.Drawing;
using System.Runtime.InteropServices;

public static class Uv4WindowApi {
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT {
        public int Left;
        public int Top;
        public int Right;
        public int Bottom;
    }

    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr hWnd, out RECT rect);

    [DllImport("user32.dll")]
    public static extern bool SetForegroundWindow(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern bool SetCursorPos(int X, int Y);

    [DllImport("user32.dll")]
    public static extern void mouse_event(uint dwFlags, uint dx, uint dy, uint dwData, UIntPtr dwExtraInfo);

    public const uint MOUSEEVENTF_LEFTDOWN = 0x0002;
    public const uint MOUSEEVENTF_LEFTUP = 0x0004;
}
"@
}

function Wait-MainWindow {
    # 等待 Keil 主窗口出现
    param([System.Diagnostics.Process]$Process, [int]$TimeoutSeconds = 25)

    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    do {
        $Process.Refresh()
        if ($Process.MainWindowHandle -ne 0) {
            return $true
        }
        Start-Sleep -Milliseconds 300
    } while ((Get-Date) -lt $deadline -and -not $Process.HasExited)

    return $false
}

function Save-WindowScreenshot {
    # 截取指定窗口句柄的屏幕区域
    param(
        [IntPtr]$Hwnd,
        [string]$Path
    )

    $rect = New-Object Uv4WindowApi+RECT
    if (-not [Uv4WindowApi]::GetWindowRect($Hwnd, [ref]$rect)) {
        throw "GetWindowRect failed"
    }

    $width = $rect.Right - $rect.Left
    $height = $rect.Bottom - $rect.Top
    if ($width -le 0 -or $height -le 0) {
        throw "Invalid window size: ${width}x${height}"
    }

    $bmp = New-Object System.Drawing.Bitmap($width, $height)
    $gfx = [System.Drawing.Graphics]::FromImage($bmp)
    try {
        $gfx.CopyFromScreen($rect.Left, $rect.Top, 0, 0, $bmp.Size)
        $bmp.Save($Path)
    }
    finally {
        $gfx.Dispose()
        $bmp.Dispose()
    }
}

function Invoke-LeftClick {
    # 模拟鼠标左键点击指定坐标
    param(
        [int]$X,
        [int]$Y
    )

    [Uv4WindowApi]::SetCursorPos($X, $Y) | Out-Null
    Start-Sleep -Milliseconds 60
    [Uv4WindowApi]::mouse_event([Uv4WindowApi]::MOUSEEVENTF_LEFTDOWN, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 30
    [Uv4WindowApi]::mouse_event([Uv4WindowApi]::MOUSEEVENTF_LEFTUP, 0, 0, 0, [UIntPtr]::Zero)
}

function Invoke-DebugMenuWalk {
    # 遍历 Debug 菜单下拉各项并截图
    param(
        [System.Diagnostics.Process]$Process,
        [object]$Wshell,
        [string]$OutputDir,
        [string]$Timestamp,
        [int]$MenuX,
        [int]$MenuY,
        [int]$WalkCount
    )

    $shots = New-Object System.Collections.Generic.List[string]
    for ($i = 0; $i -lt $WalkCount; $i++) {
        Invoke-LeftClick -X $MenuX -Y $MenuY
        Start-Sleep -Milliseconds 220
        for ($k = 0; $k -lt $i; $k++) {
            $Wshell.SendKeys("{DOWN}")
            Start-Sleep -Milliseconds 90
        }

        $shot = Join-Path $OutputDir ("uv4_debugmenu_walk_" + $Timestamp + "_" + ("{0:D2}" -f $i) + ".png")
        Save-WindowScreenshot -Hwnd $Process.MainWindowHandle -Path $shot
        $shots.Add($shot)

        $Wshell.SendKeys("{ESC}")
        Start-Sleep -Milliseconds 150
    }

    return $shots
}

function Invoke-OsSupportSweep {
    # 遍历 Debug->OS Support 子菜单各项并截图
    param(
        [System.Diagnostics.Process]$Process,
        [object]$Wshell,
        [string]$OutputDir,
        [string]$Timestamp,
        [int]$MenuX,
        [int]$MenuY,
        [int]$DownCountToOsSupport,
        [int]$ItemCount
    )

    $shots = New-Object System.Collections.Generic.List[string]
    for ($j = 0; $j -lt $ItemCount; $j++) {
        Invoke-LeftClick -X $MenuX -Y $MenuY
        Start-Sleep -Milliseconds 250

        for ($i = 0; $i -lt $DownCountToOsSupport; $i++) {
            $Wshell.SendKeys("{DOWN}")
            Start-Sleep -Milliseconds 80
        }

        $Wshell.SendKeys("{RIGHT}")
        Start-Sleep -Milliseconds 180

        for ($k = 0; $k -lt $j; $k++) {
            $Wshell.SendKeys("{DOWN}")
            Start-Sleep -Milliseconds 70
        }

        $menuShot = Join-Path $OutputDir ("uv4_osprobe_" + $Timestamp + "_menu_" + ("{0:D2}" -f $j) + ".png")
        Save-WindowScreenshot -Hwnd $Process.MainWindowHandle -Path $menuShot
        $shots.Add($menuShot)

        $Wshell.SendKeys("{ENTER}")
        Start-Sleep -Milliseconds 900

        $mainShot = Join-Path $OutputDir ("uv4_osprobe_" + $Timestamp + "_main_" + ("{0:D2}" -f $j) + ".png")
        Save-WindowScreenshot -Hwnd $Process.MainWindowHandle -Path $mainShot
        $shots.Add($mainShot)

        $Wshell.SendKeys("{ESC}")
        Start-Sleep -Milliseconds 180
    }

    return $shots
}

$uv4Abs = Resolve-AbsPath -PathValue $Uv4Path
$projAbs = Resolve-AbsPath -PathValue $ProjectFile
$ztemp = Resolve-AbsPath -PathValue "ztemp"

if (-not (Test-Path $ztemp)) {
    New-Item -Path $ztemp -ItemType Directory -Force | Out-Null
}

if (-not (Test-Path $uv4Abs)) {
    throw "UV4 not found: $uv4Abs"
}
if (-not (Test-Path $projAbs)) {
    throw "Project not found: $projAbs"
}

$existing = Get-Process UV4 -ErrorAction SilentlyContinue
    # 启动前关闭所有已存在的 Keil UV4 进程，避免冲突
if ($existing) {
    foreach ($p in $existing) {
        try {
            if (-not $p.HasExited) {
                $null = $p.CloseMainWindow()
            }
        }
        catch {
        }
    }
    Start-Sleep -Seconds 1
    Get-Process UV4 -ErrorAction SilentlyContinue | Stop-Process -Force
}

$ts = Get-Date -Format "yyyyMMdd_HHmmss"
$shotOpen = Join-Path $ztemp ("uv4_ui_" + $ts + "_01_open.png")
$shotDebug = Join-Path $ztemp ("uv4_ui_" + $ts + "_02_debug.png")
$shotRun = Join-Path $ztemp ("uv4_ui_" + $ts + "_03_run.png")
$metaJson = Join-Path $ztemp ("uv4_ui_" + $ts + "_meta.json")
$artifacts = New-Object System.Collections.Generic.List[string]

$args = @("`"$projAbs`"", "-t", "`"$Target`"")
$proc = Start-Process -FilePath $uv4Abs -ArgumentList $args -PassThru

if (-not (Wait-MainWindow -Process $proc -TimeoutSeconds 30)) {
    # 主窗口未出现则报错退出
    throw "uVision main window did not appear"
}

$wshell = New-Object -ComObject WScript.Shell
$null = $wshell.AppActivate($proc.Id)
Start-Sleep -Milliseconds 500

# Dismiss potential startup modal dialogs (e.g., improper-close warning) that block shortcuts.
for ($i = 0; $i -lt 3; $i++) {
    # 连续尝试消除启动时的弹窗（如未正常关闭提示）
    $null = $wshell.AppActivate($proc.Id)
    Start-Sleep -Milliseconds 200
    $wshell.SendKeys("{ENTER}")
    Start-Sleep -Milliseconds 250
}

$proc.Refresh()
[Uv4WindowApi]::SetForegroundWindow($proc.MainWindowHandle) | Out-Null
Start-Sleep -Milliseconds 300
Save-WindowScreenshot -Hwnd $proc.MainWindowHandle -Path $shotOpen
$artifacts.Add($shotOpen)   # 记录产物
$artifacts.Add($shotOpen)

# Enter debug
$wshell.SendKeys("^{F5}")
Start-Sleep -Seconds 2
$proc.Refresh()
if ($proc.MainWindowHandle -ne 0) {
    [Uv4WindowApi]::SetForegroundWindow($proc.MainWindowHandle) | Out-Null
    Start-Sleep -Milliseconds 300
    Save-WindowScreenshot -Hwnd $proc.MainWindowHandle -Path $shotDebug
    $artifacts.Add($shotDebug)
    $artifacts.Add($shotDebug)
}

if ($Mode -eq "basic") {
    # 经典三步截图：主窗口、调试、运行
    $wshell.SendKeys("{F5}")
    Start-Sleep -Seconds $RunSeconds
    $proc.Refresh()
    if ($proc.MainWindowHandle -ne 0) {
        [Uv4WindowApi]::SetForegroundWindow($proc.MainWindowHandle) | Out-Null
        Start-Sleep -Milliseconds 300
        Save-WindowScreenshot -Hwnd $proc.MainWindowHandle -Path $shotRun
        $artifacts.Add($shotRun)
    }
}
else {
    # 菜单自动化模式：菜单坐标基于窗口左上角偏移
    $proc.Refresh()
    $rect = New-Object Uv4WindowApi+RECT
    if (-not [Uv4WindowApi]::GetWindowRect($proc.MainWindowHandle, [ref]$rect)) {
        throw "GetWindowRect failed before menu probing"
    }

    $menuX = [int]$rect.Left + $DebugMenuOffsetX
    $menuY = [int]$rect.Top + $MenuBarOffsetY

    if ($Mode -eq "debug-menu-walk") {
        # Debug 菜单下拉遍历
        $walkShots = Invoke-DebugMenuWalk -Process $proc -Wshell $wshell -OutputDir $ztemp -Timestamp $ts -MenuX $menuX -MenuY $menuY -WalkCount $DebugMenuWalkCount
        foreach ($s in $walkShots) { $artifacts.Add($s) }
    }
    elseif ($Mode -eq "os-support-sweep") {
        # Debug->OS Support 子菜单遍历
        $probeShots = Invoke-OsSupportSweep -Process $proc -Wshell $wshell -OutputDir $ztemp -Timestamp $ts -MenuX $menuX -MenuY $menuY -DownCountToOsSupport $OsSupportDownCount -ItemCount $OsSupportItemCount
        foreach ($s in $probeShots) { $artifacts.Add($s) }
    }
}

$meta = [ordered]@{
    # 产出元数据，含所有截图路径
    timestamp = (Get-Date).ToString("yyyy-MM-dd HH:mm:ss")
    mode = $Mode
    processId = $proc.Id
    mainWindowTitle = $proc.MainWindowTitle
    screenshots = @($artifacts)
}

$meta | ConvertTo-Json -Depth 5 | Set-Content -Path $metaJson -Encoding UTF8

Write-Host "[ui-capture] meta: $metaJson"
Write-Host "[ui-capture] open: $shotOpen"
Write-Host "[ui-capture] debug: $shotDebug"
if ($Mode -eq "basic") {
    Write-Host "[ui-capture] run: $shotRun"
}
Write-Host "[ui-capture] mode: $Mode"
Write-Host "[ui-capture] artifacts: $($artifacts.Count)"

if (-not $KeepOpen) {
    # 默认调试后自动关闭 Keil
    $null = $wshell.AppActivate($proc.Id)
    Start-Sleep -Milliseconds 200
    $wshell.SendKeys("%{F4}")
}
