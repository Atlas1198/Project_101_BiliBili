#pragma once

#include <xaudio2.h>
#include <vector>
#include <string>

//同時SE最大数
static constexpr int MAX_SE = 32;
static constexpr int MAX_BGM = 5;

//再生中SE１つ文
struct AudioInstance
{
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;
	bool isActive = false;
};

// wav データ構造
struct WaveData
{
	WAVEFORMATEX format{};
	std::vector<BYTE> buffer;
};


//サウンド管理クラス
class AudioManager
{

public:
	AudioManager();	//コンストラクタ
	~AudioManager();	//デストラクタ

	void Initialize();
	void Update();
	void Finalize();


	void LoadSE();
	void LoadBGM();
	void PlaySE();
	void PlayBGM();

private:
	IXAudio2* m_pXaudio2 = nullptr;
	IXAudio2MasteringVoice* m_pMasterVoice = nullptr;

	AudioInstance m_instances[MAX_SE];

	std::vector<WaveData> m_waveDataList;

private:
	static bool LoadWave(const wchar_t* path, WaveData& out);
};