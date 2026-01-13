#include "AudioManager.h"
#include <fstream>

AudioManager::AudioManager() 
{

}

AudioManager::~AudioManager()
{

}



void AudioManager::Initialize()
{
	AudioInstance I;

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{

	}

	hr = XAudio2Create(&m_pXaudio2);
	if (FAILED(hr))
	{

	}

	hr = m_pXaudio2->CreateMasteringVoice(&m_pMasterVoice);
	if (FAILED(hr))
	{

	}

	WaveData data;
	if (LoadWave(L"asset\\audio-SE\\Bullet", data)) m_waveDataList.push_back(data);


}

void AudioManager::Update()
{




}

void AudioManager::Finalize()
{
	for(auto& instance : m_instances)
	{
		instance.m_pSourceVoice->DestroyVoice();
		instance.m_pSourceVoice = nullptr;
	}

	if (m_pMasterVoice)
	{
		m_pMasterVoice->DestroyVoice();
		m_pMasterVoice = nullptr;
	}

	if (m_pXaudio2)
	{
		m_pXaudio2->Release();
		m_pXaudio2 = nullptr;
	}

	CoUninitialize();
}

void AudioManager::LoadSE()
{
}

void AudioManager::LoadBGM()
{
}
void AudioManager::PlaySE()
{
}
void AudioManager::PlayBGM()
{
}




bool AudioManager::LoadWave(const wchar_t* path, WaveData& out)
{
	std::ifstream file(path, std::ios::binary);
	if (!file) return false;

	// RIFF
	char riff[4];
	file.read(riff, 4);
	file.ignore(4); // chunk size
	char wave[4];
	file.read(wave, 4);

	// fmt チャンク
	char fmt[4];
	file.read(fmt, 4);

	DWORD fmtSize;
	file.read(reinterpret_cast<char*>(&fmtSize), 4);

	file.read(reinterpret_cast<char*>(&out.format), fmtSize);

	// data チャンクまでスキップ
	char data[4];
	DWORD dataSize = 0;

	while (true)
	{
		file.read(data, 4);
		file.read(reinterpret_cast<char*>(&dataSize), 4);

		if (strncmp(data, "data", 4) == 0)
			break;

		file.ignore(dataSize);
	}

	out.buffer.resize(dataSize);
	file.read(reinterpret_cast<char*>(out.buffer.data()), dataSize);

	return true;
}