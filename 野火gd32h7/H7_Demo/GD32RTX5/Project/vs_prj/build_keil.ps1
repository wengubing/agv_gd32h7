param(
	[string]$KeilExe = "C:\Keil_v5\UV4\UV4.exe",
	[string]$ProjectFile = "",
	[string]$LogFile = ""
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($ProjectFile)) {
	$ProjectFile = Join-Path (Resolve-Path (Join-Path $PSScriptRoot "..")).Path "gd32h7.uvprojx"
}

if ([string]::IsNullOrWhiteSpace($LogFile)) {
	$LogFile = Join-Path (Split-Path -Parent $ProjectFile) "keil_build.log"
}

if (-not (Test-Path $KeilExe)) {
	throw "未找到 Keil 可执行文件: $KeilExe"
}

if (-not (Test-Path $ProjectFile)) {
	throw "未找到 Keil 工程文件: $ProjectFile"
}

Write-Host "Keil: $KeilExe"
Write-Host "Project: $ProjectFile"
Write-Host "Log: $LogFile"

& $KeilExe -b $ProjectFile -j0 -o $LogFile

if (-not (Test-Path $LogFile)) {
	throw "构建完成但未生成日志: $LogFile"
}

Get-Content -Path $LogFile

$log = Get-Content -Path $LogFile -Raw
if ($log -match "-\s*0 Error\(s\)") {
	Write-Host "Keil 编译成功。"
	exit 0
}

Write-Error "Keil 编译失败，请检查日志: $LogFile"
exit 1
