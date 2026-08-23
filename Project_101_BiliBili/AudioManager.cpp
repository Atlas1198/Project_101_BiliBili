#include "AudioManager.h"
#include <fstream>
#include <algorithm>
#include <cstring>

AudioManager::AudioManager() {}	//コンストラクタ

AudioManager::~AudioManager()	//デストラクタ
{
	m_available = false;
    StopAll();
    if (pMasterVoice)
	{
		pMasterVoice->DestroyVoice();
		pMasterVoice = nullptr;
	}
	if (m_comInitialized)
	{
		CoUninitialize();
		m_comInitialized = false;
	}
}

//初期化処理
bool AudioManager::Initialize() {
	m_available = false;

    // COMの初期化
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) return false;
	m_comInitialized = true;
		
    // XAudio2エンジンのインスタンス作成
    hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) return false;

    // マスターボイス（最終出力先）の作成
    hr = pXAudio2->CreateMasteringVoice(&pMasterVoice);
    if (FAILED(hr)) return false;

	m_available = true;
    return true;
	}

void AudioManager::Update() {
	if (!m_available) return;

    // SEリストの巡回と自動削除
    for (auto it = SEVoices.begin(); it != SEVoices.end(); ) {
        XAUDIO2_VOICE_STATE state;
        it->second->GetState(&state);
        // BuffersQueuedが0なら再生が完了している
        if (state.BuffersQueued == 0) {
            it->second->DestroyVoice(); // XAudio2のリソースを解放
            it = SEVoices.erase(it);    // リストから削除してイテレータを進める
        }
        else {
            ++it;
        }
    }
    // BGMリストの巡回（ループなしBGMが終わった時用）
    for (auto it = BGMVoices.begin(); it != BGMVoices.end(); ) {
        XAUDIO2_VOICE_STATE state;
        it->second->GetState(&state);
        if (state.BuffersQueued == 0) {
            it->second->DestroyVoice();
            it = BGMVoices.erase(it);
        }
        else {
            ++it;
        }
    }
}


//音声データの読み込み
bool AudioManager::LoadWav(const std::string& label, const wchar_t* filename)
	{
    //すでに同じラベルで読み込み済みなら、成功として処理を抜ける
    if (soundLibrary.find(label) != soundLibrary.end()) { return true; }

    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

	file.seekg(0, std::ios::end);
	const std::streamoff fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	if (fileSize < 12) return false;

	char riffId[4];
	char waveId[4];
	uint32_t riffSize = 0;
	if (!file.read(riffId, sizeof(riffId)) ||
		!file.read(reinterpret_cast<char*>(&riffSize), sizeof(riffSize)) ||
		!file.read(waveId, sizeof(waveId)) ||
		std::memcmp(riffId, "RIFF", sizeof(riffId)) != 0 ||
		std::memcmp(waveId, "WAVE", sizeof(waveId)) != 0)
	{
		return false;
	}
	if (riffSize < 4 || static_cast<uint64_t>(riffSize) + 8u > static_cast<uint64_t>(fileSize)) return false;

	SoundData data{};
	char chunkId[4];
	bool foundFormat = false;
	bool foundData = false;
    while (file.read(chunkId, 4))
	{
		uint32_t chunkSize = 0;
		if (!file.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize))) return false;

		const std::streamoff chunkStart = file.tellg();
		if (chunkStart < 0 || static_cast<uint64_t>(chunkSize) > static_cast<uint64_t>(fileSize - chunkStart))
		{
			return false;
		}

        if (strncmp(chunkId, "fmt ", 4) == 0)
        {//フォーマットの読み込み
			if (chunkSize < 16) return false;
			data.format.resize(chunkSize);
			if (!file.read(reinterpret_cast<char*>(data.format.data()), chunkSize)) return false;
			foundFormat = true;
	}
        else if (strncmp(chunkId, "data", 4) == 0)
        {//音声波形データ本体のも見込み
			if (chunkSize == 0) return false;
            data.buffer.resize(chunkSize);
			if (!file.read(reinterpret_cast<char*>(data.buffer.data()), chunkSize)) return false;
			foundData = true;
        }
        else
        {//不要なチャンクは読み飛ばす
            file.seekg(chunkSize, std::ios::cur);
        }

		// RIFF chunks are padded to an even byte boundary.
		if ((chunkSize & 1u) != 0 && file.tellg() < fileSize)
		{
			file.seekg(1, std::ios::cur);
		}
    }

	if (!foundFormat || !foundData) return false;
	const auto* format = reinterpret_cast<const WAVEFORMATEX*>(data.format.data());
	if (format->nChannels == 0 || format->nSamplesPerSec == 0 || format->nBlockAlign == 0)
	{
		return false;
	}

	soundLibrary[label] = std::move(data); //ライブラリに登録
    return true;
}

// 共通のボイス作成処理
IXAudio2SourceVoice* AudioManager::CreateVoice(const std::string& label) {
	auto sound = soundLibrary.find(label);
	if (!m_available || !pXAudio2 || sound == soundLibrary.end() || sound->second.format.size() < 16 || sound->second.buffer.empty())
	{
		return nullptr;
	}

    IXAudio2SourceVoice* pVoice = nullptr;
	const auto* format = reinterpret_cast<const WAVEFORMATEX*>(sound->second.format.data());
	HRESULT hr = pXAudio2->CreateSourceVoice(&pVoice, format);
    if (FAILED(hr)) return nullptr;

    XAUDIO2_BUFFER buffer = { 0 };
    buffer.pAudioData = soundLibrary[label].buffer.data();
    buffer.AudioBytes = (UINT32)soundLibrary[label].buffer.size();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    return pVoice;
}


//BGM再生
void AudioManager::PlayBGM(const std::string& label,float volume, bool loop)
{
  
    if (BGMVoices.find(label) != BGMVoices.end())
    {// すでにリストにある（再生中、または一時停止中）なら
        XAUDIO2_VOICE_STATE state;
        BGMVoices[label]->GetState(&state);
        //一時停止中なら再生する
        if (BGMVoices.find(label) != BGMVoices.end()) {
            BGMVoices[label]->Start(0);
        }
        return; //すでに再生、流れているので終了
    }

    //新規作成
    IXAudio2SourceVoice* pVoice = CreateVoice(label);
    if (!pVoice) return;

    XAUDIO2_BUFFER buffer = { 0 };
    buffer.pAudioData = soundLibrary[label].buffer.data();
    buffer.AudioBytes = (UINT32)soundLibrary[label].buffer.size();
    buffer.Flags = XAUDIO2_END_OF_STREAM; // データの終端
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

    pVoice->SubmitSourceBuffer(&buffer);
    pVoice->SetVolume(bgmVolume * volume);   //現在の設定値適応
    pVoice->Start();    //再生開始
    BGMVoices[label] = pVoice;  //BGMリストに登録
}

void AudioManager::SinglePlaySE(const std::string& label, float volume,bool preventDuplicate)
{
    // 二重再生防止が有効な場合、SEVoicesの中に同じラベルがあるかチェック
    if (preventDuplicate) {
        if (SEVoices.find(label) != SEVoices.end()) {
            return; // すでに再生中なので何もしない
        }
    }

    IXAudio2SourceVoice* pVoice = CreateVoice(label);
    if (!pVoice) return;

    XAUDIO2_BUFFER buffer = { 0 };
    buffer.pAudioData = soundLibrary[label].buffer.data();
    buffer.AudioBytes = (UINT32)soundLibrary[label].buffer.size();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    pVoice->SubmitSourceBuffer(&buffer);
    pVoice->SetVolume(seVolume * volume);
    pVoice->Start();

    // SEは多重再生したいので multimap に追加（既存の音は消さない）
    SEVoices.insert(std::make_pair(label, pVoice));
}

void AudioManager::PlaySE(const std::string& label,float volume)
	{
    IXAudio2SourceVoice* pVoice = CreateVoice(label);
    if (!pVoice) return;

    XAUDIO2_BUFFER buffer = { 0 };
    buffer.pAudioData = soundLibrary[label].buffer.data();
    buffer.AudioBytes = (UINT32)soundLibrary[label].buffer.size();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    pVoice->SubmitSourceBuffer(&buffer);
    pVoice->SetVolume(seVolume * volume);
    pVoice->Start();

    // SEは多重再生したいので multimap に追加（既存の音は消さない）
    SEVoices.insert(std::make_pair(label, pVoice));
}

void AudioManager::PlayLoopSE(const std::string& label) {
    // すでに再生中なら何もしない
    if (LoopSEVoices.find(label) != LoopSEVoices.end()) return;

    IXAudio2SourceVoice* pVoice = CreateVoice(label);
    if (!pVoice) return;

    XAUDIO2_BUFFER buffer = { 0 };
    buffer.pAudioData = soundLibrary[label].buffer.data();
    buffer.AudioBytes = (UINT32)soundLibrary[label].buffer.size();
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.LoopCount = XAUDIO2_LOOP_INFINITE; // ループ設定

    pVoice->SubmitSourceBuffer(&buffer);
    pVoice->SetVolume(seVolume); // SE用ボリューム
    pVoice->Start();

    LoopSEVoices[label] = pVoice; // 管理リストに追加
}



//BGM停止
void AudioManager::StopBGM() {
    for (auto& pair : BGMVoices) {
        pair.second->Stop();
        pair.second->DestroyVoice();
}
    BGMVoices.clear();
}

//SE停止
void AudioManager::StopAllSE() {
    for (auto& pair : SEVoices) {
        pair.second->Stop();
        pair.second->DestroyVoice();
}
    SEVoices.clear();
}

//全音声停止
void AudioManager::StopAll()
{
    StopBGM();
    StopAllSE();
	for (auto& pair : LoopSEVoices)
	{
		pair.second->Stop();
		pair.second->DestroyVoice();
	}
	LoopSEVoices.clear();
}




//SE一時停止
void AudioManager::PauseSE()
{// 全てのSEを一時停止（内部カウントを保持したまま停止）
    for (auto& pair : SEVoices) pair.second->Stop(0);
}

//音一時停止
void AudioManager::PauseAll()
{// 全ての再生中の音を一時停止（内部カウントを保持したまま停止）
    for (auto& pair : BGMVoices) pair.second->Stop(0);
    for (auto& pair : SEVoices) pair.second->Stop(0);
}



//SE再開
void AudioManager::ResumeSE()
{// 一時停止していた箇所からSE再開
    for (auto& pair : SEVoices) pair.second->Start(0);
}

//音の再開
void AudioManager::ResumeAll()
{// 一時停止していた箇所から再開
    for (auto& pair : BGMVoices) pair.second->Start(0);
    for (auto& pair : SEVoices) pair.second->Start(0);
}


// 特定のBGMを一時停止
void AudioManager::PauseBGM(const std::string& label) {
    if (BGMVoices.count(label)) {
        // 第1引数に0を渡すと、現在の再生位置を保持したまま停止（一時停止）
        BGMVoices[label]->Stop(0);
    }
}

// 特定のBGMを再開
void AudioManager::ResumeBGM(const std::string& label) {
    if (BGMVoices.count(label)) {
        BGMVoices[label]->Start(0);
    }
}

// 特定のBGMを完全に停止して削除
void AudioManager::StopBGM(const std::string& label) {
    if (BGMVoices.count(label)) {
        BGMVoices[label]->Stop();
        BGMVoices[label]->DestroyVoice();
        BGMVoices.erase(label);
    }
}

// 特定のSEを一時停止
void AudioManager::StopLoopSE(const std::string& label) {
    if (LoopSEVoices.count(label)) {
        LoopSEVoices[label]->Stop();
        LoopSEVoices[label]->DestroyVoice();
        LoopSEVoices.erase(label);
    }
}
