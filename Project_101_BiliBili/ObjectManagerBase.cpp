#include "ObjectManagerBase.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "CollisionManager.h"
#include "Collider.h"

using namespace DirectX;



//初期化
void ObjectManagerBase::Initialize(InputManager* pInputManager, TextureManager& textureManager, MeshManager& meshManager, CollisionManager& collisionManager)
{
	//派生クラスでオーバーライドされた初期化関数を呼び出し
	InitializeOverride(pInputManager, textureManager, meshManager, collisionManager);

	//オブジェクト描画情報生成
	PrepareRenderInfo(textureManager, meshManager);
}

//更新
void ObjectManagerBase::Update()
{
	//派生クラスでオーバーライドされた更新関数を呼び出し
	UpdateOverride();
}

//描画要求をシーンに提出
void ObjectManagerBase::SubmitDraws(Renderer& renderer)
{
	//派生クラスでオーバーライドされた描画要求提出関数を呼び出し
	SubmitDrawsOverride(renderer);
}

//衝突後処理
void ObjectManagerBase::ResolveCollisions()
{
	//派生クラスでオーバーライドされた衝突後処理関数を呼び出し
	ResolveCollisionsOverride();
}

//終了
void ObjectManagerBase::Finalize()
{
	//派生クラスでオーバーライドされた終了関数を呼び出し
	FinalizeOverride();
}

//描画情報をシーンに提出
void ObjectManagerBase::SubmitRenderInfo(
	Renderer& renderer,				//シーンの参照
	const ObjectBase& object,		//ゲームオブジェクト配列の参照
	std::vector<WorldRenderInfo>& info	//描画情報構造体
)
{
	//アクティブなオブジェクトの描画要求をシーンに提出
	if (object.IsActive() && object.IsDrawn())
	{//アクティブかつ描画フラグが立っている場合

		std::vector<WorldRenderInfo> submitInfos;	//Rendererへの提出用描画情報構造体配列
		submitInfos.reserve(info.size());		//容量確保

		if(object.GetMeshType() == MESH_TYPE::CAPSULE)
		{//カプセルメッシュの場合(複数メッシュに分かれているため個別に処理)
			CapsuleVisualDesc desc{};	//カプセルメッシュの記述データ
			//カプセルメッシュの記述データ設定
			AppendCapsuleRenderInfos(
				desc,					//カプセル描画情報記述子
				object.GetPosition(),	//位置
				object.GetScale(),		//スケール
				object.GetRotation(),	//回転Euler角
				object.GetColor(),		//色
				info,					//入力元描画情報配列
				submitInfos				//出力先描画情報配列
			);
		}
		else
		{//それ以外のメッシュの場合
			//描画情報構造体配列をそのまま提出用配列にコピー
			for (auto& i : info)
			{
				submitInfos.push_back(i);
			}

			//ワールド行列と色を設定
			for(auto& i : submitInfos)
			{
				i.world = object.GetWorldMatrix();
				i.common.color = object.GetColor();
			}
		}

		//共通要素の設定
		for (int i = 0; i < submitInfos.size(); i++)
		{
			submitInfos[i].position = object.GetPosition();
			submitInfos[i].scale = object.GetScale();
			submitInfos[i].common.blendMode = info[i].common.blendMode;
			submitInfos[i].common.uvRect = SplitSprite(object.GetTexSplitInfo());
			submitInfos[i].billboardType = info[i].billboardType;
		}

		//描画要求をシーンに提出
		for (auto& i : submitInfos)
		{
			renderer.SubmitToWorldList(i);
		}

	}
}

//コライダー描画要求をシーンに提出
void ObjectManagerBase::SubmitColliders(CollisionManager& collisionManager, Collider* pCollider)
{
	collisionManager.RegisterCollider(pCollider);
}
