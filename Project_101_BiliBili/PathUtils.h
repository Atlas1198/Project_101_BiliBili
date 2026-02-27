#pragma once
#include <windows.h>
#include <filesystem>
#include <string>
#include <cstdio>

namespace fs = std::filesystem;

inline void DebugLogA(const char* fmt, ...)
{
    char buf[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
    va_end(args);
    OutputDebugStringA(buf);
}

inline fs::path GetExeDir()
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    fs::path p(path);
    return p.parent_path(); // exe‚Ì‚ ‚éƒtƒHƒ‹ƒ_
}

inline fs::path GetCurrentDir()
{
    wchar_t cwd[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, cwd);
    return fs::path(cwd);
}