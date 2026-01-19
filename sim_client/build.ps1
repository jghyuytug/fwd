param(
    [Parameter(Mandatory = $false)]
    [string]$Msys2Root = "C:\\msys64"
)

$ErrorActionPreference = "Stop"
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$ucrt64 = Join-Path $Msys2Root "ucrt64\\bin"
if (-not (Test-Path $ucrt64)) { throw "MSYS2 UCRT64 bin not found: $ucrt64" }

$env:PATH = "$ucrt64;$env:PATH"

Push-Location $here
try {
    mingw32-make.exe all
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    # Copy zlib1.dll next to exe for stable double-click runs
    $dll = Join-Path $ucrt64 "zlib1.dll"
    $dst = Join-Path $here "bin\\zlib1.dll"
    if (Test-Path $dll) {
        Copy-Item -Force $dll $dst
    }
} finally {
    Pop-Location
}

