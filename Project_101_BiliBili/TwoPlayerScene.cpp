#include "TwoPlayerScene.h"

#include "EventManager.h"
#include "InputInfo.h"

TwoPlayerScene::TwoPlayerScene(float windowWidth, float windowHeight)
	: SceneBase(windowWidth, windowHeight)
{
}

void TwoPlayerScene::InitializeOverride(TextureManager& pTextureManager, MeshManager& pMeshManager)
{
    m_pSceneContext->stageType = STAGE_TYPE::STAGE_TWO;
    m_inputSystem.Update(nullptr);
    m_bulletManager.Initialize(
        m_pSceneContext, pTextureManager, pMeshManager, *m_pCollisionManager);
    auto& players = m_playerManager.GetPlayers();
    if (players.empty())
    {
        for (uint32_t i = 0; i < TwoPlayerInputSystem::CHARACTER_COUNT; ++i)
        {
            m_playerManager.AddPlayer(i, m_pSceneContext->pInputInfo,
                *m_pCollisionManager, &m_bulletManager);
        }
    }
    for (std::size_t i = 0; i < players.size(); ++i)
    {
        players[i]->SetTeamID(static_cast<int>(i / 2));
        players[i]->SetControllerID(static_cast<int>(i / 2));
        players[i]->BindTeammate(players[i ^ 1]);
        players[i]->SetCharacterInput(m_inputSystem.GetCharacterInput(i));
    }
    m_playerManager.Initialize(
        m_pSceneContext, pTextureManager, pMeshManager, *m_pCollisionManager);
    m_playerManager.SetInputAccepted(true);
    m_fieldManager.Initialize(
        m_pSceneContext, pTextureManager, pMeshManager, *m_pCollisionManager);

    m_bbManager.SetCollisionManager(m_pCollisionManager);
    m_bbManager.Initialize(
        m_pSceneContext, pTextureManager, pMeshManager, *m_pCollisionManager);
    m_bbManager.SetPlayerData(players);
    m_bbManager.Update();

    m_directionalLight.direction = DirectX::XMFLOAT3(-0.2f, -1.0f, 0.4f);
    m_directionalLight.intensity = 1.1f;
    m_directionalLight.ambient = 0.1f;

    m_pCamera->Reset();
    m_pCamera->SetPosition({ 0.0f, 70.0f, -50.0f });
    m_pCamera->SetTarget({ 0.0f, 0.0f, 3.5f });
    m_pCamera->SetFov(DirectX::XMConvertToRadians(20.0f));
    m_pCamera->Update();
}

void TwoPlayerScene::UpdateOverride()
{
    m_inputSystem.Update(m_pSceneContext ? m_pSceneContext->pInputInfo : nullptr);
    m_playerManager.Update();
    m_fieldManager.Update();
    m_bulletManager.Update();
    m_bbManager.SetPlayerData(m_playerManager.GetPlayers());
    m_bbManager.Update();

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
{
    m_fieldManager.SubmitDraws(pRenderer);
    m_playerManager.SubmitDraws(pRenderer);
    m_bulletManager.SubmitDraws(pRenderer);
    m_bbManager.SubmitDraws(pRenderer);
}

void TwoPlayerScene::FinalizeOverride()
{
    m_fieldManager.Finalize();
    m_playerManager.SetInputAccepted(false);
    m_playerManager.Finalize();
    m_bulletManager.Finalize();
    m_bbManager.Finalize();
    m_inputSystem.Update(nullptr);
}

void TwoPlayerScene::ResolveCollisions()
{
    m_fieldManager.ResolveCollisions();
    m_playerManager.ResolveCollisions();
    m_bulletManager.ResolveCollisions();
    m_bbManager.ResolveCollisions();
}
