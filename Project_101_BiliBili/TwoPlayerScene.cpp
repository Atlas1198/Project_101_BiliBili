#include "TwoPlayerScene.h"

#include "EventManager.h"
#include "InputInfo.h"

TwoPlayerScene::TwoPlayerScene(float windowWidth, float windowHeight)
	: SceneBase(windowWidth, windowHeight)
{
}

void TwoPlayerScene::InitializeOverride(TextureManager& pTextureManager, MeshManager& pMeshManager)
{}

void TwoPlayerScene::UpdateOverride()
{
    if (!m_pSceneContext || !m_pSceneContext->pInputInfo)
    {
        return;
    }

    const auto* inputInfo = m_pSceneContext->pInputInfo;

    // 動作確認用：Spaceキーでタイトルへ戻る
    if (inputInfo->key.space.trigger)
    {
        EventManager::GetInstance()->TriggerEvent<SCENE_TYPE>(
            EventType::CHANGE_SCENE,
            SCENE_TYPE::SCENE_TITLE
        );
    }

}

void TwoPlayerScene::DrawOverride(Renderer& pRenderer)
{}

void TwoPlayerScene::FinalizeOverride()
{}

void TwoPlayerScene::ResolveCollisions()
{}