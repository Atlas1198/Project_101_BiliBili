# Project 101 BiliBili 実装・開発環境監査

監査日: 2026-08-20  
対象: `Project_101_BiliBili.sln` のクライアント、Common、Server、同梱アセット  
目的: ゲームクリエイター甲子園2026への提出と、1～2人用モード追加に先立つ現状把握

## 1. 結論

このプロジェクトは「短期間で完成させた4人専用ゲーム」として必要な機能を一通り備えており、9月から既存実装を継続する判断は妥当である。DirectX 12基盤を別エンジンへ載せ替えるより、既存基盤を限定的に安定化し、ゲームモード層を追加する方が締切リスクは低い。

一方、現在のまま新モード実装へ入るのは危険である。最優先の問題は次の5点。

1. クリーン環境でクライアントをビルドできない。NuGet依存が復元されておらず、再現可能なセットアップ手順もない。
2. 4人・2対2・各チーム2人という前提が、入力、選択画面、ゲーム進行、BB、UIに分散している。
3. シーン再入場時にオブジェクトを解放せず配列だけ消す箇所が多く、2試合目以降のリークと状態残留が大きい。
4. 初期化、ファイル読込、DirectX APIの失敗を上位へ返さないため、環境差が即クラッシュになりやすい。
5. テスト、CI、README、配布物生成がなく、「誰のPCでも同じ手順でビルド・起動できる」という保証がない。

したがって、最初の1～2週間は汎用エンジン化ではなく、ビルド・配布・人数構成・再試合の4点に絞った安定化へ使うべきである。

## 2. 調査範囲と制約

- 独自コードは約205ファイル、約29,566行（巨大な同梱ヘッダ類を除外）。
- 大きな実装単位は `CollisionManager.cpp` 2,115行、`FieldManager.cpp` 1,581行、`Renderer.cpp` 767行、`GameScene.cpp` 592行。
- アセットは214ファイル、約367.6MB。
- 追跡ファイルは1,070個。ソースだけでなくAsio、json.hpp、d3dx12.h、DirectXTexのヘッダ・libをリポジトリに同梱している。
- 自動テスト、CI定義、README、ライセンス一覧は見つからなかった。
- Debug x64ビルドを実行したが、`packages/AssimpCpp.5.0.1.6` がないためクライアントのコンパイル前に停止した。Serverはビルドできたが、`size_t` から `uint32_t` への縮小変換警告が2件出た。
- パッケージ復元は監査対象を変更するため実施していない。このため、クライアントの全コンパイル・起動・実プレイは未確認。

## 3. 応募要項から見た要件

[ゲームクリエイター甲子園2026 応募要項](https://game.creators-guild.com/gck2026-terms/)で、今回のプロジェクトに直接関係する点は以下。

- エントリー・チーム登録: 2026-08-01～2026-10-31 16:59。
- 作品提出: 2026-09-01～2026-11-07 16:59。期間中は再提出可能。
- 最終審査締切: 2026-11-07 16:59。
- ゲーム部門の提出物: 実行ファイル、ソースコード、15～90秒程度の紹介・プレイ映像、サムネイル。
- ゲーム画面と映像には学校名・個人名を入れない。
- 特殊な環境を必要とせず、幅広い環境で確認できる作品ほど審査上有利。
- 二次創作は禁止。画像、デザイン、サウンドを含む素材の権利確認が必要。
- 審査軸は独自性、分かりやすさ、ゲームとしての成立、プレイ体感、突き詰め、商品としての完成度。

提出時にソースコードが必須であるため、ビルド再現性、不要ファイル、第三者ライセンスの整理は内部事情ではなく提出品質の一部になる。

## 4. 現在のアーキテクチャ

### 4.1 起動とメインループ

`main.cpp` → `App` → `Engine` / `Renderer` / 各Managerの順で起動する。

- Win32のボーダーレス全画面ウィンドウ。
- 内部フレームバッファは1920×1080固定。
- `timeGetTime()` を用いて約60Hzに制限。
- 更新、描画、入力の前フレーム状態コピーを1ループ内で処理。
- オフラインモードが固定で有効。オンライン接続とFirebaseパラメータ更新はコメントアウトされている。

### 4.2 描画・アセット

- DirectX 12、ダブルバッファ、深度バッファ、ポストプロセス用レンダーターゲット。
- 描画要求をワールド・スクリーン・ポストプロセスのリストへ集め、PSOや深度でソートして描画。
- テクスチャはDirectXTex、FBXはAssimpを使用。
- 各オブジェクト用定数バッファは必要数まで動的に増える。
- アセットパスは多数のクラスへ文字列リテラルとして分散。

### 4.3 シーン

`SceneManager` が次のシーンを起動時にすべて生成し、切替時に同じインスタンスを再初期化する。

1. Title
2. Controller connection
3. Stage selection
4. Character selection
5. Game / Result

各シーンはCamera、CollisionManager、EffectManagerを持つ。シーン変更イベントを予約し、更新末尾で切り替える設計は、更新中の破棄を避けられる点で良い。

### 4.4 ゲーム本体

`GameScene` 配下に以下がある。

- PlayerManager
- FieldManager
- BulletManager
- ItemManager
- BBManager（チーム連携・変身系）
- GameEventManager
- GameUIManager

ゲーム進行は「開始前 → カウントダウン → プレイ → リザルト」のBehaviorで管理される。フィールドは3種類。プレイヤーは4人を生成し、0・1を青、2・3を赤として相互にチームメイトを束縛する。

### 4.5 入力・音声・通信

- XInputコントローラー4台とキーボード。
- XAudio2でBGM、SE、ループSEを管理。
- 独自イベントバスでUI・ゲームロジック間を接続。
- Asioベースのクライアント/サーバー実装とFirebase/curl実装が残るが、現在の通常フローでは無効。

## 5. 確認できた不具合・高確度の欠陥

### P0: 新モード実装前に対処

#### P0-1. クリーンビルド不能

**対応済み（2026-08-20）:** 正式構成を x64 の Debug/Release に限定し、Assimp の復元・ビルド・配布物生成を PowerShell スクリプトへ統一した。Firebase/curl は既定クライアントから、Server は Solution の通常ビルドから分離した。両構成のビルド、Release 配布物生成、配布物からの短時間起動を確認済み。

- `Project_101_BiliBili.vcxproj` は `AssimpCpp.5.0.1.6` と `curl-static.1.0.6` を `packages/` から読む。
- `.gitignore` は `packages/` を除外しており、現在の作業ツリーにパッケージがない。
- Debug x64ビルドはAssimpのtargets不足で停止した。
- 利用していないFirebase機能のためにもcurlが必須になっている。

影響: 新メンバーの参加、別PC、提出直前の再ビルドで詰まる。  
推奨: x64だけを正式構成にし、依存取得を1コマンド化する。使わないServer/Firebaseは既定クライアントから外す。セットアップと配布手順をREADMEへ固定する。

#### P0-2. 初期化失敗を無視して続行

- `Engine::InitCore()` はアセット不足時に `false` を返すが、`App::InitInstance()` は戻り値を無視する（`App.cpp:289`）。
- `CreateDevice()` など多数のDirectX生成処理はHRESULTを保存するだけで、失敗しても次の処理でnullを参照し得る（`Engine.cpp:214-342`）。
- `App::Initialize()` は常にtrueを返す（`App.cpp:88-97`）。
- ウィンドウ登録・生成、Audio初期化、Descriptor Heap生成も失敗を上位へ通知しない。

影響: GPU、Windows、音声デバイス、実行場所の差が「説明のないクラッシュ」になる。審査PCで最も避けたい種類の障害。

#### P0-3. 設定JSONの欠落・破損で例外終了

`LoadParametersJSON()` は相対パス `asset/defaults.json` を直接開き、ファイルオープンやJSON parse例外を処理しない（`App.cpp:511-524`）。`ContentRoot` を導入している一方、実際の読込では利用していない。

影響: exeの起動ディレクトリが違うだけで起動時クラッシュし得る。

#### P0-4. 1～2人モードでは配列範囲外アクセスになる

- `GameScene::SpawnPlayers()` は4人生成と2対2の束縛を固定実装（`GameScene.cpp:161-185`）。
- 開始演出とカウントダウンは `players[0]`～`players[3]` を直接参照（`GameScene.cpp:266-326`）。
- `BBManager::SetPlayerData()` は各チームに必ず2人いるとして `[0]`, `[1]` を読む（`BBManager.cpp:281-307`）。
- ControllerConnecterは4人全員の接続まで進行不可（`ControllerConnecter.cpp:155-163`）。
- CharacterSelecter、PlayerManager、各UIにも4固定の配列・ループが多数ある。
- 4人前提を表す記述は簡易検索だけでも87箇所。

影響: 人数を減らすだけでは動かず、クラッシュまたは不正なチーム参照になる。

#### P0-5. シーン再入場で大量リークと状態残留

同じGameSceneを再利用する一方、Finalizeでraw pointerをdeleteせずvectorをclearする箇所がある。

- `FieldManager::FinalizeOverride()` は全オブジェクト配列をclearするだけ（`FieldManager.cpp:1398-1423`）。
- `ItemManager::FinalizeOverride()` もアイテムをdeleteせずclear（`ItemManager.cpp:213-218`）。
- `BBManager::InitializeOverride()` は再入場ごとにBB/BBAreaをnewして既存ポインタを上書きする（`BBManager.cpp:25-87`）。
- `GameScene::InitializeOverride()` はBehaviorを毎回newし、以前のBehaviorを解放しない（`GameScene.cpp:69-79`）。
- `CollisionManager::ClearColliders()` は登録配列だけを消し、前回衝突ペアを消さない（`CollisionManager.cpp:470-473`）。

影響: タイトルへ戻って2試合目を始める操作で、メモリ増加、古い状態、衝突状態の混在が起きる。開発中の反復テストにも悪影響。

#### P0-6. オブジェクト共通部の恒久リーク

- `NodeAnimatorSet` のコンストラクタで `NodeAnimator` をnewした後、`ObjectBase` コンストラクタでも再度newして最初のポインタを失う（`ObjectBase.h:13`, `ObjectBase.cpp:46`）。
- `ObjectBase` デストラクタはColliderSetをdeleteせず、削除フラグを立ててポインタをnullにする（`ObjectBase.cpp:49-58`）。
- SceneBaseはCamera、CollisionManager、EffectManagerをnewするが、デストラクタは空（`SceneBase.cpp:8-13`, `SceneBase.h:23`）。
- App終了時もEngine、InputManager、TextureManager、MeshManagerをdeleteしない（`App.cpp:212-225`）。

影響: 1試合中の弾・アイテム生成と、シーン再入場でメモリ使用量が増え続ける。

#### P0-7. WAVローダーがfmtチャンク長を信頼して固定領域へ書く

`AudioManager::LoadWav()` は `SoundData data;` を未初期化のまま作り、WAVの `fmt ` チャンクサイズ分を `WAVEFORMATEX` へ直接readする（`AudioManager.cpp:61-97`）。標準PCMの16 byteでは残りが未初期化になり、WAVEFORMATEXTENSIBLE等の18 byte超では隣接メモリを破壊し得る。RIFF/WAVE識別子、チャンク境界、奇数長paddingも検証しない。

影響: 音源差し替えだけで起動時メモリ破壊や音声初期化失敗が起こり得る。

### P1: 9月前半までに対処

#### P1-1. 時間管理が3方式混在

- Appは60Hz相当のフレーム制限だが、遅れを蓄積せず毎回 `dwExecLastTime = now` とする。
- ゲーム進行・移動・重力・演出の多くは「1更新=1フレーム」前提。
- BB、弾回復、アイテム等は `steady_clock` の実時間を使用。
- カウントダウンは60Hzループ内で `FRAMES_PER_SECOND = 40`（`GameScene.cpp:309-380`）。

影響: 高負荷時にフレーム依存部分だけ遅くなり、実時間タイマーとの同期が崩れる。PC性能差でプレイ感が変わる。

短期対策: 全コードのdelta time化ではなく、固定60Hzシミュレーションのaccumulatorと最大catch-up回数をAppへ導入し、既存フレーム定数を維持する。

#### P1-2. キーボード `up` 状態がキーごとに逆

`Keyboard_IsKeyUp()` は「現在押されていない」を返すが、WASD/P/矢印/Space/Enterでは否定し、T/F/G等では否定せず代入する（`InputManager.cpp:185-240`）。同じ `InputState::up` がキーにより「押下中」と「非押下」を意味する。

影響: 現状ほぼ未使用でも、追加モードで入力を再利用した際に不具合を生む。

#### P1-3. 既定テクスチャがGPUへコピーされない

`TextureManager::CreateDefaultTexture()` は1×1テクスチャとUpload Bufferを作るが、`defaultPixel` をUpload Bufferへ書かず、コピーコマンドもバリアも発行しない（`TextureManager.cpp:334-382`）。通常テクスチャの失敗時はこの既定SRVへフォールバックするため、失敗時表示自体が未定義状態。

#### P1-4. TextureManagerのHRESULT確認不足

Descriptor HeapやCommitted Resourceの生成結果を確認せず、失敗後もnull resourceで `GetRequiredIntermediateSize()` 等へ進む（`TextureManager.cpp:15-40`, `91-130`）。メモリ不足や非対応フォーマットがクラッシュへ直結する。

#### P1-5. ループSEがStopAllの対象外

`AudioManager::StopAll()` はBGMと通常SEしか止めず、`LoopSEVoices` を解放しない（`AudioManager.cpp:213-236`）。シーン遷移や終了時にループ音が残る経路がある。

#### P1-6. Controller振動APIに範囲・値チェックがない

`Controller::SetVibration()` はindexを検証せず配列へアクセスし、0～1外のfloatをWORDへ直接変換する（`Controller.cpp:88-95`）。現在は0～3のIDを想定するが、人数・入力割当変更時に危険。

#### P1-7. アセット参照がコードへ分散

176個以上のアセット文字列参照がソースへ埋め込まれている。現時点で見つかった欠落参照7件のうち多くは未使用・コメントだが、次のような死んだ実装も残る。

- `TransformItemManager` はどこからも生成されず、使用すると初期化直後に空の `m_items[0]` を読む。
- 旧Player/Field用の存在しないパスがヘッダに残る。
- `.orig`、Visual Studio競合バックアップ4ファイル、4MBのpreprocessedファイルが追跡されている。

影響: ファイル名変更、デザイナー差替え、配布物検証が手作業になり、不要物がソース提出物へ混入する。

#### P1-8. サポートしていない構成がSolutionに露出

Win32構成にはx64で設定しているAsio/Common include path、DirectXTex include/lib設定がなく、同等にビルドできる構成ではない。提出対象がx64のみならWin32を削除または明確に非サポート化すべき。

### P2: 余力があれば改善

- `CollisionManager::BroadPhase()` は全コライダーの総当たり O(n²)。弾数や敵数を大幅に増やす新モードでは負荷源になる。
- EventManagerは型とEventTypeの対応を保証せず、誤った型でTriggerすると未定義動作。購読tokenのRAIIもない。
- `ClearAllEvent()` で全購読を破棄するため、永続システムとシーンローカル購読を共存しにくい。
- `App`、Audio、Event、Database、ContentRoot、StageSelector等にSingleton/グローバル状態が多く、テストと再初期化が難しい。
- デバッグマクロが `_DEBUG` と `DEBUG` で不統一。`GameScene.cpp` のデバッグカメラ部は通常のDebug構成でコンパイルされない。
- `CollisionManager.cpp`、`FieldManager.cpp` の巨大化により複数人編集時の競合が起きやすい。
- オンライン実装は受信bodyサイズ上限やデシリアライズ時の長さ検査がなく、外部公開できる品質ではない。今回使わないなら隔離する。
- Firebase URLとリアルタイム更新コードがクライアントへ残る。使わない機能が依存・スレッド・障害点を増やしている。

## 6. 1～2人用モード追加に必要な設計境界

「既存の4人モードを可変人数化する」だけでは既存挙動を壊す範囲が広い。短期では、共通基盤の上にMode Rulesを置き、4人モードもその設定の1つとして表現するのが安全。

最低限、次の値を1つの `MatchConfig` 相当へ集約する。

- mode ID
- human player count
- active player slots
- controller assignment
- team assignment
- spawn point
- character assignment
- victory condition
- enabled systems（BB、item、event、friendly fire等）

プレイヤー数を数値だけで渡さず、`PlayerSlot` の配列を作る。ゲーム側は「players[2]が赤1人目」ではなく、active slotやteam queryから対象を得る。

特に分離が必要な箇所:

1. ControllerConnecter: 必要人数に達したら進める。
2. CharacterSelecter/UI: active slotだけ表示・操作。
3. SpawnPlayers: MatchConfigから生成。
4. Team/teammate: 1人チームではnullを許容するか、モードでBBを無効化。
5. GameSceneの開始演出: インデックス直参照をplayer iterationへ変更。
6. BBManager: 2人揃うことを前提にせず、人数0/1/2を処理。
7. 勝利条件とResultUI: 2対2のHP/勝者表現から独立。

追加モードの仕様が未定でも、この境界だけは先に作れる。逆に、敵AI、協力、スコアアタック等の仕様を決める前に汎用Entity/ECSへ全面改修する必要はない。

## 7. 推奨する短期ロードマップ

### フェーズA: 土台固定（8月末～9月第1週）

完了条件:

- 新しいPC/ユーザーでREADME通りにDebug/Release x64をビルドできる。
- `dist/` 相当へexe、DLL、asset、必要文書を一括生成できる。
- 起動場所を変えても動作する。
- 必須ファイル不足時はクラッシュせず、ファイル名付きエラーを出して終了する。
- タイトル→1試合→タイトル→2試合目を繰り返して状態が残らない。
- 4人モードの現状挙動を動画とチェックリストで基準化する。

実施項目:

- NuGet/依存関係固定、x64構成整理、不要なcurl/Server依存の分離。
- ContentRootの全ローダー適用とasset manifest検証。
- 初期化結果の伝播、JSON/WAV/テクスチャ読込防御。
- GameScene再初期化と所有権修正。まず試合中に生成されるオブジェクトからunique_ptr化。
- 不要な競合残骸・preprocessed・未使用ソースの隔離。
- 最小限のログファイルまたはエラーダイアログ。

### フェーズB: モード境界（9月第1～2週）

- MatchConfig / PlayerSlot / Team構成を導入。
- 既存4人モードを新設定経由で動かし、見た目と挙動を維持。
- 人数0/1/2/4の境界をロジックテスト。
- Controller/Character/Game/Resultのactive slot対応。

この段階で「既存4人モードが壊れていない」「1～2人でクラッシュせずゲームシーンへ入れる」を達成する。

### フェーズC: 追加モード実装（9月中旬～10月中旬）

- 企画決定後、勝利条件と必要な専用システムだけ追加。
- 毎週Release配布物を別PCで試遊。
- デザイナーがコード変更なしで調整できるJSON項目を必要分だけ増やす。
- 追加モードにBBが不要なら、無理に1人対応させずsystem disableで切る。

### フェーズD: 提出品質（10月中旬～11月7日）

- 機能追加を止め、クラッシュ、操作説明、導線、プレイ体感を優先。
- Releaseで長時間・連続再試合テスト。
- コントローラー抜差し、音声デバイスなし、異なる解像度、別Windows PCを確認。
- 15～90秒映像、サムネイル、ソース提出物、第三者ライセンス一覧を準備。
- 学校名・個人名がゲーム画面、映像、スクリーンショットへ入っていないか確認。
- 10月末までに提出可能版を作り、11月7日を初回完成日にしない。

## 8. チーム開発環境として最低限ほしいもの

### 必須

- `README.md`: 必要なVisual Studio workload、SDK、依存復元、ビルド、起動、配布手順。
- `docs/architecture.md`: App → Scene → Manager → Objectの責務と、イベント一覧。
- `docs/test-checklist.md`: 4人/1人/2人、全シーン遷移、2試合目、3ステージ、主要イベント。
- 1コマンドの `build` / `package` スクリプト。
- Release配布物を作るCI、または少なくとも毎日実行する共有手順。
- Pull Request単位を小さくし、`FieldManager.cpp` 等の担当範囲を決める。

### 有効だが後回し可

- clang-formatとEditorConfig。
- clang-tidy / Visual Studio Code Analysis。
- Debug UI（現在シーン、モード、人数、FPS、コライダー数、描画数、メモリ概算）。
- AddressSanitizer対応構成。少なくとも再試合リーク修正時に一度使う。
- アセットパスを検証する小さなツール。

## 9. 維持すべき良い点

- Scene変更を更新末尾へ予約する方式。
- 描画要求をRendererへ集約し、ゲームオブジェクトが直接CommandListを操作しない構造。
- Collision layer/mask、Broad/Narrow phase、Enter/Stay/Exitまで揃った衝突基盤。
- BulletManagerでunique_ptrを既に使っており、所有権改善の先例がある。
- Stage、Character、Controller、Game UIがManager単位に分かれている。
- `defaults.json` にゲーム調整値を出し始めている。
- オフライン4人モードは一度完成しており、新規制作ではなく回帰基準を作れる。

## 10. 最終判断

全面的なフレームワーク刷新は行わない。提出までに価値が高いのは次の順序。

1. 誰でもビルド・配布できる。
2. 起動失敗が診断できる。
3. 何度試合しても壊れない。
4. 4人固定前提をMatchConfig境界へ閉じ込める。
5. その上で1～2人モードを作る。
6. 最後にプレイ体感と提出物を磨く。

現在のコードは「捨てるべき試作品」ではなく、「完成経験はあるが再利用の境界と失敗処理が不足した専用実装」である。汎用化ではなく、上記境界を狭く整えることで締切内の開発基盤として十分使用可能になる。
