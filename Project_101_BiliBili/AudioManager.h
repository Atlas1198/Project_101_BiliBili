#pragma once
#include <xaudio2.h>
#include <wrl/client.h> 
#include <map>
#include <string>
#include <vector>

// 音声データを保持する構造体
struct SoundData {
    WAVEFORMATEX wfx;
	std::vector<BYTE> buffer;
};


class AudioManager {
public:
	AudioManager();	//コンストラクタ
	~AudioManager();	//デストラクタ

    bool Initialize();  //エンジン起動とスピーカーの準備
    void Update();  //　再生終了したボイスを解放
    bool LoadWav(const std::string& label, const wchar_t* filename);//wavファイル読み込み（名前を付け保存）


    // 再生
    void PlayBGM(const std::string& label, bool loop = true);   //BGM
    void PlaySE(const std::string& label);   //SE
    //void Play(const std::string& label, bool loop = false);

    void StopBGM();     //BGM停止
    void StopAllSE();   //全SE停止
    void StopAll();     //全ての音の停止
    void ResumeBGM();//BGM一時停止
    void ResumeSE(); //SE一時停止
    void ResumeAll();   //全音一時停止
    void PauseBGM();    //再開
    void PauseSE();    //再開
    void PauseAll();    //再開

    void SetBGMVolume(float volume) { bgmVolume = volume; } //音量調整・再生中のBGMにも反映

    static AudioManager* GetInstance()
    {
        static AudioManager instance;
        return &instance;
    }

private:
    Microsoft::WRL::ComPtr<IXAudio2> pXAudio2;      //Xaudio2エンジン本体
    IXAudio2MasteringVoice* pMasterVoice = nullptr; //スピーカー

    //音量調整
    float bgmVolume = 1.0f; // 0.0 ~ 1.0
    float seVolume = 1.0f;

    // 音声データの保管場
    std::map<std::string, SoundData> soundLibrary;
    // 再生中のBGM管理
    std::map<std::string, IXAudio2SourceVoice*> BGMVoices;
    // 再生中のSE管理
    std::multimap<std::string, IXAudio2SourceVoice*> SEVoices;

	//std::vector<WaveData> m_waveDataList;

    // 内部的なボイス作成用ヘルパー
    IXAudio2SourceVoice* CreateVoice(const std::string& label);
};

