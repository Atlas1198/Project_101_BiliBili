#include "AudioManager.h"
#include <fstream>
#include <algorithm>

AudioManager::AudioManager() {}	//コンストラクタ

AudioManager::~AudioManager()	//デストラクタ
{
    StopAll();
    if (pMasterVoice) pMasterVoice->DestroyVoice();
    CoUninitialize();
}

//初期化処理
bool AudioManager::Initialize() {
    // COMの初期化
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) return false;
		
    // XAudio2エンジンのインスタンス作成
    hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) return false;

    // マスターボイス（最終出力先）の作成
    hr = pXAudio2->CreateMasteringVoice(&pMasterVoice);
    if (FAILED(hr)) return false;

    return true;
	}

void AudioManager::Update() {
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

	}

    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    // RIFFヘッダーの読み込み（簡易チェック）
    char chunkId[4];
    file.read(chunkId, 4); // "RIFF"
    file.seekg(4, std::ios::cur); // ファイルサイズ飛ばし
    file.read(chunkId, 4); // "WAVE"

    SoundData data;
    while (file.read(chunkId, 4))
	{
        unsigned int chunkSize;
        file.read((char*)&chunkSize, 4);

        if (strncmp(chunkId, "fmt ", 4) == 0)
        {//フォーマットの読み込み
            file.read((char*)&data.wfx, chunkSize);
	}
        else if (strncmp(chunkId, "data", 4) == 0)
        {//音声波形データ本体のも見込み
            data.buffer.resize(chunkSize);
            file.read((char*)data.buffer.data(), chunkSize);
        }
        else
        {//不要なチャンクは読み飛ばす
            file.seekg(chunkSize, std::ios::cur);
        }
    }

    soundLibrary[label] = data; //ライブラリに登録
    return true;
}

// 共通のボイス作成処理
IXAudio2SourceVoice* AudioManager::CreateVoice(const std::string& label) {
    if (soundLibrary.find(label) == soundLibrary.end()) return nullptr;

    IXAudio2SourceVoice* pVoice = nullptr;
    HRESULT hr = pXAudio2->CreateSourceVoice(&pVoice, &soundLibrary[label].wfx);
    if (FAILED(hr)) return nullptr;

    XAUDIO2_BUFFER buffer = { 0 };
    buffer.pAudioData = soundLibrary[label].buffer.data();
    buffer.AudioBytes = (UINT32)soundLibrary[label].buffer.size();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    return pVoice;
}

void AudioManager::Update()
{

//BGM再生
void AudioManager::PlayBGM(const std::string& label, bool loop) {

    IXAudio2SourceVoice* pVoice = CreateVoice(label);
    if (!pVoice) return;

    XAUDIO2_BUFFER buffer = { 0 };
    buffer.pAudioData = soundLibrary[label].buffer.data();
    buffer.AudioBytes = (UINT32)soundLibrary[label].buffer.size();
    buffer.Flags = XAUDIO2_END_OF_STREAM; // データの終端
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

    pVoice->SubmitSourceBuffer(&buffer);
    pVoice->SetVolume(bgmVolume);   //現在の設定値適応
    pVoice->Start();    //再生開始
    BGMVoices[label] = pVoice;  //BGMリストに登録
}


void AudioManager::PlaySE(const std::string& label)
	{
    IXAudio2SourceVoice* pVoice = CreateVoice(label);
    if (!pVoice) return;

    XAUDIO2_BUFFER buffer = { 0 };
    buffer.pAudioData = soundLibrary[label].buffer.data();
    buffer.AudioBytes = (UINT32)soundLibrary[label].buffer.size();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    pVoice->SubmitSourceBuffer(&buffer);
    pVoice->Start();

    // SEは多重再生したいので multimap に追加（既存の音は消さない）
    SEVoices.insert(std::make_pair(label, pVoice));
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
}

//BGM一時停止
void AudioManager::PauseBGM()
{// 全てのBGMを一時停止（内部カウントを保持したまま停止）
    for (auto& pair : BGMVoices) pair.second->Stop(0);
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

//BGM再開
void AudioManager::ResumeBGM()
{// 一時停止していた箇所からBGM再開
    for (auto& pair : BGMVoices) pair.second->Start(0);
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