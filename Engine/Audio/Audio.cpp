#include "Audio.h"
#include <cassert>


Audio* Audio::GetInstance() {
	static Audio instance;
	return &instance;
}

//初期化
void Audio::Initialize() {
	// XAudioエンジンのインスタンスを生成
	HRESULT result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

//再生
void Audio::SoundPlayWave(uint32_t audioHandle, bool roopFlag, float volume) {

	// voiceHandleをインクリメント
	voiceHandle_++;

	HRESULT result;
	// 波形フォーマットを元にSourceVoiceの作成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundDatas_[audioHandle].wfex);
	assert(SUCCEEDED(result));

	// コンテナに追加
	Voice* voice = new Voice();
	voice->handle = voiceHandle_;
	voice->sourceVoice = pSourceVoice;
	sourceVoices_.insert(voice);

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundDatas_[audioHandle].pBuffer;
	buf.AudioBytes = soundDatas_[audioHandle].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (roopFlag) {
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	pSourceVoice->SetVolume(volume);
	result = pSourceVoice->Start();



}

//解放
void Audio::SoundUnload(SoundData* soundData) {
	// バッファのメモリを解放
	delete[] soundData->pBuffer;
	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

//読み込み
uint32_t Audio::SoundLoadWave(const char* filename) {
	// 同じ音声データがないか探す
	for (SoundData& soundData : soundDatas_) {
		if (soundData.name == filename) {
			return soundData.audioHandle;
		}
	}

	audioHandle_++;
	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());

	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	// LISTチャンクを検出した場合
	if (strncmp(data.id, "LIST", 4) == 0) {
		// 読み取り位置をLISTチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Waveファイルを閉じる
	file.close();

	// returnする為の音声データ
	soundDatas_[audioHandle_].wfex = format.fmt;
	soundDatas_[audioHandle_].pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundDatas_[audioHandle_].bufferSize = data.size;
	soundDatas_[audioHandle_].name = filename;
	soundDatas_[audioHandle_].audioHandle = audioHandle_;

	return audioHandle_;
}

//ストップ
void Audio::StopAudio(uint32_t audioHandle) {
	HRESULT result;
	for (const Voice* voice : sourceVoices_) {
		if (voice->handle == audioHandle) {
			result = voice->sourceVoice->Stop();
		}
	}
}

void Audio::Finalize() {
	// ボイスデータ開放
	for (const Voice* voice : sourceVoices_) {
		if (voice->sourceVoice != nullptr) {
			voice->sourceVoice->DestroyVoice();
		}
		delete voice;
	}

	// XAudio2解放
	xAudio2_.Reset();
	// 音声データ開放
	for (int i = 0; i < soundDatas_.size(); i++) {
		SoundUnload(&soundDatas_[i]);
	}
}