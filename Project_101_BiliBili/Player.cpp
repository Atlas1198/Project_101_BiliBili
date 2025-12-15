#include "Player.h"
#include <DirectXMath.h>
#include "App.h"
#include "EventManager.h"
#include "EffectData.h"

using namespace DirectX;
using namespace CollisionData;

Player::Player(MESH_TYPE meshType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 velocity, uint32_t id, bool isActive, ColliderType colliderType, DirectX::XMFLOAT3 collisionBoxSize, bool collisionIsTrigger)
	: ObjectBase(
		meshType,
		position,
		rotation,
		scale,
		velocity,
		isActive,
		OBJECT_TAG::PLAYER,
		CollisionData::COLLISION_LAYER::PLAYER
	),
	id(id)

{
	m_pColliderSet->AddCollider(
		ColliderType::BOX,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
}

//初期化
void Player::Initialize(InputManager* pInputManager, BulletManager* pBulletManager)
{
	m_pInputInfo = pInputManager->GetInputInfo();	//入力情報構造体の取得
	m_pBulletManager = pBulletManager;
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

		/*
		Vec3 spawnPoses[4] = {
		{-13.0f, -4.0f, 18.0f},
		{13.0f, -4.0f, 18.0f},
		{-13.0f, -4.0f, -8.0f},
		{13.0f, -4.0f, -8.0f}
	};
		*/

		if (m_position.x < -13.5f) m_position.x = 13.1f;
		if (m_position.x > 13.5f) m_position.x = -13.1f;
		if (m_position.z < -8.5f) m_position.z = 18.1f;
		if (m_position.z > 18.5f) m_position.z = -8.1f;

		Shoot();
		//Rotate();
		//Scale();
	}
}
 
//衝突解決
void Player::ResolveCollisionsOverride()
{
	XMFLOAT3 pushVector{};	//押し出しベクトル
	auto& infos = m_pColliderSet->GetCollisionInfos();

	pushVector = GetPushOutVector(
		infos,	//衝突情報配列
		{//対象タグリスト(レイヤーマスクにも含まれている必要がある)
			OBJECT_TAG::PLAYER,		//他のプレイヤーも押し出す
			OBJECT_TAG::WALL,		//壁
			OBJECT_TAG::WALLPASS,	//弾貫通壁
			OBJECT_TAG::GROUND		//地面
		}
	);

	//最大押し出しベクトル分だけ移動
	m_position.x += pushVector.x;
	//m_position.y += pushVector.y;
	m_position.z += pushVector.z;

	for (auto& info : infos)
	{
		if (info.opponent->GetTag() == OBJECT_TAG::GROUND)
		{
			//地面に接触している場合はY座標を補正
			m_velocity.y = 0.0f;
		}
	}
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

	

	bool up = m_pInputInfo->key.w.down;
	bool down = m_pInputInfo->key.s.down;
	bool left = m_pInputInfo->key.a.down;
	bool right = m_pInputInfo->key.d.down;

	XMFLOAT2 dir = { 0.0f, 0.0f };

	if (!App::GetInstance()->isOnline)
	{
		switch (id)
		{
		case 0:
			dir = m_pInputInfo->controller[0].leftStick; 
			up = m_pInputInfo->key.w.down;
			down = m_pInputInfo->key.s.down;
			left = m_pInputInfo->key.a.down;
			right = m_pInputInfo->key.d.down;
			break;
		case 1:
			dir = m_pInputInfo->controller[1].leftStick;
			up = m_pInputInfo->key.t.down;
			down = m_pInputInfo->key.g.down;
			left = m_pInputInfo->key.f.down;
			right = m_pInputInfo->key.h.down;
			break;
		case 2:
			dir = m_pInputInfo->controller[2].leftStick;
			up = m_pInputInfo->key.i.down;
			down = m_pInputInfo->key.k.down;
			left = m_pInputInfo->key.j.down;
			right = m_pInputInfo->key.l.down;
			break;
		case 3:
			dir = m_pInputInfo->controller[3].leftStick;
			up = m_pInputInfo->key.up.down;
			down = m_pInputInfo->key.down.down;
			left = m_pInputInfo->key.left.down;
			right = m_pInputInfo->key.right.down;
			break;
		default:
			break;
		}
	}

	m_position.x += dir.x * MOVE_SPEED;
	m_position.z += dir.y * MOVE_SPEED;

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

void Player::Shoot()
{
	if (!m_pBulletManager) return;

	bool shoot = false;

	if (!App::GetInstance()->isOnline)
	{
		switch (id)
		{
		case 0:
			shoot = m_pInputInfo->key.z.trigger|| m_pInputInfo->controller[0].B.trigger;
			break;
		case 1:
			shoot = m_pInputInfo->key.c.trigger || m_pInputInfo->controller[1].B.trigger;
			break;
		case 2:
			shoot = m_pInputInfo->key.n.trigger || m_pInputInfo->controller[2].B.trigger;
			break;
		case 3:
			shoot = m_pInputInfo->key.rightCtrl.trigger || m_pInputInfo->controller[3].B.trigger;
			break;
		default:
			break;
		}
	}

	if (shoot && teammate)
	{
		DirectX::XMFLOAT3 dir{ 0.0f, 0.0f, 1.0f };

		DirectX::XMFLOAT3 matePos = teammate->GetPosition();
		DirectX::XMVECTOR vThis = DirectX::XMLoadFloat3(&m_position);
		DirectX::XMVECTOR vMate = DirectX::XMLoadFloat3(&matePos);

		DirectX::XMVECTOR vDir = DirectX::XMVectorSubtract(vMate, vThis);

		DirectX::XMVECTOR vLenVec = DirectX::XMVector3Length(vDir);
		float len = DirectX::XMVectorGetX(vLenVec);
		const float EPS = 1e-6f;
		if (len > EPS)
		{
			vDir = DirectX::XMVectorScale(vDir, 1.0f / len);
			DirectX::XMStoreFloat3(&dir, vDir);
		}
		else
		{
			// 味方と同じ位置なら前方に撃つ
			dir = DirectX::XMFLOAT3{ 0.0f, 0.0f, 1.0f };
		}

		m_pBulletManager->FireBullet(
			m_position,
			dir,
			BULLET_SPEED,
			teamID,
			id
		);
	}
}

//回転
void Player::Rotate()
{
	if(m_pInputInfo->key.left.down)
	{
		//左回転
		m_rotation.y -= ROTATE_SPEED;
	}
	if(m_pInputInfo->key.right.down)
	{
		//右回転
		m_rotation.y += ROTATE_SPEED;
	}
}

//スケール
void Player::Scale()
{
	if (m_pInputInfo->key.up.down)
	{
		//拡大
		//m_scale.x = (std::min)(m_scale.x + 0.008f, 5.0f);
		m_scale.x = (std::min)(m_scale.x + 0.008f, 5.0f);
		//m_scale.z = (std::min)(m_scale.z + 0.008f, 5.0f);
	}
	if (m_pInputInfo->key.down.down)
	{
		//縮小
		//m_scale.x = (std::max)(m_scale.x - 0.008f, 0.005f);
		m_scale.x = (std::max)(m_scale.x - 0.008f, 0.005f);
		//m_scale.z = (std::max)(m_scale.z - 0.008f, 0.005f);
	}
}
