# Builds Gravity Defied as a PS Vita VPK.
#
# Requirements:
#   - VitaSDK installed and VITASDK environment variable pointing to it
#     (https://vitasdk.org). C:\vitasdk is auto-detected.
#   - vita packages installed into the SDK:
#       sdl2, sdl2_ttf   (via vdpm or from https://github.com/vitasdk/packages)
#     SDL2_image is expected to be installed as well (PNG support only is
#     enough for this game).
#   - CMake >= 3.24 and Ninja.

$ErrorActionPreference = "Stop"

if (-not $env:VITASDK) {
    if (Test-Path "C:\vitasdk") {
        $env:VITASDK = "C:\vitasdk"
    } else {
        throw "VITASDK environment variable is not set and C:\vitasdk was not found"
    }
}

$toolchain = Join-Path $env:VITASDK "share/vita.toolchain.cmake"
$buildDir = Join-Path $PSScriptRoot "build-vita"

cmake -S $PSScriptRoot -B $buildDir -G Ninja `
    -DCMAKE_TOOLCHAIN_FILE="$toolchain" `
    -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed" }

cmake --build $buildDir
if ($LASTEXITCODE -ne 0) { throw "Build failed" }

$vpk = Join-Path $buildDir "GravityDefied.vpk"
if (Test-Path $vpk) {
    Write-Host ""
    Write-Host "VPK created: $vpk"
    Write-Host "Install it on a Vita with VitaShell, then launch from LiveArea."
} else {
    throw "VPK was not produced"
}
