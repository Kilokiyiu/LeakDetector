# 在 examples/array_leak 目录下执行
$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $Root "cmake-build-debug"

function Import-VcVars {
    if (Get-Command cl -ErrorAction SilentlyContinue) { return }
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) { return }
    $vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $vs) { return }
    $vcvars = Join-Path $vs "VC\Auxiliary\Build\vcvars64.bat"
    if (-not (Test-Path $vcvars)) { return }
    Write-Host "[env] Loading: $vcvars"
    cmd /c "`"$vcvars`" && set" | ForEach-Object {
        if ($_ -match '^(?<key>[^=]+)=(?<val>.*)$') {
            Set-Item -Path "env:$($Matches.key)" -Value $Matches.val
        }
    }
}

Import-VcVars

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "cmake not found. Install VS C++ workload or add CMake to PATH."
}
if (-not (Get-Command cl -ErrorAction SilentlyContinue)) {
    Write-Error "cl.exe not found. Open Rider Terminal from x64 Native Tools prompt, or install VS C++."
}

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

Write-Host "[1/2] CMake configure..."
cmake -S $Root -B $BuildDir -G Ninja -DCMAKE_BUILD_TYPE=Debug

Write-Host "[2/2] Build array_leak..."
cmake --build $BuildDir --target array_leak

$Exe = Join-Path $BuildDir "array_leak.exe"
if (-not (Test-Path $Exe)) {
    Write-Error "Build failed: $Exe not found"
}

Write-Host ""
Write-Host "Running: $Exe"
Write-Host ""
& $Exe
