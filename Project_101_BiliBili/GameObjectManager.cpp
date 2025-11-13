#include "GameObjectManager.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "CollisionManager.h"
#include "Collider.h"

using namespace RenderData;

//描画要求をシーンに提出
void GameObjectManager::SubmitDraws(
	Renderer& renderer,					//シーンの参照
	const GameObject& objects,			//ゲームオブジェクト配列の参照
	const std::vector<RenderInfo>& info	//描画情報構造体
)
{
	//アクティブなオブジェクトの描画要求をシーンに提出
	if (objects.IsActive() && objects.IsDrawn())
	{//アクティブかつ描画フラグが立っている場合
		for (auto& i : info)
		{
			RenderInfo copy = i;					//描画情報構造体のコピー
			copy.world = objects.GetWorldMatrix();	//ワールド行列の取得
			copy.color = objects.GetColor();		//オブジェクトの色RGBAの取得
			renderer.Submit(copy);					//描画要求をシーンに提出
		}
	}
}

//コライダー描画要求をシーンに提出
void GameObjectManager::SubmitColliders(CollisionManager& collisionManager, Collider* pCollider)
{
	collisionManager.RegisterCollider(pCollider);
}
