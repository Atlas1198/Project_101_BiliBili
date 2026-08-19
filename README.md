# Project 101 BiliBili

ゲームクリエイター甲子園2026への提出に向けて開発中のWindows x64向けDirectX 12ゲームです。

## 対応環境

- Windows 10またはWindows 11（64 bit）
- Visual Studio 2022以降
- Visual Studio Installerの「C++によるデスクトップ開発」
- Windows 10/11 SDK
- PowerShell 7またはWindows PowerShell 5.1

Win32/x86はサポートしません。正式な構成は `Debug|x64` と `Release|x64` です。

## ビルド

リポジトリルートで次を実行します。

```powershell
.\scripts\build.ps1 -Configuration Debug
```

Releaseビルド:

```powershell
.\scripts\build.ps1 -Configuration Release
```

スクリプトはVisual Studio付属のMSBuildを自動検出し、最初にNuGetから固定バージョンのAssimpを `packages/` へ復元します。`packages/` はGit管理対象外です。

依存関係をすでに復元済みの場合:

```powershell
.\scripts\build.ps1 -Configuration Debug -NoRestore
```

## 配布物の生成

```powershell
.\scripts\package.ps1 -Configuration Release
```

`dist/Release/` に次を生成します。

- `Project_101_BiliBili.exe`
- 実行に必要なDLL
- `asset/` 一式

配布・試遊にはexeだけでなく、生成されたフォルダー全体を使用してください。

## 起動

ビルド出力から直接起動する場合、作業ディレクトリを `Project_101_BiliBili/`（`asset/` があるディレクトリ）にしてください。提出・別PC確認では `scripts/package.ps1` で生成した配布物を使用してください。

## 任意の実験機能

- `Server` プロジェクトとオンライン通信コードは残していますが、Solutionの通常ビルド対象には含めていません。
- Firebaseによる外部パラメータ更新はクライアントのビルド対象から外しています。
- 現在の提出対象はオフライン版です。

## トラブルシューティング

### AssimpCpp.targetsが見つからない

`-NoRestore` を付けずにビルドしてください。解決しない場合は、Visual Studio InstallerでNuGetとC++デスクトップ開発環境が導入されているか確認してください。

### asset folder not foundと表示される

`scripts/package.ps1` で配布物を作るか、`asset/` が存在するディレクトリを作業ディレクトリとして起動してください。

## 監査資料

現状分析と優先修正項目は [PROJECT_AUDIT_2026-08-20.md](PROJECT_AUDIT_2026-08-20.md) を参照してください。
