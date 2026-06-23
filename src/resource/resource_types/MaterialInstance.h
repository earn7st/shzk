#pragma once

#include "resource/ResourceHandle.h"

struct MaterialConstants
{
};

struct MaterialInstance
{
	MaterialHandle material;
	
	TextureHandle baseColorTexture;
	TextureHandle normaltexture;

	MaterialConstants constants;
};