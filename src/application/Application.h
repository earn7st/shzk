#pragma once

#include <string>
#include "application/SDLWindow.h"
#include "render/Renderer.h"
#include "render/SceneRenderer.h"
#include "resource/ResourceManager.h"
#include "scene/Camera.h"
#include "scene/Scene.h"
#include "scene/FPSCameraController.h"

class Application
{

public:
	Application() = default;
	Application(const std::string& name, uint32_t width = 1600, uint32_t height = 900);

	~Application() = default;

	void Init();
	void Run();

	void Shutdown();

private:
	std::string applicationName;
	SDLWindow window;
	Scene scene;
	FPSCameraController fpsController;
	Camera mainCamera;
	ResourceManager resourceManager;
	
	Renderer renderer;
	SceneRenderer sceneRenderer;

	void Tick(double deltaTime);
	void Render();

	double totalTime = 0.0f;
	double deltaTime = 0.0f;

	// Window Resize
	bool bSwapchainDirty = false;
	void OnWindowResized(uint32_t width, uint32_t height);

};

