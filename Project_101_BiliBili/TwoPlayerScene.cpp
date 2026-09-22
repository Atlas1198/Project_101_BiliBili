#include "TwoPlayerScene.h"

#include "EventManager.h"
#include "InputInfo.h"

TwoPlayerScene::TwoPlayerScene(float windowWidth, float windowHeight)
	: SceneBase(windowWidth, windowHeight)
{
    m_gameUIManager = std::make_unique<GameUIManager>(
        &m_pCamera->GetCameraInfo(), windowWidth, windowHeight);
}

void TwoPlayerScene::InitializeOverride(TextureManager& pTextureManager, MeshManager& pMeshManager)
{
    // Set the dedicated stage before PlayerManager applies stage settings.
    m_pSceneContext->stageType = STAGE_TYPE::STAGE_TWO;
    m_inputSystem.Update(nullptr);
    m_bulletManager.Initialize(
        m_pSceneContext, pTextureManager, pMeshManager, *m_pCollisionManager);
    auto& players = m_playerManager.GetPlayers();
    if (players.empty())
    {
        // Create one character per controller half, four in total.
        for (uint32_t i = 0; i < TwoPlayerInputSystem::CHARACTER_COUNT; ++i)
        {
            m_playerManager.AddPlayer(i, m_pSceneContext->pInputInfo,
                *m_pCollisionManager, &m_bulletManager);
        }
    }
    // Pair adjacent characters as teammates and assign their split input.
    for (std::size_t i = 0; i < players.size(); ++i)
    {
        players[i]->SetTeamID(static_cast<int>(i / 2));
        players[i]->SetControllerID(static_cast<int>(i / 2));
        players[i]->BindTeammate(players[i ^ 1]);
        players[i]->SetCharacterInput(m_inputSystem.GetCharacterInput(i));
        m_pSceneContext->playersInfo[i].controllerID = static_cast<int>(i / 2);
        m_pSceneContext->playersInfo[i].characterID = static_cast<int>(i);
    }
    m_playerManager.Initialize(
        m_pSceneContext, pTextureManager, pMeshManager, *m_pCollisionManager);
    m_playerManager.SetInputAccepted(true);
    m_fieldManager.Initialize(
        m_pSceneContext, pTextureManager, pMeshManager, *m_pCollisionManager);

    m_gameUIManager->Initialize(
        pTextureManager, pMeshManager, *m_pSceneContext);
    m_bulletManager.SetGameUIManager(m_gameUIManager.get());

    EventManager::GetInstance()->TriggerEvent<std::pair<int, bool>>(
        EventType::SET_BULLET_UI_ACTIVE, { 0, true });
    EventManager::GetInstance()->TriggerEvent<std::pair<int, bool>>(
        EventType::SET_BULLET_UI_ACTIVE, { 1, true });

    // Place a stationary enemy at the center of the two-player stage.
    m_enemy = std::make_unique<Enemy>(DirectX::XMFLOAT3(0.0f, -4.0f, 3.5f));
    m_enemy->Initialize(pTextureManager, pMeshManager, *m_pCollisionManager);

    // Supply all players to the lines connecting each teammate pair.
    m_bbManager.SetGameUIManager(m_gameUIManager.get());
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
    m_gameUIManager->StartFadeIn(0.01f);
}

void TwoPlayerScene::UpdateOverride()
{
    // Split current controller input before updating the players.
    m_inputSystem.Update(m_pSceneContext ? m_pSceneContext->pInputInfo : nullptr);
    m_playerManager.Update();
    m_fieldManager.Update();
    m_bulletManager.Update();
    if (m_enemy)
    {
        m_enemy->Update();
    }
    // Refresh line endpoints after player movement.
    m_bbManager.SetPlayerData(m_playerManager.GetPlayers());
    m_bbManager.Update();
    m_gameUIManager->Update();

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
    m_gameUIManager->SubmitDraws(pRenderer);
    m_bulletManager.SubmitDraws(pRenderer);
    if (m_enemy)
    {
        m_enemy->SubmitDraw(pRenderer);
    }
    m_bbManager.SubmitDraws(pRenderer);
}

void TwoPlayerScene::FinalizeOverride()
{
    m_fieldManager.Finalize();
    m_playerManager.SetInputAccepted(false);
    m_playerManager.Finalize();
    m_gameUIManager->Finalize();
    m_bulletManager.Finalize();
    m_bbManager.Finalize();
    m_enemy.reset();
    // Prevent input state from carrying over when re-entering the scene.
    m_inputSystem.Update(nullptr);
}

void TwoPlayerScene::ResolveCollisions()
{
    m_fieldManager.ResolveCollisions();
    m_playerManager.ResolveCollisions();
    m_bulletManager.ResolveCollisions();
    if (m_enemy)
    {
        m_enemy->ResolveCollisions();
    }
    m_bbManager.ResolveCollisions();
}
