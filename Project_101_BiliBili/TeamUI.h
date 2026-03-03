#pragma once
#include "HPBarUI.h"
#include "IconUI.h"
#include "SharedStruct.h"

//チーム1UIクラス
class TeamUI : public UIBase
{
public:
	TeamUI(
		DirectX::XMFLOAT3 position,
		DirectX::XMFLOAT3 scale,
		DirectX::XMFLOAT3 rotation,
		UINT order,
		const wchar_t* frameTexturePath,
		const wchar_t* gageTexturePath,
		const wchar_t* baseTexturePath,
		const wchar_t* alertTexturePath,
		const wchar_t* iconTexturePath,
		DIRECTION offsetDirection
	);
	~TeamUI();

	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;

	void UpdateOverride() override;
	void FinalizeOverride() override;
	void PrepareRenderInfoOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;

	void GetDamage(float health);	//ダメージ取得関数

	void SetTeamCharacter(int p1, int p2);
	void TurnOnBBUI();
	void SetIsBBActivated(bool isActivated) { m_pIconUI->SetIsBBActivated(isActivated); }

private:
	HPBarUI* m_pHPBarUI;	//HPバーUIポインタ
	IconUI* m_pIconUI;		//アイコンUIポインタ
	UIImage* m_pAlertImage;	//アラート画像UIポインタ

	int m_hitEffectTimer = 0;	//被弾エフェクトタイマー
	bool m_isHitEffect = false;	//被弾エフェクトフラグ
	const DirectX::XMFLOAT3 m_originalPosition; // 元の位置

private:
	void UpdateHitEffect();	//被弾エフェクト更新関数
	void ShakeUI();			//UIシェイク関数
};