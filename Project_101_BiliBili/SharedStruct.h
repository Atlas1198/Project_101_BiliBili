#pragma once
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>
#include <DirectXMath.h>
#include "DirectXTex.h"
#include <vector>
#include <string>
#include "ComPtr.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "TextureManager.h"

// 頂点データ構造体
struct Vertex
{
	DirectX::XMFLOAT3 position;	//頂点の位置
	DirectX::XMFLOAT3 normal;	//頂点の法線
	DirectX::XMFLOAT2 uv;		//頂点のUV座標
	DirectX::XMFLOAT3 tangent;	//接空間
	DirectX::XMFLOAT4 color;	//頂点色

	static const D3D12_INPUT_LAYOUT_DESC InputLayout; //入力レイアウト

private:
	static const size_t InputLayoutCount = 5;								//入力レイアウトの要素数
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputLayoutCount];	//入力要素の配列
};

//UV矩形構造体
struct UVRect
{
	float u = 0.0f;		//UV矩形の左上X座標
	float v = 0.0f;		//UV矩形の左上Y座標
	float su = 1.0f;	//UV矩形の幅
	float sv = 1.0f;	//UV矩形の高さ
};

//定数バッファ構造体
struct alignas(256) PerObjectConstants
{
	DirectX::XMMATRIX worldMatrix;			//ワールド行列
	DirectX::XMMATRIX worldInvTranspose;	//ワールド逆転置行列
	DirectX::XMMATRIX viewMatrix;			//ビュー行列
	DirectX::XMMATRIX projMatrix;			//プロジェクション行列
	DirectX::XMFLOAT4 objectColor;			//オブジェクトの色
	DirectX::XMFLOAT4 uvRect;				//UV矩形

	//lighting related data
	DirectX::XMFLOAT4 lightDir_Intensity;	//light direction (xyz) and intensity (w)
	DirectX::XMFLOAT4 lightColor_Ambient;	//light color (xyz) and ambient intensity (w)
};

//時間関連定数構造体
struct alignas(256) TimeConstants
{
	float time;				//経過時間
	float bbTimer;			//BBタイマー
	float bbRemainingTime;	//BBタイマー
};

//directional light structure
struct DirectionalLight
{
	DirectX::XMFLOAT3 direction = { -1.0f, -1.0f, 1.0f };	//light direction
	float intensity = 1.0f;									//light intensity
	DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };			//light color
	float ambient = 0.1f;									//ambient light intensity
	bool enabled = true;									//light enabled flag
};

//3D変換情報構造体
struct Transform3D
{
	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };			//位置
	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };				//スケール
	DirectX::XMFLOAT4 rotation = { 0.0f, 0.0f, 0.0f, 1.0f };	//回転(クォータニオン)
};

//エフェクト用定数バッファ構造体
struct EffectCB
{
	//カメラ関連データ
	DirectX::XMMATRIX viewProj;
	DirectX::XMFLOAT3 camRight;
	float _pad0;
	DirectX::XMFLOAT3 camUp;
	float _pad1;

	//エフェクト関連データ
	DirectX::XMFLOAT3 center;
	float _pad2;
	DirectX::XMFLOAT2 size;
	DirectX::XMFLOAT2 _padSize;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4 uvRect;
};

// カメラ情報構造体
struct CameraInfo
{
	DirectX::XMFLOAT3 position;	//カメラの位置
	DirectX::XMFLOAT3 target;	//カメラの注視点
	DirectX::XMFLOAT3 up;		//カメラの上方向ベクトル
	DirectX::XMFLOAT3 right;	//カメラの右方向ベクトル
	DirectX::XMFLOAT3 forward;	//カメラの前方向ベクトル
	float fov;					//垂直視野角
	float aspectRatio;			//アスペクト比
	float nearZ;				//ニアクリップ距離
	float farZ;					//ファークリップ距離

	DirectX::XMFLOAT2 ConvertWorldToScreen(
		const DirectX::XMFLOAT3& worldPos,	//ワールド座標
		int screenWidth,					//画面幅
		int screenHeight					//画面高さ
	) const;
};

//タグ列挙体
enum class OBJECT_TAG
{
	NONE = 0,			//なし
	PLAYER,				//プレイヤー
	WALL,				//壁
	WALLPASS,			//弾貫通壁
	WALLCURVE,			//曲線壁
	GROUND,				//地面
	BULLET,				//弾
	ITEM_TRANSFORM,		//変身アイテム
	BB_LINE,			//ビリビリライン
	BB_ELECTRICITY,		//ビリビリ電流
	SPRING,				//バネ
	MAX					//最大数
};

//シーン列挙体
enum class SCENE_TYPE
{
	SCENE_NONE = 0,		//シーン無し
	SCENE_TITLE,		//タイトルシーン
	SCENE_CONTROLLER,	//コントローラー設定シーン
	SCENE_STAGE,		//ステージ選択シーン
	SCENE_CHARACTER,	//キャラクター選択シーン
	SCENE_GAME,			//ゲームシーン
	SCENE_RESULT,		//リザルトシーン
};

//方向列挙体
enum class DIRECTION
{
	NONE = 0,	//なし
	UP,			//上
	DOWN,		//下
	LEFT,		//左
	RIGHT,		//右
};

//前方宣言
class Collider;
class ObjectBase;

//衝突データ用名前空間
namespace CollisionData
{
	//衝突状態列挙体
	enum COLLISION_STATE
	{
		COLLISION_NONE = 0,	//衝突なし
		COLLISION_ENTER,	//衝突開始
		COLLISION_STAY,		//衝突継続
		COLLISION_EXIT,		//衝突終了
	};

	//コリジョンレイヤー列挙体
	enum class COLLISION_LAYER
	{
		DEFAULT = 0,	//デフォルト
		PLAYER,			//プレイヤー
		WALL,			//壁
		WALLPASS,		//弾貫通壁
		WALLCURVE,		//曲線壁
		GROUND,			//地面
		BULLET,			//弾
		ITEM_TRANSFORM,	//変身アイテム
		BB_LINE,		//ビリビリライン
		BB_ELECTRICITY,	//ビリビリ電流
		SPRING,			//バネ
		MAX_LAYER		//最大数
	};

	//レイヤーマスク型
	using LayerMask = uint32_t;

	//衝突情報構造体
	struct CollisionInfo
	{
		Collider* opponent = nullptr;								//衝突相手のコライダー
		DirectX::XMFLOAT3 contactPoint = {0.0f, 0.0f, 0.0f};		//衝突点
		DirectX::XMFLOAT3 contactNormal = {0.0f, 0.0f, 0.0f};		//衝突法線
		DirectX::XMFLOAT3 penetrationDepth = {0.0f, 0.0f, 0.0f};	//貫入深さ
		CollisionData::COLLISION_STATE state = 
			CollisionData::COLLISION_STATE::COLLISION_NONE;			//衝突状態
	};

	//オブジェクト衝突情報構造体
	struct ObjectCollisionInfo
	{
		ObjectBase* opponent = nullptr;								//衝突相手のオブジェクト
		DirectX::XMFLOAT3 contactPoint = {0.0f, 0.0f, 0.0f};		//衝突点
		DirectX::XMFLOAT3 contactNormal = {0.0f, 0.0f, 0.0f};		//衝突法線
		DirectX::XMFLOAT3 penetrationDepth = {0.0f, 0.0f, 0.0f};	//貫入深さ
		CollisionData::COLLISION_STATE state = 
			CollisionData::COLLISION_STATE::COLLISION_NONE;			//衝突状態
	};

	//レイキャストヒット情報構造体
	struct RaycastHitInfo
	{
		Collider* opponent = nullptr;						//衝突したコライダー
		DirectX::XMFLOAT3 hitPoint = {0.0f, 0.0f, 0.0f};	//衝突点
		DirectX::XMFLOAT3 hitNormal = {0.0f, 0.0f, 0.0f};	//衝突法線
		float hitDistance = 0.0f;							//衝突距離
	};

	//レイキャストセグメント構造体
	struct RaycastSegment
	{
		DirectX::XMFLOAT3 startPoint = {0.0f, 0.0f, 0.0f};	//始点
		DirectX::XMFLOAT3 endPoint = {0.0f, 0.0f, 0.0f};	//終点
		LayerMask layerMask = 0;							//レイヤーマスク
		COLLISION_LAYER layer = COLLISION_LAYER::DEFAULT;	//レイヤー
		std::vector<RaycastHitInfo> hitInfos;				//ヒット情報配列
	};

	//レイヤーをビットに変換する関数
	LayerMask LayerToBit(COLLISION_LAYER layer);

	//レイヤーマスク取得関数
	LayerMask GetLayerMask(COLLISION_LAYER layer);

	//複数のレイヤーからレイヤーマスクを作成する関数
	LayerMask MakeMask(std::initializer_list<COLLISION_LAYER> layers);

	//貫入深さから押し出しベクトルを取得する関数
	DirectX::XMFLOAT3 GetPushOutVector(
		std::vector<CollisionData::ObjectCollisionInfo>& infos,	//衝突情報配列
		const std::initializer_list<OBJECT_TAG>& tagList	//対象タグリスト
	);
}


//=======================
//ベクトル演算関数群
//=======================
//2点間の距離の二乗を計算する関数
inline static float LengthSqBetween(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
{
	DirectX::XMFLOAT3 diff{
		b.x - a.x,
		b.y - a.y,
		b.z - a.z
	};
	return diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
}

//2点間の距離を計算する関数
inline static float LengthBetween(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
{
	return sqrtf(LengthSqBetween(a, b));
}

//ベクトルの長さを計算する関数(XMFLOAT3版)
inline static float LengthXMF3(const DirectX::XMFLOAT3& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

//ベクトルの長さを計算する関数(XMVECTOR版)
inline static float LengthXMV(const DirectX::XMVECTOR& v)
{
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, v);
	return sqrtf(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z);
}

//ベクトルの正規化を行う関数
inline static DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& v)
{
	float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	if (len > 0.0f)
	{
		return DirectX::XMFLOAT3{ v.x / len, v.y / len, v.z / len };
	}
	else
	{
		return DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	}
}

//内積を計算する関数
inline static float Dot(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

//外積を計算する関数
inline static DirectX::XMFLOAT3 Cross(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
{
	return DirectX::XMFLOAT3{
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

//0～1の範囲に値をクランプする関数
inline static float Clamp01(float value)
{
	if (value < 0.0f)
	{
		return 0.0f;
	}
	else if (value > 1.0f)
	{
		return 1.0f;
	}
	else
	{
		return value;
	}
}

//線形補間を行う関数(float版)
inline static float Lerpf(float start, float end, float t)
{
	return start + (end - start) * t;
}

//線形補間を行う関数(XMFLOAT3版)
inline static DirectX::XMFLOAT3 LerpXMF3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, float t)
{
	return DirectX::XMFLOAT3{
		start.x + (end.x - start.x) * t,
		start.y + (end.y - start.y) * t,
		start.z + (end.z - start.z) * t
	};
}

//線形補間を行う関数(XMVECTOR版)
inline static DirectX::XMVECTOR LerpXMV(const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end, float t)
{
	return DirectX::XMVectorLerp(start, end, t);
}

//イージング関数群
//二次イージングイン
inline static float EaseInQuad(float t)
{
	return t * t;
}

//三次イージングイン
inline static float EaseInCubic(float t)
{
	return t * t * t;
}

//二次イージングアウト
inline static float EaseOutQuad(float t)
{
	return 1.0f - (1.0f - t) * (1.0f - t);
}

//三次イージングアウト
inline static float EaseOutCubic(float t)
{
	float p = 1.0f - t	;
	return 1.0f - p * p * p;
}

//バックイージングアウト
inline static float EaseOutBack(float t)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	float u = t - 1.0f;
	return 1.0f + c3 * u * u * u + c1 * u * u;
}

//3D変換情報合成関数
inline static Transform3D CombineTransform3D(const Transform3D& parent, const Transform3D& local)
{
	using namespace DirectX;

	Transform3D out{};

	out.scale = {
		parent.scale.x * local.scale.x,
		parent.scale.y * local.scale.y,
		parent.scale.z * local.scale.z
	};

	XMVECTOR qP = XMLoadFloat4(&parent.rotation);
	XMVECTOR qL = XMLoadFloat4(&local.rotation);
	XMVECTOR qW = XMQuaternionMultiply(qL, qP);
	qW = XMQuaternionNormalize(qW);
	XMStoreFloat4(&out.rotation, qW);

	XMVECTOR p = XMLoadFloat3(&local.position);
	p = XMVectorMultiply(p, XMLoadFloat3(&parent.scale));
	p = XMVector3Rotate(p, qP);
	p = XMVectorAdd(p, XMLoadFloat3(&parent.position));
	XMStoreFloat3(&out.position, p);

	return out;
}

//変換情報から変換行列を取得する関数
inline static DirectX::XMMATRIX GetMatrixFromTransform3D(const Transform3D& t)
{
	using namespace DirectX;
	XMMATRIX S = XMMatrixScaling(t.scale.x, t.scale.y, t.scale.z);
	XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&t.rotation));
	XMMATRIX T = XMMatrixTranslation(t.position.x, t.position.y, t.position.z);

	return S * R * T;
}

//Get transformation matrix from position, scale, and rotation
inline static DirectX::XMMATRIX GetMatrixFromGeometry(
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& scale,
	const DirectX::XMFLOAT3& rotation
)
{
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(
		DirectX::XMConvertToRadians(rotation.x),
		DirectX::XMConvertToRadians(rotation.y),
		DirectX::XMConvertToRadians(rotation.z));
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	return S * R * T;
}


//スプライト分割情報構造体
struct TexSplitInfo
{
	int index = 0;			//スプライトのインデックス
	int cols = 1;			//列数
	int rows = 1;			//行数
	int total = 1;			//総フレーム数(列数 * 行数)
	int frameCount = 0;		//現在のフレームカウント
	int updateRate = 0;		//更新レート(何フレームに1回進むか)

	float offsetU = 0.0f;	//UVオフセットU
	float offsetV = 0.0f;	//UVオフセットV

	float scaleU = 1.0f;	//UVスケールU
	float scaleV = 1.0f;	//UVスケールV
};

//スプライト分割情報からUV矩形を取得する関数
inline static DirectX::XMFLOAT4 SplitSprite(TexSplitInfo info)
{
	const float baseSu = 1.0f / static_cast<float>(info.cols);	//基本UVスケールU
	const float baseSv = 1.0f / static_cast<float>(info.rows);	//基本UVスケールV

	const int col = info.index % info.cols;	//現在の列
	const int row = info.index / info.cols;	//現在の行

	const float frameU = static_cast<float>(col) * baseSu;	//フレームUVオフセットU
	const float frameV = static_cast<float>(row) * baseSv;	//フレームUVオフセットV

	const float minU = frameU + info.offsetU * baseSu;	//最小U座標
	const float minV = frameV + info.offsetV * baseSv;	//最小V座標

	const float sizeU = baseSu * info.scaleU;	//最大U座標
	const float sizeV = baseSv * info.scaleV;	//最大V座標

	//UV矩形の作成
	return DirectX::XMFLOAT4(minU, minV, sizeU, sizeV);
}

//クオータニオンからオイラー角への変換
inline DirectX::XMFLOAT3 QuaternionToEuler(const DirectX::XMVECTOR& q)
{
	using namespace DirectX;

	// q = (x, y, z, w)
	XMFLOAT4 fq;
	XMStoreFloat4(&fq, q);

	// --- x軸回転（Pitch として使うやつ） ---
	// roll_x = atan2( 2(w*x + y*z), 1 - 2(x^2 + y^2) )
	float sinr_cosp = 2.0f * (fq.w * fq.x + fq.y * fq.z);
	float cosr_cosp = 1.0f - 2.0f * (fq.x * fq.x + fq.y * fq.y);
	float rotX = std::atan2(sinr_cosp, cosr_cosp);

	// --- y軸回転（Yaw として使うやつ） ---
	// pitch_y = asin( 2(w*y - z*x) )   （±90°近辺でクランプ）
	float sinp = 2.0f * (fq.w * fq.y - fq.z * fq.x);
	if (sinp > 1.0f)  sinp = 1.0f;
	if (sinp < -1.0f) sinp = -1.0f;
	float rotY = std::asin(sinp);

	// --- z軸回転（Roll として使うやつ） ---
	// yaw_z = atan2( 2(w*z + x*y), 1 - 2(y^2 + z^2) )
	float siny_cosp = 2.0f * (fq.w * fq.z + fq.x * fq.y);
	float cosy_cosp = 1.0f - 2.0f * (fq.y * fq.y + fq.z * fq.z);
	float rotZ = std::atan2(siny_cosp, cosy_cosp);

	// X: x軸まわり, Y: y軸まわり, Z: z軸まわり
	return XMFLOAT3(rotX, rotY, rotZ);
}