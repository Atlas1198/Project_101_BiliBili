#pragma once
#include "ObjectManagerBase.h"
#include "TransformItem.h"
#include "SharedStruct.h"
#include <vector>
#include <memory>

class TransformItemManager : public ObjectManagerBase
{
public:
    TransformItemManager() {}
    ~TransformItemManager() {}
    const wchar_t *texPath = L"asset/texture/change_item.png";

protected:
    void InitializeOverride(InputManager* pInputManager,
        TextureManager& textureManager,
        MeshManager& meshManager,
        CollisionManager& collisionManager) override;

    void UpdateOverride() override;
    void SubmitDrawsOverride(Renderer& renderer) override;
    void ResolveCollisionsOverride() override;
    void FinalizeOverride() override;
    void PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager) override;

private:
    std::vector<std::unique_ptr<TransformItem>> m_items;
    float m_spawnTimer = 0.0f;
    std::vector<RenderInfo> m_itemInfo;
};