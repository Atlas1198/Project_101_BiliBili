#include "Player.h"
#include "Spring.h"
#include "App.h"
#include "EventManager.h"
#include "EffectData.h"
#include "SharedStruct.h"
#include <DirectXMath.h>
#include <algorithm> // clamp
#include <cmath>


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
		CollisionData::COLLISION_LAYER::PLAYER,
		collisionBoxSize
	),
	id(id)

{
	const XMFLOAT3 COLLIDER_SCALE =
	{
		m_scale.x * 0.7f,
		m_scale.y * 0.7f,
		m_scale.z * 0.7f
	};

	m_pColliderSet->AddCollider(
		ColliderType::SPHERE,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		COLLIDER_SCALE,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
	);

	TexSplitInfo texInfo{};
	texInfo.cols = 3;
	texInfo.rows = 8;
	texInfo.total = texInfo.cols * texInfo.rows;
	texInfo.index = 0;
	texInfo.frameCount = 0;
	texInfo.updateRate = 0;

	m_texSplitInfo = texInfo;

	minAnimIndex = 0;
	maxAnimIndex = 1;
}

//初期化
void Player::Initialize(InputInfo* inputInfo, BulletManager* pBulletManager)
{
	m_pInputInfo = inputInfo;	//入力情報構造体の取得
	m_pBulletManager = pBulletManager;
	gameTimer.Mark();
	Reset();

	EventManager::GetInstance()->Subscribe<uint32_t>(
		EventType::DAMAGE_ANIMATION,
		[this](std::shared_ptr<uint32_t> data)
		{
			StartDamageAnimation();
		}
	);
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
			for (const auto& desc : App::GetInstance()->players)
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
		if (m_ignoreCollisionFrame > 0)
		{
			--m_ignoreCollisionFrame;
		}

		if (!bbSlowMoveSpeed && (gameTimer.Peek() > 120.0f))
		{
			bbSlowMoveSpeed = true;
		}

		if (bbSlowMoveSpeed && (gameTimer.Peek() > 180.0f))
		{
			bbSlowMoveSpeed = false;
		}

		if (runTimerStarted)
		{
			if (runTimer.Peek() >= RUN_DELAY)
			{
				canRun = true;
				runTimerStarted = false;
			}
		}

		if (damageAnimation)
		{
			if (damageAnimTimer.Peek() >= 2.0f)
			{
				damageAnimation = false;
				m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
			}

			float colors = std::sin(damageAnimTimer.Peek() * 10.0f) * 0.5f + 0.5f;

			m_color = { 1.0f, colors, colors, 1.0f };
		}

		Move();		//移動

		/*
		Vec3 spawnPoses[4] = {
		{-13.0f, -4.0f, 18.0f},
		{13.0f, -4.0f, 18.0f},
		{-13.0f, -4.0f, -8.0f},
		{13.0f, -4.0f, -8.0f}
	};
		*/

		/*if (m_position.x < -22.5f) m_position.x = 22.1f;
		if (m_position.x > 22.5f) m_position.x = -22.1f;
		if (m_position.z < -8.5f) m_position.z = 18.1f;
		if (m_position.z > 18.5f) m_position.z = -8.1f;*/

		Shoot();
		//Rotate();
		//Scale();
	}
}

void Player::ResolveCollisionsOverride()
{
	if (m_ignoreCollisionFrame > 0)
	{
		return;
	}

	XMFLOAT3 pushVector{};	//押し出しベクトル
	auto &infos = m_pColliderSet->GetCollisionInfos();

	pushVector = GetPushOutVector(
		infos,	//衝突情報配列
		{//対象タグリスト(レイヤーマスクにも含まれている必要がある)
			OBJECT_TAG::PLAYER,		//他のプレイヤーも押し出す
			OBJECT_TAG::WALL,		//壁
			OBJECT_TAG::WALLPASS,	//弾貫通壁
			OBJECT_TAG::WALLCURVE,	//カーブ壁
			OBJECT_TAG::GROUND		//地面
		}
	);

	//最大押し出しベクトル分だけ移動
	m_position.x += pushVector.x;
	//m_position.y += pushVector.y;
	m_position.z += pushVector.z;

	m_isGrounded = false;

	for (auto &info : infos)
	{
		if (info.opponent->GetTag() == OBJECT_TAG::GROUND)
		{
			m_isGrounded = true;

			// 落下は止める
			m_velocity.y = 0.0f;

			// ★ここ：着地時の滑り防止（バネジャンプ後なら水平速度も止める）
			if (m_isSpringJump)
			{
				m_velocity.x = 0.0f;
				m_velocity.z = 0.0f;
			}

			m_isSpringJump = false;
		}
	}

	for (auto &info : infos)
	{
		if (!info.opponent)      // ★NULLチェック
			continue;

		if (info.opponent->GetTag() == OBJECT_TAG::SPRING)
		{
			if (!m_isSpringJump)
			{
				Spring *spring = dynamic_cast<Spring *>(info.opponent);
				if (!spring) continue;
				if (spring)
				{
					const XMFLOAT3 startPos = m_position;                 // ★ここ重要
					const XMFLOAT3 targetPos = spring->ChooseLaunchTarget();

					const float dx = targetPos.x - startPos.x;
					const float dy = targetPos.y - startPos.y;
					const float dz = targetPos.z - startPos.z;

					// 距離に応じて飛行時間(フレーム)を決める
					// 目安：水平速度 0.9f くらいで飛ばす
					const float distXZ = std::sqrt(dx * dx + dz * dz);

					const float desiredSpeedXZ = 0.1f; // 好みで調整（大きいほど速く短時間）
					float T = (desiredSpeedXZ > 0.0001f) ? (distXZ / desiredSpeedXZ) : 30.0f;

					// 早すぎ/遅すぎ防止（15～60フレームに制限）
					T = std::clamp(T, 15.0f, 60.0f);

					// あなたの重力適用：毎フレーム vy -= GRAVITY
					const float g = GRAVITY;

					// 目標：Tフレーム後に target に到達
					// x,z は等速： vx = dx/T, vz = dz/T
					// y は等加速度： dy = vy0*T - 0.5*g*T^2  -> vy0 = (dy + 0.5*g*T^2)/T
					const float vx = dx / T;
					const float vz = dz / T;
					const float vy = (dy + 0.5f * g * T * T) / T;

					m_velocity.x = vx;
					m_velocity.z = vz;
					m_velocity.y = vy;

					m_isGrounded = false;
					m_isSpringJump = true;

					// 連続ヒット防止（必要なら）
					// m_ignoreCollisionFrame = 5;

					spring->SetIsBlowing(true);
					m_pSceneContext->pInputInfo->controller[controllerID].SetVibration(1.0f, 1.0f, 10);
				}
			}
			AudioManager::GetInstance()->PlaySE("BANE_JUMP");

		}
	}

}

void Player::SetBB(bool isActive)
{
	if (isActive)
	{
		bbActive = true;
		m_texSplitInfo.cols = 2;
		m_texSplitInfo.frameCount = 0;
	}
	else
	{
		bbActive = false;
		m_texSplitInfo.cols = 3;
		m_texSplitInfo.frameCount = 0;
	}
}

//コントローラーを振動させる
void Player::ShakeController(float leftMotor, float rightMotor, int duration)
{
	auto myController = m_pInputInfo->controller[id];
	myController.SetVibration(leftMotor, rightMotor, duration);
}

void Player::StartDamageAnimation()
{
	damageAnimation = true;
	damageAnimTimer.Mark();
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
		auto myController = m_pInputInfo->controller[id];

		switch (id)
		{
		case 0:
			up = m_pInputInfo->key.w.down;
			down = m_pInputInfo->key.s.down;
			left = m_pInputInfo->key.a.down;
			right = m_pInputInfo->key.d.down;
			break;
		case 1:
			up = m_pInputInfo->key.t.down;
			down = m_pInputInfo->key.g.down;
			left = m_pInputInfo->key.f.down;
			right = m_pInputInfo->key.h.down;
			break;
		case 2:
			up = m_pInputInfo->key.i.down;
			down = m_pInputInfo->key.k.down;
			left = m_pInputInfo->key.j.down;
			right = m_pInputInfo->key.l.down;
			break;
		case 3:
			up = m_pInputInfo->key.up.down;
			down = m_pInputInfo->key.down.down;
			left = m_pInputInfo->key.left.down;
			right = m_pInputInfo->key.right.down;
			break;
		default:
			break;
		}

		//コントローラー入力
		//十字キー入力
		XMFLOAT2 crossInput{ 0.0f, 0.0f };
		if (myController.LEFT.down) crossInput.x = (std::max)(-1.0f, crossInput.x - 1.0f);
		if (myController.RIGHT.down) crossInput.x = (std::min)(1.0f, crossInput.x + 1.0f);
		if (myController.UP.down) crossInput.y = (std::min)(1.0f, crossInput.y + 1.0f);
		if (myController.DOWN.down) crossInput.y = (std::max)(-1.0f, crossInput.y - 1.0f);
		crossInput.x = Normalize({crossInput.x, crossInput.y, 0.0f}).x;
		crossInput.y = Normalize({ crossInput.x, crossInput.y, 0.0f }).y;
		//左スティック入力
		XMFLOAT2 leftStickInput = { 0.0f, 0.0f };
		const float DEAD_ZONE = 0.2f;
		if (fabs(myController.leftStick.x) > DEAD_ZONE) leftStickInput.x = myController.leftStick.x;
		if (fabs(myController.leftStick.y) > DEAD_ZONE) leftStickInput.y = myController.leftStick.y;
		leftStickInput.x = Normalize({ leftStickInput.x, leftStickInput.y, 0.0f }).x;
		leftStickInput.y = Normalize({ leftStickInput.x, leftStickInput.y, 0.0f }).y;
		//合成(-1.0f～1.0fにクランプ)
		dir.x = std::clamp(crossInput.x + leftStickInput.x, -1.0f, 1.0f);
		dir.y = std::clamp(crossInput.y + leftStickInput.y, -1.0f, 1.0f);
	}

	float modifier = 1.0f;

	if (!canRun)
	{
		if (!runTimerStarted)
		{
			modifier = RUN_MODIFIER;
			runTimerStarted = true;
			runTimer.Mark();
		}
		else
		{
			modifier = RUN_MODIFIER + (runTimer.Peek() / RUN_DELAY) * (1.0f - RUN_MODIFIER);
		}
	}

	if (bbSlowMoveSpeed && bbActive)
	{
		modifier *= BB_SLOW_MOVE_MODIFIER;
	}

	if (!m_isSpringJump)
	{
		m_position.x += dir.x * MOVE_SPEED * modifier;
		m_position.z += dir.y * MOVE_SPEED * modifier;

		if (up)
		{
			//前進
			m_position.x += direction.x * MOVE_SPEED * modifier;
			m_position.y += direction.y * MOVE_SPEED * modifier;
			m_position.z += direction.z * MOVE_SPEED * modifier;
		}
		if (down)
		{
			//後退
			m_position.x -= direction.x * MOVE_SPEED * modifier;
			m_position.y -= direction.y * MOVE_SPEED * modifier;
			m_position.z -= direction.z * MOVE_SPEED * modifier;
		}
		if (left)
		{
			//左移動
			m_position.x -= direction.z * MOVE_SPEED * modifier;
			m_position.z += direction.x * MOVE_SPEED * modifier;
		}
		if (right)
		{
			//右移動
			m_position.x += direction.z * MOVE_SPEED * modifier;
			m_position.z -= direction.x * MOVE_SPEED * modifier;
		}


		if (dir.x != 0.0f || dir.y != 0.0f)
		{
			down = dir.y < -0.5f;
			up = dir.y > 0.5f;
			left = dir.x < -0.5f;
			right = dir.x > 0.5f;
		}

		/*
		if (down && left) this->direction = 1;
		else if (down && right) this->direction = 7;
		else if (up && right) this->direction = 5;
		else if (up && left) this->direction = 3;
		else if (up) this->direction = 4;
		else if (down) this->direction = 0;
		else if (left) this->direction = 2;
		else if (right) this->direction = 6;
		*/

		isMoving = true;

		if (!bbActive)
		{
			if (down && left) this->direction = 1;
			else if (down && right) this->direction = 7;
			else if (up && right) this->direction = 5;
			else if (up && left) this->direction = 3;
			else if (up) this->direction = 4;
			else if (down) this->direction = 0;
			else if (left) this->direction = 2;
			else if (right) this->direction = 6;
			else isMoving = false;
		}
		else
		{
			if (down && left)
				this->direction = 1;
			else if (down && right)
				this->direction = 7;
			else if (up && right)
				this->direction = 5;
			else if (up && left)
				this->direction = 3;
			else if (up)
				this->direction = 4;
			else if (down)
				this->direction = 0;
			else if (left)
				this->direction = 2;
			else if (right)
				this->direction = 6;
			else
				isMoving = false;
		}

		if (!isMoving)
		{
			runTimerStarted = false;
			canRun = false;
			runTimer.Mark();
		}
	}

	m_position.x += m_velocity.x;
	m_position.y += m_velocity.y;
	m_position.z += m_velocity.z;

	if (!m_isSpringJump)
	{
		m_velocity.x *= 0.95f;
		m_velocity.y *= 0.95f;
		m_velocity.z *= 0.95f;
	}


	UpdateAnimation();

	if (!m_isGrounded)
	{
		m_velocity.y -= GRAVITY;
	}
}

void Player::UpdateAnimation()
{
	if (!bbActive && isShooting)
	{
		m_texSplitInfo.frameCount++;
		m_texSplitInfo.index = direction * 3 + 2;

		if (m_texSplitInfo.frameCount >= shootAnimDuration)
		{
			isShooting = false;
			m_texSplitInfo.frameCount = 0;
		}
	}
	else
	{
		minAnimIndex = bbActive ? direction * 2 : direction * 3;
		maxAnimIndex = minAnimIndex + 1;

		if (isMoving)
			m_texSplitInfo.frameCount++;

		if (m_texSplitInfo.index < minAnimIndex || m_texSplitInfo.index > maxAnimIndex)
		{
			m_texSplitInfo.index = minAnimIndex;
			m_texSplitInfo.frameCount = 0;
		}

		if (m_texSplitInfo.frameCount >= animUpdateRate)
		{
			m_texSplitInfo.index++;

			if (m_texSplitInfo.index > maxAnimIndex)
			{
				m_texSplitInfo.index = minAnimIndex;
			}

			m_texSplitInfo.frameCount = 0;
		}
	}
}

void Player::Shoot()
{
	if (!m_pBulletManager) return;
	if (bbActive) return;

	bool shoot = false;

	if (!App::GetInstance()->isOnline)
	{
		switch (id)
		{
		case 0:
			shoot = m_pInputInfo->key.z.trigger || m_pInputInfo->controller[0].B.trigger;
			
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

		isShooting = true;
		m_texSplitInfo.frameCount = 0;
		UpdateAnimation();
	
	}
}

//回転
void Player::Rotate()
{
	if (m_pInputInfo->key.left.down)
	{
		//左回転
		m_rotation.y -= ROTATE_SPEED;
	}
	if (m_pInputInfo->key.right.down)
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
		m_scale.x = (std::min)(m_scale.x + 0.008f, 5.0f);
		m_scale.x = (std::min)(m_scale.x + 0.008f, 5.0f);
		m_scale.z = (std::min)(m_scale.z + 0.008f, 5.0f);
	}
	if (m_pInputInfo->key.down.down)
	{
		//縮小
		m_scale.x = (std::max)(m_scale.x - 0.008f, 0.005f);
		m_scale.x = (std::max)(m_scale.x - 0.008f, 0.005f);
		m_scale.z = (std::max)(m_scale.z - 0.008f, 0.005f);
	}
}

void Player::Reset()
{
	m_isGrounded = false;
	m_isSpringJump = false;
	m_ignoreCollisionFrame = 3;
	direction = 0;
	minAnimIndex = 0;
	maxAnimIndex = 0;
	animUpdateRate = 10;
	isMoving = false;
	isShooting = false;
	bbActive = false;
	canRun = false;
	runTimerStarted = false;
	bbSlowMoveSpeed = false;
	runTimer.Mark();
	gameTimer.Mark();
}