//#include <windows.h>
//#include <fstream>
//#include "Audio_SE.h"
//
//// グローバル（検証用なのでOK）
//static IXAudio2* g_xaudio2 = nullptr;
//static IXAudio2MasteringVoice* g_masterVoice = nullptr;
//static IXAudio2SourceVoice* g_sourceVoice = nullptr;
//
//// ------------------------------------------------------------
//// wav ロード（最小）
//// ------------------------------------------------------------
//static bool LoadWav(const wchar_t* path, WavData& out)
//{
//	std::ifstream file(path, std::ios::binary);
//	if (!file) return false;
//
//	// RIFF
//	char riff[4];
//	file.read(riff, 4);
//	file.ignore(4); // chunk size
//	char wave[4];
//	file.read(wave, 4);
//
//	// fmt チャンク
//	char fmt[4];
//	file.read(fmt, 4);
//
//	DWORD fmtSize;
//	file.read(reinterpret_cast<char*>(&fmtSize), 4);
//
//	file.read(reinterpret_cast<char*>(&out.format), fmtSize);
//
//	// data チャンクまでスキップ
//	char data[4];
//	DWORD dataSize = 0;
//
//	while (true)
//	{
//		file.read(data, 4);
//		file.read(reinterpret_cast<char*>(&dataSize), 4);
//
//		if (strncmp(data, "data", 4) == 0)
//			break;
//
//		file.ignore(dataSize);
//	}
//
//	out.buffer.resize(dataSize);
//	file.read(reinterpret_cast<char*>(out.buffer.data()), dataSize);
//
//	return true;
//}
//
//// ------------------------------------------------------------
//// 初期化
//// ------------------------------------------------------------
//bool InitAudio()
//{
//	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
//	if (FAILED(hr)) return false;
//
//	hr = XAudio2Create(&g_xaudio2);
//	if (FAILED(hr)) return false;
//
//	hr = g_xaudio2->CreateMasteringVoice(&g_masterVoice);
//	if (FAILED(hr)) return false;
//
//	return true;
//}
//
//// ------------------------------------------------------------
//// 終了
//// ------------------------------------------------------------
//void FinalizeAudio()
//{
//	if (g_sourceVoice)
//	{
//		g_sourceVoice->DestroyVoice();
//		g_sourceVoice = nullptr;
//	}
//
//	if (g_masterVoice)
//	{
//		g_masterVoice->DestroyVoice();
//		g_masterVoice = nullptr;
//	}
//
//	if (g_xaudio2)
//	{
//		g_xaudio2->Release();
//		g_xaudio2 = nullptr;
//	}
//
//	CoUninitialize();
//}
//
//// ------------------------------------------------------------
//// wav 再生（1音）
//// ------------------------------------------------------------
//bool PlayWav(const wchar_t* filePath)
//{
//	WavData wav{};
//
//	//読み取り失敗
//	if (!LoadWav(filePath, wav))
//	{
//		return false;
//	}
//
//	// 既存の voice を破棄（連打用）
//	if (g_sourceVoice)
//	{
//		g_sourceVoice->DestroyVoice();
//		g_sourceVoice = nullptr;
//	}
//
//	//ソースボイス作成
//	HRESULT hr = g_xaudio2->CreateSourceVoice(
//		&g_sourceVoice,
//		&wav.format
//	);
//	if (FAILED(hr)) return false;
//
//	XAUDIO2_BUFFER buf{};
//	buf.AudioBytes = static_cast<UINT32>(wav.buffer.size());
//	buf.pAudioData = wav.buffer.data();
//	buf.Flags = XAUDIO2_END_OF_STREAM;
//
//	g_sourceVoice->SubmitSourceBuffer(&buf);
//	g_sourceVoice->Start();
//
//	return true;
//}
