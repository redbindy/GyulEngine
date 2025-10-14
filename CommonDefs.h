#pragma once

#define SHADER_FOLDER TEXT("./Renderer/Shaders/")
#define SHADER_PATH(shaderStrAscii) (SHADER_FOLDER TEXT(shaderStrAscii))

#define RESOURCE_FOLDER TEXT("./Resources/")
#define RESOURCE_PATH(resourceStrAscii) (RESOURCE_FOLDER TEXT(resourceStrAscii))

enum EConstant
{
	MAX_LABEL_LENGTH = 128
};