#include "Sky.h"
#include "Graphics/Graphics.h"
#include "Graphics/Shader.h"

Sky::Sky()
{
	model = std::make_unique<Model>("Data/Environment/0.Sky.mdl");
}

Sky::~Sky()
{
}

// •`‰æˆ—
void Sky::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}