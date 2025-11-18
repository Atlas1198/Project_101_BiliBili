#include "Player.h"
#include <DirectXMath.h>
#include "App.h"

using namespace DirectX;

//初期化
void Player::Initialize(InputManager* pInputManager)
{
	m_pInputInfo = pInputManager->GetInputInfo();	//入力情報構造体の取得
}

//更新
void Player::UpdateOverride()
{
	if (App::GetInstance()->isOnline)
	{
		if (id == App::GetInstance()->descPlayer.uniqueID)
		{
			Move();		//移動
			//Rotate();	//回転
			//Scale();	//スケール

			App::GetInstance()->descPlayer.pos = { m_position.x, m_position.y, m_position.z };
			App::GetInstance()->players[id].pos = { m_position.x, m_position.y, m_position.z };
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
	else
	{
		Move();		//移動
	}
}

//衝突解決
void Player::ResolveCollisionsOverride()
{
}

//移動
void Player::Move()
{
	//回転から移動方向を決定する
	XMFLOAT3 direction{};
	direction.x = sinf(XMConvertToRadians(m_rotation.y));
	direction.y = 0.0f;
	direction.z = cosf(XMConvertToRadians(m_rotation.y));

	/*
		赤：WASD    +     F
		青：UHJK    +     G
		黄：↑←↓→     +     右Ctrl
		緑：テンキーの8456 + 右のPlusボタン
	*/

	

	bool up = m_pInputInfo->w.down;
	bool down = m_pInputInfo->s.down;
	bool left = m_pInputInfo->a.down;
	bool right = m_pInputInfo->d.down;

	if (!App::GetInstance()->isOnline)
	{
		switch (id)
		{
		case 0:
			break;
		case 1:
			up = m_pInputInfo->t.down;
			down = m_pInputInfo->g.down;
			left = m_pInputInfo->f.down;
			right = m_pInputInfo->h.down;
			break;
		case 2:
			up = m_pInputInfo->i.down;
			down = m_pInputInfo->k.down;
			left = m_pInputInfo->j.down;
			right = m_pInputInfo->l.down;
			break;
		case 3:
			up = m_pInputInfo->up.down;
			down = m_pInputInfo->down.down;
			left = m_pInputInfo->left.down;
			right = m_pInputInfo->right.down;
			break;
		default:
			break;
		}
	}

	if(up)
	{
		//前進
		m_position.x += direction.x * MOVE_SPEED;
		m_position.y += direction.y * MOVE_SPEED;
		m_position.z += direction.z * MOVE_SPEED;
	}
	if(down)
	{
		//後退
		m_position.x -= direction.x * MOVE_SPEED;
		m_position.y -= direction.y * MOVE_SPEED;
		m_position.z -= direction.z * MOVE_SPEED;
	}
	if(left)
	{
		//左移動
		m_position.x -= direction.z * MOVE_SPEED;
		m_position.z += direction.x * MOVE_SPEED;
	}
	if(right)
	{
		//右移動
		m_position.x += direction.z * MOVE_SPEED;
		m_position.z -= direction.x * MOVE_SPEED;
	}
}

//回転
void Player::Rotate()
{
	//if(m_pInputInfo->left.down)
	//{
	//	//左回転
	//	m_rotation.y -= ROTATE_SPEED;
	//}
	//if(m_pInputInfo->right.down)
	//{
	//	//右回転
	//	m_rotation.y += ROTATE_SPEED;
	//}
}

//スケール
void Player::Scale()
{
	//if (m_pInputInfo->up.down)
	//{
	//	//拡大
	//	//m_scale.x = (std::min)(m_scale.x + 0.008f, 5.0f);
	//	m_scale.y = (std::min)(m_scale.y + 0.008f, 5.0f);
	//	//m_scale.z = (std::min)(m_scale.z + 0.008f, 5.0f);
	//}
	//if (m_pInputInfo->down.down)
	//{
	//	//縮小
	//	//m_scale.x = (std::max)(m_scale.x - 0.008f, 0.005f);
	//	m_scale.y = (std::max)(m_scale.y - 0.008f, 0.005f);
	//	//m_scale.z = (std::max)(m_scale.z - 0.008f, 0.005f);
	//}
}
