
// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>
#include <memory>
#include "editor/Editor.h"
#include "runtime/core/Transform.h"
#include "runtime/asset/AssetManager.h"
#include "runtime/global/Engine.h"
// framework
#include "runtime/framework/Scene.h"
#include "runtime/framework/Node.h"
#include "runtime/framework/components/MeshComponent.h"
#include "runtime/framework/components/CameraComponent.h"	
#include "runtime/framework/components/TransformComponent.h"	
//import
#include "runtime/import/GltfLoader.h"

void InitScene(std::shared_ptr<shzk::Scene>& scene)
{
	shzk::GltfLoader gltfLoader;
	shzk::GltfLoadResult result;
	gltfLoader.Load(SHZK_ASSETS_DIR "DamagedHelmet/glTF/DamagedHelmet.gltf", result);
	shzk::AssetManager::Get()->ProcessGltfLoadResult(result);

	// Camera
	if (true)
	{
		std::shared_ptr<shzk::Node> camera = std::make_shared<shzk::Node>(0, "main_camera");

		std::shared_ptr<shzk::TransformComponent> transformComp = std::make_shared<shzk::TransformComponent>();
		shzk::Transform transform{};
		transform.translation = glm::vec3(0.f, 0.f, 5.f);
		transformComp->SetTransform(transform);

		std::shared_ptr<shzk::CameraComponent> cameraComp = std::make_shared<shzk::CameraComponent>();
		
		camera->AddComponent(transformComp);
		camera->AddComponent(cameraComp);
	
		scene->AddNode(camera);
	}
	
	// Damaged Helmet
	if (true)
	{
		std::shared_ptr<shzk::Node> helmet0 = std::make_shared<shzk::Node>(1, "damaged_helmet0");
		std::shared_ptr<shzk::Node> helmet1 = std::make_shared<shzk::Node>(2, "damaged_helmet1");
		
		shzk::Transform transform{};
		transform.translation = glm::vec3(1.f, 0.f, 0.f);
		transform.rotation = glm::angleAxis(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		
		{
			std::shared_ptr<shzk::TransformComponent> transformComp = std::make_shared<shzk::TransformComponent>();
			transformComp->SetTransform(transform);
			helmet0->AddComponent(transformComp);
			std::shared_ptr<shzk::MeshComponent> meshComp = std::make_shared<shzk::MeshComponent>();
			meshComp->SetModel(result.models[0]);
			helmet0->AddComponent(meshComp);
		}

		{
			std::shared_ptr<shzk::TransformComponent> transformComp = std::make_shared<shzk::TransformComponent>();
			transform.translation = glm::vec3(-1.f, 0.f, 0.f);
			transformComp->SetTransform(transform);
			helmet1->AddComponent(transformComp);
			std::shared_ptr<shzk::MeshComponent> meshComp = std::make_shared<shzk::MeshComponent>();
			meshComp->SetModel(result.models[0]);
			helmet1->AddComponent(meshComp);
		}
		
		scene->AddNode(helmet0);
		scene->AddNode(helmet1);
	}
	
}

int main()
{
	// _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);

	shzk::EngineInitInfo engineInitInfo{};
	engineInitInfo.name = "shzk";
	engineInitInfo.width = 1280;
	engineInitInfo.height = 720;

	shzk::Engine::Init(engineInitInfo);

	std::shared_ptr<shzk::Scene> scene = std::make_shared<shzk::Scene>();
	InitScene(scene);
	shzk::Engine::Get()->SetActiveScene(scene);

	shzk::Editor* editor = new shzk::Editor();
	editor->Run();
	editor->Shutdown();

	return 0;
}