#include "CutInUI.h"

using namespace DirectX;

//コンストラクタ
CutInUI::CutInUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order, DIRECTION direction)
	: UIBase(position, scale, rotation, order), m_direction(direction)
{
	//スケールのX成分を方向に応じて反転
	float directionFactor = 0.0f;
	switch (direction)
	{
	case DIRECTION::LEFT:
		directionFactor = 1.0f;
		break;
	case DIRECTION::RIGHT:
		directionFactor = -1.0f;
		break;
	default:
		break;
	}

	XMFLOAT3 imageScale = XMFLOAT3{ 490.0f * directionFactor, 224.0f, 1.0f };
	XMFLOAT3 imagePosition =
	{
		directionFactor* (1980.0f * 0.5f) + directionFactor * (fabs(imageScale.x) * 0.5f),
		-200.0f,
		0.0f
	};

	m_pCutInImage[0] = AddChild<UIImage>(
		imagePosition,							//位置
		imageScale,								//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
		0,										//描画順序
		L"asset/texture/game_scene/UI_INGAME_L_cut-in.png"
	);
	m_pCutInImage[0]->SetColor(XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f });	//赤色に設定（テスト用）

	m_pCutInImage[1] = AddChild<UIImage>(
		imagePosition,							//位置
		imageScale,								//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
		0,										//描画順序
		L"asset/texture/game_scene/UI_INGAME_L_cut-in.png"
	);
	m_pCutInImage[1]->SetColor(XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f });	//青色に設定（テスト用）

	m_pCutInImage[2] = AddChild<UIImage>(
		imagePosition,							//位置
		imageScale,								//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
		0,										//描画順序
		L"asset/texture/game_scene/UI_INGAME_L_cut-in.png"
	);
	m_pCutInImage[2]->SetColor(XMFLOAT4{ 1.0f, 1.0f, 0.0f, 1.0f });	//黄色に設定（テスト用）

	m_pCutInImage[3] = AddChild<UIImage>(
		position,								//位置
		imageScale,								//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
		0,										//描画順序
		L"asset/texture/game_scene/UI_INGAME_L_cut-in.png"
	);
	m_pCutInImage[3]->SetColor(XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f });	//緑色に設定（テスト用）

	for(auto& pos : m_initialPosition)
	{
		pos = imagePosition;	//初期位置保存
	}
}

//デストラクタ
CutInUI::~CutInUI()
{
}

//初期化
void CutInUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}

//更新
void CutInUI::UpdateOverride()
{
	m_mainTimer++;	//メインタイマー更新

	for(int i = 0; i < 4; ++i)
	{
		switch (m_state[i])
		{
		case STATE::NONE:
			//何もしない
			m_pCutInImage[i]->SetLocalTransform(
				Transform3D{
					m_initialPosition[i],
					m_pCutInImage[i]->GetLocalTransform().scale,
					m_pCutInImage[i]->GetLocalTransform().rotation
				}
			);
			break;
		case STATE::STATE_IN:
			//カットイン導入処理
			Start(i);
			break;
		case STATE::STATE_STAY:
			//カットイン滞在処理
			Stay(i);
			break;
		case STATE::STATE_OUT:
			//カットイン終了処理
			End(i);
			break;
			}
	}
}

//終了
void CutInUI::FinalizeOverride()
{
}

//カットイン呼び出し関数
void CutInUI::CallCutIn(int characterIndex)
{
	if (characterIndex < 0 || characterIndex >= 4) return;

	m_indexStartTime[characterIndex] = m_mainTimer;	//インデックスタイマー初期化
	m_state[characterIndex] = STATE::STATE_IN;					//状態を導入に設定
}

//オブジェクトの描画情報生成
void CutInUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	for(auto& img : m_pCutInImage)
	{
		img->PrepareRenderInfo(textureManager, meshManager);
	}
}

//カットイン導入処理
void CutInUI::Start(int characterIndex)
{
	const int inDuration = 10;	//導入時間(フレーム数)

	//開始位置と目的地位置の計算
	const float startX = m_direction == DIRECTION::LEFT ? -(1980.0f * 0.5f) - (fabs(m_pCutInImage[characterIndex]->GetLocalTransform().scale.x) * 0.5f) :
		(1980.0f * 0.5f) + (fabs(m_pCutInImage[characterIndex]->GetLocalTransform().scale.x) * 0.5f);
	const float destinationX = m_direction == DIRECTION::LEFT ? -(1980.0f * 0.5f) + (fabs(m_pCutInImage[characterIndex]->GetLocalTransform().scale.x) * 0.5f) :
		(1980.0f * 0.5f) - (fabs(m_pCutInImage[characterIndex]->GetLocalTransform().scale.x) * 0.5f);

	XMFLOAT3A startPos =
	{
		startX,
		m_pCutInImage[characterIndex]->GetLocalTransform().position.y,
		m_pCutInImage[characterIndex]->GetLocalTransform().position.z
	};

	XMFLOAT3A destPos =
	{
		destinationX,
		m_pCutInImage[characterIndex]->GetLocalTransform().position.y,
		m_pCutInImage[characterIndex]->GetLocalTransform().position.z
	};

	int progress = m_mainTimer - m_indexStartTime[characterIndex];	//進行度(フレーム数)

	XMFLOAT3 newPos = { 0.0f, 0.0f, 0.0f };
	newPos = LerpXMF3(startPos, destPos, progress / static_cast<float>(inDuration));	//線形補間で新しい位置を計算

	//位置更新
	m_pCutInImage[characterIndex]->SetLocalTransform(
		Transform3D{
			newPos,
			m_pCutInImage[characterIndex]->GetLocalTransform().scale,
			m_pCutInImage[characterIndex]->GetLocalTransform().rotation
		}
	);

	//導入完了判定
	if (progress >= inDuration)
	{
		//状態を滞在に変更
		m_state[characterIndex] = STATE::STATE_STAY;
		m_indexStartTime[characterIndex] = m_mainTimer;	//インデックスタイマーリセット
	}
}

//カットイン滞在処理
void CutInUI::Stay(int characterIndex)
{
	const int stayDuration = 90;	//滞在時間(フレーム数)

	int progress = m_mainTimer - m_indexStartTime[characterIndex];	//進行度(フレーム数)

	//左右振動エフェクト
	float amplitude = 1.5f;	//振幅
	float frequency = 0.025f;	//周波数
	float offsetX = amplitude * sinf(frequency * progress * 2.0f * 3.14159f);	//X方向のオフセット計算
	XMFLOAT3 currentPos = m_pCutInImage[characterIndex]->GetLocalTransform().position;
	currentPos.x += offsetX;												//X位置にオフセットを加算
	m_pCutInImage[characterIndex]->SetLocalTransform(
		Transform3D{
			currentPos,
			m_pCutInImage[characterIndex]->GetLocalTransform().scale,
			m_pCutInImage[characterIndex]->GetLocalTransform().rotation
		}
	);

	if (progress >= stayDuration)
	{
		//状態を終了に変更
		m_state[characterIndex] = STATE::STATE_OUT;
		m_indexStartTime[characterIndex] = m_mainTimer;	//インデックスタイマーリセット
	}
}

//カットイン終了処理
void CutInUI::End(int characterIndex)
{
	const int outDuration = 2;	//終了時間(フレーム数)

	//開始位置と目的地位置の計算
	const float startX = m_direction == DIRECTION::LEFT ? -(1980.0f * 0.5f) + (fabs(m_pCutInImage[characterIndex]->GetLocalTransform().scale.x) * 0.5f) :
		(1980.0f * 0.5f) - (fabs(m_pCutInImage[characterIndex]->GetLocalTransform().scale.x) * 0.5f);

	const float destinationX = m_direction == DIRECTION::LEFT ? -(1980.0f * 0.5f) - (fabs(m_pCutInImage[characterIndex]->GetLocalTransform().scale.x) * 0.5f) :
		(1980.0f * 0.5f) + (fabs(m_pCutInImage[characterIndex]->GetLocalTransform().scale.x) * 0.5f);

	XMFLOAT3A startPos =
	{
		startX,
		m_pCutInImage[characterIndex]->GetLocalTransform().position.y,
		m_pCutInImage[characterIndex]->GetLocalTransform().position.z
	};

	XMFLOAT3A destPos =
	{
		destinationX,
		m_pCutInImage[characterIndex]->GetLocalTransform().position.y,
		m_pCutInImage[characterIndex]->GetLocalTransform().position.z
	};

	int progress = m_mainTimer - m_indexStartTime[characterIndex];	//進行度(フレーム数)
	XMFLOAT3 newPos = { 0.0f, 0.0f, 0.0f };
	newPos = LerpXMF3(startPos, destPos, progress / static_cast<float>(outDuration));	//線形補間で新しい位置を計算

	//位置更新
	m_pCutInImage[characterIndex]->SetLocalTransform(
		Transform3D{
			newPos,
			m_pCutInImage[characterIndex]->GetLocalTransform().scale,
			m_pCutInImage[characterIndex]->GetLocalTransform().rotation
		}
	);

	//終了完了判定
	if (progress >= outDuration)
	{
		m_state[characterIndex] = STATE::NONE;	//状態をなしに設定
	}
}