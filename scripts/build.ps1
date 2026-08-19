[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug',
    [switch]$Rebuild,
    [switch]$NoRestore
)

$ErrorActionPreference = 'Stop'
$repositoryRoot = Split-Path -Parent $PSScriptRoot
$solutionPath = Join-Path $repositoryRoot 'Project_101_BiliBili.sln'
$vswherePath = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'

if (-not (Test-Path -LiteralPath $vswherePath)) {
    throw 'vswhere.exe が見つかりません。Visual Studio 2022以降をインストールしてください。'
}

$msbuildPath = & $vswherePath -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
if (-not $msbuildPath -or -not (Test-Path -LiteralPath $msbuildPath)) {
    throw 'MSBuild.exe が見つかりません。Visual Studio Installerで「C++によるデスクトップ開発」を追加してください。'
}

Push-Location $repositoryRoot
try {
    if (-not $NoRestore) {
        Write-Host 'Restoring NuGet packages...'
        & $msbuildPath $solutionPath /t:Restore /p:RestorePackagesConfig=true /p:Configuration=$Configuration /p:Platform=x64 /m /v:minimal
        if ($LASTEXITCODE -ne 0) {
            throw "NuGet package restore failed with exit code $LASTEXITCODE."
        }
    }

    $target = if ($Rebuild) { 'Rebuild' } else { 'Build' }
    Write-Host "Building $Configuration|x64 ($target)..."
    & $msbuildPath $solutionPath /t:$target /p:Configuration=$Configuration /p:Platform=x64 /m /v:minimal
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code $LASTEXITCODE."
    }
}
finally {
    Pop-Location
}

Write-Host "Build completed: $Configuration|x64"
