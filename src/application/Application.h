#pragma once

#include "platform/SDLWindow.h"
#include "assets/AssetManager.h"
#include "render/Renderer.h"
#include "scene/Scene.h"

class Application
{

private:

	SDLWindow _window;
	Scene _scene;
	AssetManager _assetManager;
	Renderer _renderer;

public:
	void Init();
	void Run();
	void Shutdown();

};
