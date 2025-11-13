#include "Camera.h"
#include "SceneManager.h"

Camera::Camera(float window_width, float window_height)
{
	// デフォルトのカメラ設定
	m_position = DEFAULT_POSITION;					//カメラ位置
	m_target = DEFAULT_TARGET;						//注視点
	m_up = DEFAULT_UP;								//上方向ベクトル
	m_fov = DEFAULT_FOV;							//垂直視野角
	m_aspectRatio = window_width / window_height;	//アスペクト比
	m_nearZ = DEFAULT_NEAR_Z;						//ニアクリップ距離
	m_farZ = DEFAULT_FAR_Z;							//ファークリップ距離
}

//初期化
void Camera::Initialize(InputManager* pInputManager)
{
	m_pInputInfo = pInputManager->GetInputInfo();	//入力情報構造体の取得
}

//カメラ更新
void Camera::Update()
{
	//カメラ移動処理などをここに追加可能
	//if (m_pInputInfo->w.down)
	//{
	//	//前進
	//	m_position.z += 0.1f;

	//}
	//if (m_pInputInfo->s.down)
	//{
	//	//後退
	//	m_position.z -= 0.1f;
	//}
	//if (m_pInputInfo->a.down)
	//{
	//	//左移動
	//	m_position.x -= 0.1f;
	//}
	//if (m_pInputInfo->d.down)
	//{
	//	//右移動
	//	m_position.x += 0.1f;
	//}

	UpdateCameraInfo(); //カメラ情報構造体を更新
}

//カメラの位置を設定
void Camera::SetPosition(const DirectX::XMFLOAT3& position)
{
	m_position = position;
}

//カメラの注視点を設定
void Camera::SetTarget(const DirectX::XMFLOAT3& target)
{
	m_target = target;
}

//カメラの上方向ベクトルを設定
void Camera::SetUp(const DirectX::XMFLOAT3& up)
{
	m_up = up;
}

//垂直視野角を設定
void Camera::SetFov(float fov)
{
	m_fov = fov;
}

//アスペクト比を設定
void Camera::SetAspectRatio(float aspectRatio)
{
	m_aspectRatio = aspectRatio;
}

//ニアクリップ距離を設定
void Camera::SetNearZ(float nearZ)
{
	m_nearZ = nearZ;
}

//ファークリップ距離を設定
void Camera::SetFarZ(float farZ)
{
	m_farZ = farZ;
}

//カメラ情報構造体を取得
CameraInfo* Camera::GetCameraInfo()
{
	UpdateCameraInfo(); //カメラ情報構造体を更新

	return &m_cameraInfo;	//カメラ情報構造体を返す
}

//カメラ情報構造体を更新
void Camera::UpdateCameraInfo()
{
	m_cameraInfo.position = m_position;			//カメラの位置
	m_cameraInfo.target = m_target;				//カメラの注視点
	m_cameraInfo.up = m_up;						//カメラの上方向ベクトル
	m_cameraInfo.fov = m_fov;					//垂直視野角
	m_cameraInfo.aspectRatio = m_aspectRatio;	//アスペクト比
	m_cameraInfo.nearZ = m_nearZ;				//ニアクリップ距離
	m_cameraInfo.farZ = m_farZ;					//ファークリップ距離
}
