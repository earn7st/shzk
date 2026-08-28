#include "Editor.h"

#include "runtime/global/Engine.h"
#include "runtime/framework/Scene.h"
#include "runtime/framework/Node.h" 
#include "runtime/framework/components/TransformComponent.h"
#include "runtime/framework/components/CameraComponent.h"

namespace shzk
{
	void Editor::Init(std::shared_ptr<Scene> scene)
	{
		if (!scene->GetActiveCamera())
		{
			scene->AddNode(CreateDefaultCamera());
		}
	}

	void Editor::Run()
	{
		while (!Engine::ShouldClose())
		{
			Engine::Tick();
		}
	}
	
	void Editor::Shutdown()
	{
		Engine::Shutdown();
	}

	// --- static functions ---
	std::shared_ptr<Node> Editor::CreateDefaultCamera()
	{
		std::shared_ptr<shzk::Node> camera = std::make_shared<shzk::Node>(0, "main_camera");

		std::shared_ptr<shzk::TransformComponent> transformComp = std::make_shared<shzk::TransformComponent>();
		shzk::Transform transform{};
		transform.translation = glm::vec3(0.f, 0.f, 5.f);
		transformComp->SetTransform(transform);

		std::shared_ptr<shzk::CameraComponent> cameraComp = std::make_shared<shzk::CameraComponent>();
		cameraComp->SetIsActiveCamera(true);

		camera->AddComponent(transformComp);
		camera->AddComponent(cameraComp);

		return camera;
	}

}