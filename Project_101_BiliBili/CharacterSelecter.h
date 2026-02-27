#pragma once
#include "SharedStruct.h"
#include "Context.h"

//前方宣言
class InputManager;

//キャラクター選択状態構造体
struct SelecterState
{
	int characterIndex = 0;		//現在のキャラクターインデックス
	bool isSelected = false;	//選択済みフラグ
};

//キャラクター選択クラス
class CharacterSelecter
{
public:
	CharacterSelecter() = default;	//コンストラクタ
	~CharacterSelecter() = default;	//デストラクタ

	void Initialize();							//初期化
	void Update(SceneContext& sceneContext);	//更新
	void Finalize(SceneContext& sceneContext);	//終了

private:
	SelecterState m_states[4];	//キャラクター選択状態配列(最大4人分)

	bool m_isAllSelected = false;			//全キャラクター選択済みフラグ
	bool m_isCalledGoToNextScene = false;	//次のシーンへ進む処理が呼ばれたかどうか
	int m_countToNextScene = 0;				//次のシーンへ進むまでのカウント

	int m_backSceneKeyInputTimer = 0;
	int m_backSceneInputTimer[4] = { 0 };
};