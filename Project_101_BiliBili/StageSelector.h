#pragma once
#include <vector>
#include "SharedStruct.h"
#include "Context.h"

//前方宣言
struct InputInfo;

// ステージセレクタークラス
class StageSelector
{
public:
	void Initialize();
	void Update(SceneContext& context);
	void Finalize();

	bool IsSelected() { return m_isSelected; }

	STAGE_TYPE GetStage()
	{
		return static_cast<STAGE_TYPE>(m_cursor);
	}

	//シングルトンインスタンス取得
	static StageSelector& GetInstance()
	{
		static StageSelector instance;
		return instance;
	}

private:

	int m_timer = 0;
	int m_cursor = 0;
	int m_previousCursor = -1;
	bool m_isSelected = false;
};