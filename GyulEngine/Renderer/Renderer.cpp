#include "Renderer.h"

Renderer* Renderer::spInstance = nullptr;

Renderer::Renderer()
	: mpMainCamera(nullptr)
{
}

Renderer::~Renderer()
{

}
