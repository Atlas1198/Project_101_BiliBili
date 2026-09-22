#pragma once

#include "SceneBase.h"
#include "FieldManager.h"
#include "TwoPlayerInputSystem.h"
#include "PlayerManager.h"
#include "BBManager.h"
#include "Enemy.h"
#include "GameUIManager.h"
#include "ItemManager.h"
#include <memory>

class Renderer;
class TextureManager;
class MeshManager;

// Two-player scene where each controller operates two characters.
class TwoPlayerScene final : public SceneBase
{
public:
	TwoPlayerScene(float windowWidth, float windowHeight);

	void InitializeOverride(TextureManager& pTextureManager, 
		MeshManager& pMeshManager) override;

	void UpdateOverride() override;
	void DrawOverride(Renderer& pRenderer) override;
	void FinalizeOverride() override;
	void ResolveCollisions() override;

private:
	TwoPlayerInputSystem m_inputSystem;
	FieldManager m_fieldManager;
	BulletManager m_bulletManager;
	PlayerManager m_playerManager;
	BBManager m_bbManager;
	ItemManager m_itemManager;
	std::unique_ptr<GameUIManager> m_gameUIManager;
	std::unique_ptr<Enemy> m_enemy;
};
