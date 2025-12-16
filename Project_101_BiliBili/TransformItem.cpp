#include "TransformItem.h"
#include "Player.h"

TransformItem::TransformItem(DirectX::XMFLOAT3 pos)
    : ObjectBase(
        MESH_TYPE::CUBE,                          //メッシュタイプ
        pos,                                                //座標
        { 0,0,0 },                                          //回転
        { 1,1,1 },                                          //スケール
        { 0,0,0 },                                          //移動速度
        true,                                               //アクティブフラグ
        OBJECT_TAG::ITEM_TRANSFORM,                         //オブジェクトタグ
        CollisionData::COLLISION_LAYER::ITEM_TRANSFORM)     //衝突レイヤー
{
    // コライダーセットの作成
    m_pColliderSet->AddCollider(
        ColliderType::BOX,                     //コライダータイプ
        { 0.0f, 0.0f, 0.0f },                  //ローカル中心座標
        { 1.0f, 1.0f, 1.0f },                  //ローカルスケール
        { 0.0f, 0.0f, 0.0f }                   //ローカル回転
	);
}

void TransformItem::UpdateOverride()
{
    if (!IsActive()) 
    {
        return;
    }

    m_livedTime += 1.0f / 60.0f; 
    if (m_livedTime >= m_maxLifeTime)
    {
        SetActive(false);
    }
}

void TransformItem::ResolveCollisionsOverride()
{
    if (!IsActive()) 
    {
        return;
    }

    for (auto& c : m_pColliderSet->GetCollisionInfos())
    {
        ObjectBase* p = c.opponent;
        if (p && p->GetTag() == OBJECT_TAG::PLAYER)
        {
            Player* player = dynamic_cast<Player*>(p);
            if (player)
            {
                //int team = player->GetTeamID();
                
                //変身

                //g_TransformSystem->ApplyTransform(player);

                SetActive(false);
                break;
            }
        
        }
    }

    // 衝突情報クリア
    m_pColliderSet->ClearCollisionInfos();
}