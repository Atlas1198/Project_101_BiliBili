[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release',
    [string]$OutputDirectory
)

$ErrorActionPreference = 'Stop'
$repositoryRoot = Split-Path -Parent $PSScriptRoot

if (-not $OutputDirectory) {
    $OutputDirectory = Join-Path $repositoryRoot "dist\$Configuration"
}
elseif (-not [IO.Path]::IsPathRooted($OutputDirectory)) {
    $OutputDirectory = Join-Path $repositoryRoot $OutputDirectory
}

$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
$distRoot = [IO.Path]::GetFullPath((Join-Path $repositoryRoot 'dist'))
if (-not $OutputDirectory.StartsWith($distRoot + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) {
    throw "OutputDirectory はリポジトリ内の dist 以下を指定してください: $distRoot"
}

& (Join-Path $PSScriptRoot 'build.ps1') -Configuration $Configuration
if ($LASTEXITCODE -ne 0) {
    throw "Build script failed with exit code $LASTEXITCODE."
}

$buildDirectory = Join-Path $repositoryRoot "x64\$Configuration"
$executablePath = Join-Path $buildDirectory 'Project_101_BiliBili.exe'
$assetDirectory = Join-Path $repositoryRoot 'Project_101_BiliBili\asset'

if (-not (Test-Path -LiteralPath $executablePath)) {
    throw "Executable not found: $executablePath"
}
if (-not (Test-Path -LiteralPath $assetDirectory)) {
    throw "Asset directory not found: $assetDirectory"
}

if (Test-Path -LiteralPath $OutputDirectory) {
    Remove-Item -LiteralPath $OutputDirectory -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
Copy-Item -LiteralPath $executablePath -Destination $OutputDirectory -Force

Get-ChildItem -LiteralPath $buildDirectory -File -Filter '*.dll' | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination $OutputDirectory -Force
}

$outputAssetDirectory = Join-Path $OutputDirectory 'asset'
Copy-Item -LiteralPath $assetDirectory -Destination $OutputDirectory -Recurse -Force

$requiredFiles = @(
    (Join-Path $OutputDirectory 'Project_101_BiliBili.exe'),
    (Join-Path $outputAssetDirectory 'defaults.json'),
    (Join-Path $outputAssetDirectory 'shader\VertexShader.hlsl'),
    (Join-Path $outputAssetDirectory 'shader\PixelShader.hlsl')
)

$missingFiles = $requiredFiles | Where-Object { -not (Test-Path -LiteralPath $_) }
if ($missingFiles) {
    throw "Package validation failed. Missing files:`n$($missingFiles -join "`n")"
}

Write-Host "Package completed: $OutputDirectory"
