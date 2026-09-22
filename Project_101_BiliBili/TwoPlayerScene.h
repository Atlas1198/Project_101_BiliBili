#pragma once

#include "SceneBase.h"
#include "FieldManager.h"
#include "TwoPlayerInputSystem.h"
#include "PlayerManager.h"
#include "BBManager.h"

class Renderer;
class TextureManager;
class MeshManager;

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
};
