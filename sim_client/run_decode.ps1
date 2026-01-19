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
    [string]$OutDir = "out"
)

$ErrorActionPreference = "Stop"
$here = Split-Path -Parent $MyInvocation.MyCommand.Path

if ([string]::IsNullOrWhiteSpace($Pvf)) {
    $candidate = Join-Path $here "..\\Script.pvf"
    if (Test-Path $candidate) { $Pvf = $candidate }
}
if ([string]::IsNullOrWhiteSpace($ImagePacks2)) {
    $candidate = Join-Path $here "..\\wew332\\imagepacks2"
    if (Test-Path $candidate) { $ImagePacks2 = $candidate }
}

& (Join-Path $here "build.ps1")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$exe = Join-Path $here "bin\\sim_client.exe"
if (-not (Test-Path $exe)) { throw "sim_client.exe not found at $exe" }

function Normalize([string]$s) { return $s.Replace('\', '/') }
$PvfArg = Normalize (Resolve-Path $Pvf).Path
$NpkArg = Normalize (Resolve-Path $ImagePacks2).Path
$AniArg = Normalize $Ani
$OutArg = Normalize $OutDir

& $exe "decode-ani" $PvfArg $NpkArg $AniArg $Frames $OutArg
exit $LASTEXITCODE

