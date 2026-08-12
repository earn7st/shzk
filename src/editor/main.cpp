
// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>
#include <memory>
#include "editor/Editor.h"
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

	// Camera
	if (true)
	{
		std::shared_ptr<shzk::Node> camera = std::make_shared<shzk::Node>(0, "main_camera");

		std::shared_ptr<shzk::TransformComponent> transformComponent = std::make_shared<shzk::TransformComponent>();
		std::shared_ptr<shzk::CameraComponent>	cameraComponent = std::make_shared<shzk::CameraComponent>();

		camera->AddComponent(transformComponent);
		camera->AddComponent(cameraComponent);
	
		scene->AddNode(camera);
	}
	
	// Damaged Helmet
	if (true)
	{
		std::shared_ptr<shzk::Node> helmet = std::make_shared<shzk::Node>(1, "damaged_helmet");
		
		std::shared_ptr<shzk::TransformComponent> transform = std::make_shared<shzk::TransformComponent>();
		std::shared_ptr<shzk::MeshComponent> mesh = std::make_shared<shzk::MeshComponent>();
		mesh->SetModel(result.models[0]);

		helmet->AddComponent(transform);
		helmet->AddComponent(mesh);

		scene->AddNode(helmet);
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