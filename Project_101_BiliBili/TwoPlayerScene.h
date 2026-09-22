#pragma once

#include "SceneBase.h"

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
};