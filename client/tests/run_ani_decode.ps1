param(
    [Parameter(Mandatory = $false)]
    [string]$Pvf = "",

    [Parameter(Mandatory = $false)]
    [string]$ImagePacks2 = "",

    [Parameter(Mandatory = $false)]
    [string]$Ani = "creature/common/animation/levelup_dodgeunder.ani",

    [Parameter(Mandatory = $false)]
    [int]$Frames = 5,

    [Parameter(Mandatory = $false)]
    [string]$Msys2Root = "C:\\msys64"
)

$ErrorActionPreference = "Stop"

function Normalize-PvfPath([string]$s) {
    if ([string]::IsNullOrWhiteSpace($s)) { return $s }
    return $s.Replace('\', '/')
}

$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$exe = Join-Path $here "test_ani_decode.exe"

if ([string]::IsNullOrWhiteSpace($Pvf)) {
    $candidate = Join-Path $here "..\\..\\Script.pvf"
    if (Test-Path $candidate) { $Pvf = $candidate }
}
if ([string]::IsNullOrWhiteSpace($ImagePacks2)) {
    $candidate = Join-Path $here "..\\..\\wew332\\imagepacks2"
    if (Test-Path $candidate) { $ImagePacks2 = $candidate }
}

if ([string]::IsNullOrWhiteSpace($Pvf) -or -not (Test-Path $Pvf)) {
    throw "PVF not found. Pass -Pvf <path_to_Script.pvf>."
}
if ([string]::IsNullOrWhiteSpace($ImagePacks2) -or -not (Test-Path $ImagePacks2)) {
    throw "ImagePacks2 dir not found. Pass -ImagePacks2 <dir>."
}

$ucrt64Bin = Join-Path $Msys2Root "ucrt64\\bin"
if (-not (Test-Path $ucrt64Bin)) {
    throw "MSYS2 UCRT64 bin not found at '$ucrt64Bin'. Install MSYS2 (UCRT64) or pass -Msys2Root."
}

# Ensure required runtime DLLs are discoverable
$env:PATH = "$ucrt64Bin;$env:PATH"

if (-not (Test-Path $exe)) {
    $make = Join-Path $ucrt64Bin "mingw32-make.exe"
    if (-not (Test-Path $make)) {
        throw "mingw32-make.exe not found at '$make'."
    }
    Push-Location $here
    try {
        & $make "test_ani_decode.exe"
        if ($LASTEXITCODE -ne 0) { throw "Build failed (exit $LASTEXITCODE)." }
    } finally {
        Pop-Location
    }
}

# Avoid needing PATH when double-click / plain PowerShell: copy zlib1.dll next to the exe if missing
$zlibDll = Join-Path $here "zlib1.dll"
if (-not (Test-Path $zlibDll)) {
    $src = Join-Path $ucrt64Bin "zlib1.dll"
    if (Test-Path $src) {
        Copy-Item -Force $src $zlibDll
    }
}

$PvfArg = Normalize-PvfPath (Resolve-Path $Pvf).Path
$NpkArg = Normalize-PvfPath (Resolve-Path $ImagePacks2).Path
$AniArg = Normalize-PvfPath $Ani

Write-Host "Running: $exe"
Write-Host "  PVF: $PvfArg"
Write-Host "  ImagePacks2: $NpkArg"
Write-Host "  ANI: $AniArg"
Write-Host "  Frames/action: $Frames"

& $exe $PvfArg $NpkArg $AniArg $Frames
exit $LASTEXITCODE

