#include "AudioResources.h"
#include "AudioManager.h"
#include <vector>
#include <string>
#include <windows.h>

struct SoundResource {
    std::string label;
    const wchar_t* path;
};

void LoadAllGameSounds(AudioManager& am) {
    // リスト
    static const std::vector<SoundResource> catalog = {
        // --- BGMで使う用 ---
        //{ "", L""},{ "", L"asset/sound/SE/"},
        { "GAME_BGM",  L"asset/sound/BGM/インゲーム_メイン_BGM.wav" },
        { "GAME_TF_BGM", L"asset/sound/BGM/インゲーム_変身時_BGM.wav" },
        { "CHARA_BGM", L"asset/sound/BGM/キャラ選択_BGM.wav"},
        { "TITLE_BGM", L"asset/sound/BGM/TITLE_BGM.wav" },
        { "RESULT_BGM", L"asset/sound/BGM/リザルト_BGM.wav"},
        { "CON_BGM", L"asset/sound/BGM/controllersetting_BGM.wav"},

        //// --- SEで使う用 ---
        { "ANNOUNCE", L"asset/sound/SE/インゲーム_アナウンス.wav"},
        { "ANNOUNCE_ALL", L"asset/sound/SE/インゲーム_アナウンス_合体.wav"},
        { "GAME_COUNT1", L"asset/sound/SE/インゲーム_カウントダウン_1_SE.wav" },
        { "GAME_COUNT2", L"asset/sound/SE/インゲーム_カウントダウン_2_SE.wav" },
        { "GAME_STRAT", L"asset/sound/SE/インゲーム_開始2_SE.wav"},
        { "GAME_TF", L"asset/sound/SE/インゲーム_変身_SE.wav"},
        { "GAME_FINISH", L"asset/sound/SE/インゲーム_終了_SE.wav"},
        { "", L"asset/sound/SE/インゲーム_変身解除_SE.wav"},
        { "Shoot", L"asset/sound/SE/Shoot.wav" },
        { "TF_SHOOT", L"asset/sound/SE/インゲーム_ビリビリ.wav"},
        { "GAME_FINISH_SHOOT", L"asset/sound/SE/インゲーム_最終ダメージ_SE.wav"},
        { "HIT", L"asset/sound/SE/インゲーム_ダメージ_SE.wav"},
        { "BANE_JUMP", L"asset/sound/SE/インゲーム_ファン_SE.wav"},
        { "MOVE", L"asset/sound/SE/歩く・金属・走る.wav"},
        { "TITLE_NEXT", L"asset/sound/SE/タイトル_決定_SE.wav"},
        { "", L"asset/sound/SE/キャラ選択_カーソル移動wav"},
        { "CHARA_RESET", L"asset/sound/SE/キャラ選択_キャンセル.wav"},
        { "CURSOR_MOVE", L"asset/sound/SE/キャラ選択_カーソル移動.wav"},
        { "CHARA_SET", L"asset/sound/SE/キャラ選択_レディ.wav"},
        { "CHARA_NEXT", L"asset/sound/SE/キャラ選択_決定_SE.wav"},
        { "STAGE_SET", L"asset/sound/SE/ステージ選択_決定_SE.wav"},
        { "CON_NEXT", L"asset/sound/SE/接続かくにん_進行_SE.wav"},
        { "CON_SET", L"asset/sound/SE/接続かくにん_接続_SE.wav"},
        { "RESULT", L"asset/sound/SE/リザルト_発表_SE.wav"},
        { "RESULT_NEXT", L"asset/sound/SE/リザルト_決定長押し_SE.wav"},
        { "DECIDE_SE",   L"Resources/Audio/SE/decide.wav" }
    };

    for (const auto& res : catalog) {
        if (!am.LoadWav(res.label, res.path)) {
            OutputDebugStringW(L"[Audio] Failed to load: ");
            OutputDebugStringW(res.path);
            OutputDebugStringW(L"\n");
        }
    }
}   