# 引き継ぎメモ

## 現在の目的

2人の操作者が、コントローラーを左右に分けて合計4キャラを操作する仕組みを作成中。
現時点では入力分割システムだけを追加し、シーンやPlayerにはまだ接続していない。

## 実装済み

- `STAGE_TYPE::STAGE_TWO`を追加済み。
- `FieldManager`に空の`STAGE_TWO`分岐を追加済み。
- `TwoPlayerInputSystem`を追加済み。
  - コントローラー0：左側→キャラ0、右側→キャラ1
  - コントローラー1：左側→キャラ2、右側→キャラ3
  - 左側：左スティック移動＋L1発射
  - 右側：右スティック移動＋R1発射
- Debug/x64ビルド成功済み。

## 現在の未コミット変更

- `Project_101_BiliBili/TwoPlayerInputSystem.h`
- `Project_101_BiliBili/TwoPlayerInputSystem.cpp`
- `Project_101_BiliBili/Project_101_BiliBili.vcxproj`
- `Project_101_BiliBili/Project_101_BiliBili.vcxproj.filters`

## 次に行うこと

ユーザーの指示があれば、`TwoPlayerScene`側で`TwoPlayerInputSystem::Update()`を呼び、4体の`Player`へ各入力を渡す。
キャラ配置や線の生成はまだ行わない。

## 注意

- 変更は小さく保ち、必要なファイルだけ確認する。
- ファイルごとに文字コードが異なる。特に`FieldManager.cpp`をShift-JISとして保存するとコメントが文字化けするため注意。
- ユーザーは既存の大規模なリファクタリングを一度戻しているため、構造変更は事前に範囲を明確にする。
