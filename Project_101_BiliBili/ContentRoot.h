#pragma once
#include "PathUtils.h"

struct ContentRoot
{
    fs::path exeDir;
    fs::path cwd;
    fs::path root; // assetの親（root/asset/...）

    // root候補を順に試す（事故りやすいズレを吸収）
    bool Initialize()
    {
        exeDir = GetExeDir();
        cwd = GetCurrentDir();

        DebugLogA("[Path] CWD: %ws\n", cwd.c_str());
        DebugLogA("[Path] ExeDir: %ws\n", exeDir.c_str());

        // ① exeDir/asset があるならそれを採用（配布で強い）
        if (fs::exists(exeDir / L"asset"))
        {
            root = exeDir;
            DebugLogA("[Path] ContentRoot = ExeDir\n");
            return true;
        }

        // ② CWD/asset があるならそれ（VSのWorkingDirに依存する場合）
        if (fs::exists(cwd / L"asset"))
        {
            root = cwd;
            DebugLogA("[Path] ContentRoot = CWD\n");
            return true;
        }

        // ③ どうしても見つからない：ここで即停止（後段クラッシュ防止）
        DebugLogA("[Path][FATAL] asset folder not found.\n");
        DebugLogA("[Path][FATAL] Expected: %ws OR %ws\n",
            (exeDir / L"asset").c_str(),
            (cwd / L"asset").c_str());
        return false;
    }

    fs::path ResolveAsset(const fs::path& relativeUnderAsset) const
    {
        // 例: ResolveAsset(L"texture/player.png") -> root/asset/texture/player.png
        return root / L"asset" / relativeUnderAsset;
    }

    static ContentRoot* GetInstance(){
        static ContentRoot instance;
        return &instance;
	}

private:
	ContentRoot() = default;
};