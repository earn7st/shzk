#pragma once

#include <string>
#include "application/SDLWindow.h"
#include "render/Renderer.h"
#include "render/SceneRenderer.h"
#include "resource/ResourceManager.h"
#include "scene/Camera.h"
#include "scene/Scene.h"

class Application
{

public:
	Application() = default;
	Application(const std::string& name, uint32_t width = 1920, uint32_t height = 1080);

	~Application() = default;

	void Init();
	void Run();

	void Shutdown();

private:
	std::string applicationName;
	SDLWindow window;
	Scene scene;
	Camera mainCamera;
	ResourceManager resourceManager;

	Renderer renderer;
	SceneRenderer sceneRenderer;

	double totalTime = 0.0f;
	double deltaTime = 0.0f;

	void Tick(double deltaTime);
	void Render();

	bool bSwapchainDirty = false;
	void OnWindwoResized(uint32_t width, uint32_t height);
};

