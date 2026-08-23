param(
    [string]$Configuration = "Release",
    [string]$PackageName = "BiliBili_Windows_x64_20260822"
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $repoRoot "x64\$Configuration"
$sourceExe = Join-Path $buildDir "Project_101_BiliBili.exe"
$sourceAssimp = Join-Path $buildDir "assimp-vc142-mt.dll"
$sourceAsset = Join-Path $repoRoot "Project_101_BiliBili\asset"
$distDir = Join-Path $repoRoot "dist"
$packageDir = Join-Path $distDir $PackageName
$zipPath = "$packageDir.zip"

foreach ($requiredPath in @($sourceExe, $sourceAssimp, $sourceAsset)) {
    if (-not (Test-Path -LiteralPath $requiredPath)) {
        throw "Required release input was not found: $requiredPath"
    }
}

if ((Test-Path -LiteralPath $packageDir) -or (Test-Path -LiteralPath $zipPath)) {
    throw "Package output already exists. Choose another PackageName: $PackageName"
}

New-Item -ItemType Directory -Path $packageDir -Force | Out-Null
Copy-Item -LiteralPath $sourceExe -Destination (Join-Path $packageDir "BiliBili.exe")
Copy-Item -LiteralPath $sourceAssimp -Destination $packageDir
Copy-Item -LiteralPath $sourceAsset -Destination $packageDir -Recurse

$readme = @"
BiliBili Windows x64 build

Start:
  Run BiliBili.exe.

Requirements:
  - Windows 10/11 64-bit
  - DirectX 12 compatible GPU and current graphics driver
  - Four XInput-compatible controllers for the current game mode

Keep BiliBili.exe, assimp-vc142-mt.dll, and the asset folder together.
"@
Set-Content -LiteralPath (Join-Path $packageDir "README.txt") -Value $readme -Encoding UTF8

$manifestPath = Join-Path $packageDir "SHA256SUMS.txt"
$manifestLines = Get-ChildItem -LiteralPath $packageDir -Recurse -File |
    Where-Object { $_.FullName -ne $manifestPath } |
    Sort-Object FullName |
    ForEach-Object {
        $relativePath = [System.IO.Path]::GetRelativePath($packageDir, $_.FullName)
        $hash = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
        "$hash  $relativePath"
    }
Set-Content -LiteralPath $manifestPath -Value $manifestLines -Encoding UTF8

Compress-Archive -LiteralPath $packageDir -DestinationPath $zipPath -CompressionLevel Optimal

$packageBytes = (Get-ChildItem -LiteralPath $packageDir -Recurse -File | Measure-Object Length -Sum).Sum
$zipBytes = (Get-Item -LiteralPath $zipPath).Length

[pscustomobject]@{
    PackageDirectory = $packageDir
    ZipFile = $zipPath
    PackageBytes = $packageBytes
    ZipBytes = $zipBytes
}
