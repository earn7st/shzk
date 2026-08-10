#include <iostream>
#include <cassert>

#include "runtime/global/Engine.h"
#include "runtime/import/GltfLoader.h"
#include "runtime/asset/Model.h"
#include "runtime/asset/Texture.h"
#include "runtime/asset/AssetManager.h"
#include "runtime/asset/Material.h"
#include "runtime/core/Primitive.h"
#include "runtime/render/resources/Buffer.h"


static int g_passed = 0;
static int g_failed = 0;

#define TEST_ASSERT(cond, msg)                              \
      do {                                                     \
          if (!(cond)) {                                       \
              std::cerr << "  FAIL: " << msg << std::endl;     \
              ++g_failed;                                      \
          } else {                                             \
              ++g_passed;                                      \
          }                                                    \
      } while (0)

void PrintResult(const shzk::GltfLoadResult& result)
{
    std::cout << "--- Textures (" << result.textures.size() << ") ---" << std::endl;
    for (size_t i = 0; i < result.textures.size(); ++i)
    {
        if (result.textures[i])
            std::cout << "  [" << i << "] " << result.textures[i]->GetName() << std::endl;
        else
            std::cout << "  [" << i << "] nullptr" << std::endl;
    }

    std::cout << "--- Materials (" << result.materials.size() << ") ---" << std::endl;
    for (size_t i = 0; i < result.materials.size(); ++i)
    {
        if (!result.materials[i])
        {
            std::cout << "  [" << i << "] nullptr" << std::endl;
            continue;
        }
        const auto& m = result.materials[i];
        std::cout << "  [" << i << "] baseColor=("
            << m->m_baseColor.x << ", " << m->m_baseColor.y << ", "
            << m->m_baseColor.z << ", " << m->m_baseColor.w << ")"
            << "  metallic=" << m->m_metallic
            << "  roughness=" << m->m_roughness
            << "  alphaCutoff=" << m->m_alphaCutoff
            << std::endl;
        std::cout << "        diffuse=" << (m->m_textureDiffuse ? "true" : "false")
            << "  normal=" << (m->m_textureNormal ? "true" : "false")
            << "  metallicRough=" << (m->m_textureArm ? "true" : "false")
            << std::endl;
    }

    std::cout << "--- Models (" << result.models.size() << ") ---" << std::endl;
    for (size_t i = 0; i < result.models.size(); ++i)
    {
        const auto& model = result.models[i];
        std::cout << "  [" << i << "] " << model->GetName()
            << "  submeshes=" << model->GetSubmeshes().size() << std::endl;

        for (size_t j = 0; j < model->GetSubmeshes().size(); ++j)
        {
            const auto& sm = model->GetSubmeshes()[j];
            const auto& prim = sm.primitive;
            std::cout << "        Submesh[" << j << "]: ";
            if (prim)
            {
                std::cout << "verts=" << prim->position.size()
                    << "  normals=" << prim->normal.size()
                    << "  texcoords=" << prim->texcoord.size();
            }
            std::cout << "  VB=" << (sm.vertexBuffer ? "true" : "false")
                << "(" << (sm.vertexBuffer ? sm.vertexBuffer->GetVertexNum() : 0) << ")"
                << "  IB=" << (sm.indexBuffer ? "true" : "false")
                << "(" << (sm.indexBuffer ? sm.indexBuffer->GetIndexNum() : 0) << ")"
                << "  material=" << (sm.material ? "true" : "false");
            if (sm.material)
            {
                std::cout << " (baseColor=" << sm.material->m_baseColor.x
                    << "," << sm.material->m_baseColor.y
                    << "," << sm.material->m_baseColor.z << ")";
            }
            std::cout << std::endl;
        }
    }
}

void TestBasicMesh()
{
    std::cout << "\n========== Test: basicmesh.glb ==========" << std::endl;

    shzk::GltfLoader loader;
    shzk::GltfLoadResult result;
    loader.Load(SHZK_ASSETS_DIR "basicmesh.glb", result);

    PrintResult(result);

    shzk::AssetManager::Get()->ProcessGltfLoadResult(result);

    TEST_ASSERT(!result.models.empty(), "at least 1 model");
    TEST_ASSERT(result.models[0]->GetSubmeshes().size() > 0, "at least 1 submesh");

    const auto& sm = result.models[0]->GetSubmeshes()[0];
    TEST_ASSERT(sm.primitive != nullptr, "primitive populated");
    TEST_ASSERT(sm.primitive->position.size() > 0, "positions not empty");
    TEST_ASSERT(sm.vertexBuffer != nullptr, "vertexBuffer created");
    TEST_ASSERT(sm.vertexBuffer->GetVertexNum() == sm.primitive->position.size(),
        "vertexBuffer count matches positions");
    if (sm.indexBuffer)
    {
        std::cout << "  Index count: " << sm.indexBuffer->GetIndexNum() << std::endl;
    }
}

void TestDamagedHelmetGltf()
{
    std::cout << "\n========== Test: DamagedHelmet.gltf ==========" << std::endl;

    shzk::GltfLoader loader;
    shzk::GltfLoadResult result;
    loader.Load(SHZK_ASSETS_DIR "DamagedHelmet/glTF/DamagedHelmet.gltf", result);

    PrintResult(result);

    shzk::AssetManager::Get()->ProcessGltfLoadResult(result);

    TEST_ASSERT(!result.models.empty(), "at least 1 model");
    TEST_ASSERT(!result.materials.empty(), "materials populated");

    const auto& mat = result.materials[0];
    if (mat->m_textureDiffuse)
    {
        std::cout << "  Texture diffuse loaded: " << mat->m_textureDiffuse->GetName() << std::endl;
    }
    else
    {
        std::cout << "  Texture diffuse: nullptr (Texture path bug: "
            "filename() stripped directory)" << std::endl;
    }
}

int main()
{
    std::cout << "=== GltfLoader Test Suite ===" << std::endl;

    shzk::EngineInitInfo engineInitInfo{};
    engineInitInfo.name = "gltf_test";
    engineInitInfo.width = 800;
    engineInitInfo.height = 600;
    shzk::Engine::Init(engineInitInfo);

    TestBasicMesh();
    TestDamagedHelmetGltf();

    std::cout << "\n=== " << g_passed << " passed, "
        << g_failed << " failed ===" << std::endl;

    shzk::AssetManager::Get()->PrintAllAssets();

    shzk::Engine::Shutdown();
    return g_failed > 0 ? 1 : 0;
	return 0;
}