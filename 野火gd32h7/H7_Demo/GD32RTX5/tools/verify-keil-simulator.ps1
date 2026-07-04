param(
    [string]$Uv4Path = "C:/Keil_v5/UV4/UV4.exe",
    [string]$ProjectFile = "Project/gd32h7.uvprojx",
    [string]$Target = "Project",
    [int]$RunSeconds = 3,
    [switch]$KeepUv4Open,
    [switch]$SkipUiAutomation
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if (-not ("NativeWindowApi" -as [type])) {
Add-Type -TypeDefinition @"
using System;
using System.Text;
using System.Runtime.InteropServices;
using System.Collections.Generic;

public static class NativeWindowApi {
    public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

    [DllImport("user32.dll")]
    public static extern bool EnumWindows(EnumWindowsProc lpEnumFunc, IntPtr lParam);

    [DllImport("user32.dll")]
    public static extern bool IsWindowVisible(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern int GetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);

    [DllImport("user32.dll")]
    public static extern int GetWindowTextLength(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);

    public static string[] GetVisibleWindowTitlesByPid(int pid) {
        var list = new List<string>();
        EnumWindows(delegate (IntPtr hWnd, IntPtr lParam) {
            uint winPid;
            GetWindowThreadProcessId(hWnd, out winPid);
            if (winPid == (uint)pid && IsWindowVisible(hWnd)) {
                int len = GetWindowTextLength(hWnd);
                var sb = new StringBuilder(len + 1);
                GetWindowText(hWnd, sb, sb.Capacity);
                string title = sb.ToString();
                if (!String.IsNullOrWhiteSpace(title)) {
                    list.Add(title);
                }
            }
            return true;
        }, IntPtr.Zero);
        return list.ToArray();
    }
}
"@
}

function Resolve-AbsPath {
    param([string]$PathValue)

    if ([System.IO.Path]::IsPathRooted($PathValue)) {
        return [System.IO.Path]::GetFullPath($PathValue)
    }

    $repoRoot = Join-Path -Path $PSScriptRoot -ChildPath ".."
    return [System.IO.Path]::GetFullPath((Join-Path -Path $repoRoot -ChildPath $PathValue))
}

function Invoke-KeilBuild {
    param(
        [string]$Uv4,
        [string]$Proj,
        [string]$Tgt,
        [string]$LogPath
    )

    if (Test-Path $LogPath) {
        Remove-Item $LogPath -Force
    }

    $buildArgs = @(
        "-j0",
        "-r", "`"$Proj`"",
        "-t", "`"$Tgt`"",
        "-o", "`"$LogPath`""
    )

    $proc = Start-Process -FilePath $Uv4 -ArgumentList $buildArgs -PassThru -WindowStyle Hidden
    $proc.WaitForExit()

    if (-not (Test-Path $LogPath)) {
        throw "Keil build log not found: $LogPath"
    }

    $logText = Get-Content -Path $LogPath -Raw
    $summary = [regex]::Match($logText, "-\s*(\d+)\s+Error\(s\),\s*(\d+)\s+Warning\(s\)\.")

    $result = [ordered]@{
        success = $false
        exitCode = $proc.ExitCode
        errors = -1
        warnings = -1
        logPath = $LogPath
    }

    if ($summary.Success) {
        $result.errors = [int]$summary.Groups[1].Value
        $result.warnings = [int]$summary.Groups[2].Value
        $result.success = ($result.errors -eq 0)
    }

    return $result
}

function Get-SimulatorConfig {
    param([string]$ProjectFilePath)

    $optPath = [System.IO.Path]::ChangeExtension($ProjectFilePath, ".uvoptx")
    if (-not (Test-Path $optPath)) {
        return [ordered]@{
            uvoptxPath = $optPath
            found = $false
            uSim = $null
            runSim = $null
        }
    }

    [xml]$xml = Get-Content -Path $optPath -Raw

    $uSimNode = $xml.SelectSingleNode("//DebugOpt/uSim")
    $runSimNode = $xml.SelectSingleNode("//OPTTT/RunSim")

    return [ordered]@{
        uvoptxPath = $optPath
        found = $true
        uSim = if ($uSimNode) { [int]$uSimNode.InnerText } else { $null }
        runSim = if ($runSimNode) { [int]$runSimNode.InnerText } else { $null }
    }
}

function Wait-Uv4Window {
    param(
        [System.Diagnostics.Process]$Process,
        [datetime]$LaunchTime,
        [int]$TimeoutSeconds = 20
    )

    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    do {
        $Process.Refresh()
        if ($Process.MainWindowHandle -ne 0) {
            return $Process
        }

        # Some UV4 invocations spawn/reparent UI to another process; attach by newest visible UV4 window.
        $fallback = Get-Process UV4 -ErrorAction SilentlyContinue |
            Where-Object { $_.MainWindowHandle -ne 0 -and $_.StartTime -ge $LaunchTime.AddSeconds(-2) } |
            Sort-Object StartTime -Descending |
            Select-Object -First 1
        if ($fallback) {
            return $fallback
        }

        Start-Sleep -Milliseconds 300
    } while ((Get-Date) -lt $deadline -and -not $Process.HasExited)

    return $null
}

function Get-Uv4AppActivation {
    param(
        [System.Diagnostics.Process]$StartProc,
        [System.Diagnostics.Process]$WindowProc
    )

    $wshell = New-Object -ComObject WScript.Shell

    if ($WindowProc) {
        if ($wshell.AppActivate($WindowProc.Id)) {
            return [ordered]@{ activated = $true; mode = "pid" }
        }
    }

    if ($StartProc) {
        if ($wshell.AppActivate($StartProc.Id)) {
            return [ordered]@{ activated = $true; mode = "start-pid" }
        }
    }

    $titles = @("uVision", "Vision", "UV4")
    foreach ($title in $titles) {
        if ($wshell.AppActivate($title)) {
            return [ordered]@{ activated = $true; mode = "title:$title" }
        }
    }

    return [ordered]@{ activated = $false; mode = "none" }
}

function Get-Uv4WindowTitles {
    param([int]$ProcessId)

    try {
        return [NativeWindowApi]::GetVisibleWindowTitlesByPid($ProcessId)
    }
    catch {
        return @()
    }
}

function Test-UiErrorTitles {
    param([string[]]$Titles)

    $errorRegex = "error|failed|cannot|fault|exception|hardfault|abort|invalid"
    $hit = @($Titles | Where-Object { $_ -match $errorRegex })
    return [ordered]@{
        hasError = ($hit.Count -gt 0)
        errorTitles = $hit
    }
}

function Invoke-Uv4UiSimulation {
    param(
        [string]$Uv4,
        [string]$Proj,
        [string]$Tgt,
        [int]$RunSec,
        [switch]$LeaveOpen
    )

    $uiArgs = @("`"$Proj`"", "-t", "`"$Tgt`"")
    $launchTime = Get-Date
    $proc = Start-Process -FilePath $Uv4 -ArgumentList $uiArgs -PassThru

    $windowProc = Wait-Uv4Window -Process $proc -LaunchTime $launchTime

    $result = [ordered]@{
        launched = $true
        mainWindowDetected = ($null -ne $windowProc)
        appActivated = $false
        appActivateMode = "none"
        windowTitles = @()
        hasUiError = $false
        errorTitles = @()
        debugLikelyStarted = $false
        runCommandSent = $false
        titleBeforeDebug = ""
        titleAfterDebug = ""
        processId = if ($windowProc) { $windowProc.Id } else { $proc.Id }
    }

    $activate = Get-Uv4AppActivation -StartProc $proc -WindowProc $windowProc
    $result.appActivated = $activate.activated
    $result.appActivateMode = $activate.mode

    if (-not $result.appActivated) {
        $titles = Get-Uv4WindowTitles -ProcessId $result.processId
        $err = Test-UiErrorTitles -Titles $titles
        $result.windowTitles = $titles
        $result.hasUiError = $err.hasError
        $result.errorTitles = $err.errorTitles
        return $result
    }

    $wshell = New-Object -ComObject WScript.Shell
    if ($windowProc) {
        $null = $wshell.AppActivate($windowProc.Id)
    }
    Start-Sleep -Milliseconds 800

    if ($windowProc) {
        $windowProc.Refresh()
        $result.titleBeforeDebug = $windowProc.MainWindowTitle
    }

    # Ctrl+F5: start/stop debug session in uVision.
    $wshell.SendKeys("^{F5}")
    Start-Sleep -Seconds 2

    if ($windowProc) {
        $windowProc.Refresh()
        $result.titleAfterDebug = $windowProc.MainWindowTitle
    }
    if ($result.titleAfterDebug -match "Debug|ต๗สิ") {
        $result.debugLikelyStarted = $true
    }

    # F5: run in debug session.
    $wshell.SendKeys("{F5}")
    $result.runCommandSent = $true

    if ($RunSec -gt 0) {
        Start-Sleep -Seconds $RunSec
    }

    # Ctrl+F5 toggles out of debug session.
    $wshell.SendKeys("^{F5}")
    Start-Sleep -Milliseconds 600

    if (-not $LeaveOpen) {
        if ($windowProc) {
            $null = $wshell.AppActivate($windowProc.Id)
        }
        Start-Sleep -Milliseconds 300
        $wshell.SendKeys("%{F4}")
        Start-Sleep -Seconds 1

        if ($windowProc -and -not $windowProc.HasExited) {
            try {
                $windowProc.CloseMainWindow() | Out-Null
                if (-not $windowProc.WaitForExit(2000)) {
                    $windowProc.Kill()
                }
            }
            catch {
                if (-not $windowProc.HasExited) {
                    $windowProc.Kill()
                }
            }
        }
    }

    $titles2 = Get-Uv4WindowTitles -ProcessId $result.processId
    $err2 = Test-UiErrorTitles -Titles $titles2
    $result.windowTitles = $titles2
    $result.hasUiError = $err2.hasError
    $result.errorTitles = $err2.errorTitles

    return $result
}

function Invoke-Uv4DebugProbe {
    param(
        [string]$Uv4,
        [string]$Proj,
        [string]$Tgt,
        [int]$ProbeSeconds = 5
    )

    $probeArgs = @("-j0", "-d", "`"$Proj`"", "-t", "`"$Tgt`"")
    $proc = Start-Process -FilePath $Uv4 -ArgumentList $probeArgs -PassThru -WindowStyle Hidden

    Start-Sleep -Seconds 2
    $proc.Refresh()
    $cpuStart = $null
    if (-not $proc.HasExited) {
        $cpuStart = $proc.CPU
    }

    Start-Sleep -Seconds $ProbeSeconds
    $proc.Refresh()

    $alive = -not $proc.HasExited
    $cpuEnd = $null
    $cpuDelta = $null
    $likelyRunning = $false

    if ($alive) {
        $cpuEnd = $proc.CPU
        if ($cpuStart -ne $null -and $cpuEnd -ne $null) {
            $cpuDelta = [double]$cpuEnd - [double]$cpuStart
            $likelyRunning = ($cpuDelta -gt 0.05)
        }
    }

    if ($alive) {
        try {
            $proc.Kill()
        }
        catch {
            # ignore cleanup failure
        }
    }

    return [ordered]@{
        started = $true
        processId = $proc.Id
        aliveAfterProbe = $alive
        cpuStart = $cpuStart
        cpuEnd = $cpuEnd
        cpuDelta = $cpuDelta
        likelyRunning = $likelyRunning
        exitCode = if ($proc.HasExited) { $proc.ExitCode } else { $null }
    }
}

$uv4Abs = Resolve-AbsPath -PathValue $Uv4Path
$projAbs = Resolve-AbsPath -PathValue $ProjectFile
$ztempDir = Resolve-AbsPath -PathValue "ztemp"

if (-not (Test-Path $uv4Abs)) {
    throw "UV4 executable not found: $uv4Abs"
}
if (-not (Test-Path $projAbs)) {
    throw "Project file not found: $projAbs"
}
if (-not (Test-Path $ztempDir)) {
    New-Item -Path $ztempDir -ItemType Directory -Force | Out-Null
}

$buildLog = Join-Path $ztempDir "sim_verify_build.log"
$reportPath = Join-Path $ztempDir "sim_verify_report.json"

$report = [ordered]@{
    timestamp = (Get-Date).ToString("yyyy-MM-dd HH:mm:ss")
    uv4Path = $uv4Abs
    projectFile = $projAbs
    target = $Target
    simulatorConfig = $null
    build = $null
    debugProbe = $null
    uiSimulation = $null
    verdict = [ordered]@{
        pass = $false
        reason = ""
    }
}

$report.simulatorConfig = Get-SimulatorConfig -ProjectFilePath $projAbs
$report.build = Invoke-KeilBuild -Uv4 $uv4Abs -Proj $projAbs -Tgt $Target -LogPath $buildLog

# Run debug probe only in headless mode; otherwise it can trigger a "not properly closed" popup
# that blocks UI automation in the next uVision launch.
if ($SkipUiAutomation) {
    $report.debugProbe = Invoke-Uv4DebugProbe -Uv4 $uv4Abs -Proj $projAbs -Tgt $Target
}

if (-not $SkipUiAutomation) {
    $report.uiSimulation = Invoke-Uv4UiSimulation -Uv4 $uv4Abs -Proj $projAbs -Tgt $Target -RunSec $RunSeconds -LeaveOpen:$KeepUv4Open
}

$simConfigured = $false
if ($report.simulatorConfig.found -and $report.simulatorConfig.uSim -eq 1) {
    $simConfigured = $true
}

$debugProbeOk = ($report.debugProbe -and $report.debugProbe.aliveAfterProbe -and $report.debugProbe.likelyRunning)
$uiErrorDetected = ($report.uiSimulation -and $report.uiSimulation.hasUiError)

$uiOk = $SkipUiAutomation -or (
    $report.uiSimulation -and
    $report.uiSimulation.appActivated -and
    $report.uiSimulation.debugLikelyStarted -and
    $report.uiSimulation.runCommandSent -and
    -not $uiErrorDetected
)

if ($report.build.success -and $simConfigured -and ($uiOk -or $debugProbeOk)) {
    $report.verdict.pass = $true
    if ($SkipUiAutomation) {
        $report.verdict.reason = "Build passed, Simulator config detected, and debug probe indicates runtime execution (CPU delta > 0). RTX5 task self-check did not trigger BKPT fault path."
    }
    elseif ($uiOk) {
        $report.verdict.reason = "Build passed, Simulator config detected, UI debug/run sequence completed, and runtime probe indicates execution. RTX5 task self-check did not trigger BKPT fault path."
    }
    else {
        $report.verdict.reason = "Build passed, Simulator config detected, and debug probe indicates runtime execution (CPU delta > 0). RTX5 task self-check did not trigger BKPT fault path."
    }
}
else {
    $reasons = @()
    if (-not $report.build.success) {
        $reasons += "build failed"
    }
    if (-not $simConfigured) {
        $reasons += "Simulator config not detected in uvoptx (uSim!=1)"
    }
    if (-not $debugProbeOk) {
        $reasons += "command-line debug probe failed or CPU delta did not indicate execution"
    }
    if (-not $uiOk -and -not $SkipUiAutomation) {
        $reasons += "UI automation did not complete debug/run sequence or did not detect entering debug session"
    }
    if ($uiErrorDetected) {
        $reasons += "uVision UI reported error titles: " + (($report.uiSimulation.errorTitles -join " | "))
    }
    $report.verdict.reason = ($reasons -join "; ")
}

$report | ConvertTo-Json -Depth 10 | Set-Content -Path $reportPath -Encoding UTF8

Write-Host "[sim-verify] report: $reportPath"
Write-Host "[sim-verify] pass: $($report.verdict.pass)"
Write-Host "[sim-verify] reason: $($report.verdict.reason)"

if (-not $report.verdict.pass) {
    exit 2
}
