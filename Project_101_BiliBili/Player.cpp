#include "Player.h"
#include <DirectXMath.h>
#include "App.h"

using namespace DirectX;

//èâä˙âª
void Player::Initialize(InputManager* pInputManager)
{
	m_pInputInfo = pInputManager->GetInputInfo();	//ì¸óÕèÓïÒç\ë¢ëÃÇÃéÊìæ
}

//çXêV
void Player::UpdateOverride()
{
	if (id == App::GetInstance()->descPlayer.uniqueID)
	{
		Move();		//à⁄ìÆ
		//Rotate();	//âÒì]
		//Scale();	//ÉXÉPÅ[Éã

		App::GetInstance()->descPlayer.pos = {m_position.x, m_position.y, m_position.z};
		App::GetInstance()->players[id].pos = {m_position.x, m_position.y, m_position.z};
	}
	else
	{
		for (const auto &desc : App::GetInstance()->players)
		{
			if (desc.first == id)
			{
				m_position.x = desc.second.pos.x;
				m_position.y = desc.second.pos.y;
				m_position.z = desc.second.pos.z;
			}
		}
	}
	
}

//è’ìÀâåà
void Player::ResolveCollisionsOverride()
{
}

//à⁄ìÆ
void Player::Move()
{
	//âÒì]Ç©ÇÁà⁄ìÆï˚å¸ÇåàíËÇ∑ÇÈ
	XMFLOAT3 direction{};
	direction.x = sinf(XMConvertToRadians(m_rotation.y));
	direction.y = 0.0f;
	direction.z = cosf(XMConvertToRadians(m_rotation.y));

	if(m_pInputInfo->w.down)
	{
		//ëOêi
		m_position.x += direction.x * MOVE_SPEED;
		m_position.y += direction.y * MOVE_SPEED;
		m_position.z += direction.z * MOVE_SPEED;
	}
	if(m_pInputInfo->s.down)
	{
		//å„ëﬁ
		m_position.x -= direction.x * MOVE_SPEED;
		m_position.y -= direction.y * MOVE_SPEED;
		m_position.z -= direction.z * MOVE_SPEED;
	}
	if(m_pInputInfo->a.down)
	{
		//ç∂à⁄ìÆ
		m_position.x -= direction.z * MOVE_SPEED;
		m_position.z += direction.x * MOVE_SPEED;
	}
	if(m_pInputInfo->d.down)
	{
		//âEà⁄ìÆ
		m_position.x += direction.z * MOVE_SPEED;
		m_position.z -= direction.x * MOVE_SPEED;
	}
}

//âÒì]
void Player::Rotate()
{
	if(m_pInputInfo->left.down)
	{
		//ç∂âÒì]
		m_rotation.y -= ROTATE_SPEED;
	}
	if(m_pInputInfo->right.down)
	{
		//âEâÒì]
		m_rotation.y += ROTATE_SPEED;
	}
}

//ÉXÉPÅ[Éã
void Player::Scale()
{
	if (m_pInputInfo->up.down)
	{
		//ägëÂ
		m_scale.x = (std::min)(m_scale.x + 0.008f, 5.0f);
		m_scale.y = (std::min)(m_scale.y + 0.008f, 5.0f);
		m_scale.z = (std::min)(m_scale.z + 0.008f, 5.0f);
	}
	if (m_pInputInfo->down.down)
	{
		//èkè¨
		m_scale.x = (std::max)(m_scale.x - 0.008f, 0.005f);
		m_scale.y = (std::max)(m_scale.y - 0.008f, 0.005f);
		m_scale.z = (std::max)(m_scale.z - 0.008f, 0.005f);
	}
}
