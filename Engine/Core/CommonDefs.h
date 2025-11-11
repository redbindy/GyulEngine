#pragma once

enum EConstant
{
	MAX_LABEL_LENGTH = 256,
};

#define UTF8_TEXT(str) (reinterpret_cast<const char*>(u8##str))