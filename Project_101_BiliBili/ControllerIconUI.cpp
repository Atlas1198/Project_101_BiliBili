#include "ControllerIconUI.h"
#include "InputInfo.h"

using namespace DirectX;

// コンストラクタ
ControllerIconUI::ControllerIconUI(
	DirectX::XMFLOAT3 position, 
	DirectX::XMFLOAT3 scale, 
	DirectX::XMFLOAT3 rotation, 
	UINT order, 
	const std::wstring& textureFilePath,
	PSOKey psoKey
)
	: UIBase(position, scale, rotation, order, psoKey), 
	m_textureFilePath(textureFilePath)
{
}

// 初期化
void ControllerIconUI::InitializeOverride(
	TextureManager& textureManager,
	MeshManager& meshManager
)
{
	const float SCALE_FACTOR = 1.4f;

	m_pIconImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 415.0f * SCALE_FACTOR, 414.0f * SCALE_FACTOR, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order,
		m_textureFilePath
	);
}

// 更新
void ControllerIconUI::UpdateOverride()
{
	if (!m_isActivated)
	{
		auto transform =m_pIconImage->GetLocalTransform();
		auto scale = transform.scale;
		scale.x *= 0.95f;
		scale.y *= 0.95f;

		XMFLOAT3 setScale =
		{
			scale.x,
			scale.y,
			1.0f
		};

		if( scale.x <= 415.0f)
		{
			m_isActivated = true;
			setScale = { 415.0f, 414.0f, 1.0f };
		}

		m_pIconImage->SetLocalTransform(
			{
				transform.position,
				setScale,
				transform.rotation
			}
		);
	}

	if(m_isReacting)
	{
		m_reactionFrameCount++;

		const float changeAmount = 1.05f;
		if (m_reactionFrameCount <= 3)
		{//拡大
			Transform3D transform = m_pIconImage->GetLocalTransform();
			transform.scale.x *= changeAmount;
			transform.scale.y *= changeAmount;
			m_pIconImage->SetLocalTransform(transform);
		}
		else if (m_reactionFrameCount <= 6)
		{//縮小
			Transform3D transform = m_pIconImage->GetLocalTransform();
			transform.scale.x /= changeAmount;
			transform.scale.y /= changeAmount;
			m_pIconImage->SetLocalTransform(transform);
		}


		if(m_reactionFrameCount >= 10)
		{
			m_isReacting = false;
			m_reactionFrameCount = 0;
		}
	}
}

// 終了
void ControllerIconUI::FinalizeOverride()
{
	
}

// アクティブリアクション
void ControllerIconUI::ActivateReaction(const InputInfo& info)
{
	if (!m_isActivated || m_isReacting) return;

	m_isReacting = true;
	m_reactionFrameCount = 0;
}