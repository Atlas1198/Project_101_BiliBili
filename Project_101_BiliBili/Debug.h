#pragma once
#include <windows.h>
#include <string>
#include <cstdarg>
#include <comdef.h>

// Out put debug string
static void DBG(const char* fmt, ...)
{
	char buf[4096];

	va_list args;
	va_start(args, fmt);
	vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
	va_end(args);

	OutputDebugStringA(buf);
}

static void DBG_HR(HRESULT hr) {
	_com_error err(hr);
	auto message = err.ErrorMessage();
	DBG(message);
}
