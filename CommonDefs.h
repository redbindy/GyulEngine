#pragma once

#define SHADER_FOLDER "./Renderer/Shaders/"
#define SHADER_PATH(shaderStrAscii) (SHADER_FOLDER shaderStrAscii)

#define RESOURCE_FOLDER "./Resources/"
#define RESOURCE_PATH(resourceStrAscii) (RESOURCE_FOLDER resourceStrAscii)

#define UTF8_TEXT(str) (reinterpret_cast<const char*>(u8##str))

#define GET_PADDING_SIZE(align, type) ((align) - sizeof(type))

enum EConstant
{
	MAX_LABEL_LENGTH = 128,
	PATH_BUFFER_SIZE = MAX_PATH * 4
};