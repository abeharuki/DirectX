#pragma once
#include <xaudio2.h>
#pragma comment (lib,"xaudio2.lib")
#include <fstream>
#include <wrl.h>
#include <array>
#include <set>

class Audio {
public:
	// 音声データの最大数
	static const int kMaxSoundData = 256;

	
	/// チャンクヘッダ
	struct ChunkHeader {
		char id[4];   // チャンク毎のID
		int32_t size; // チャンクサイズ
	};

	
	/// RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk; //"RIFF"
		char type[4];      //"WAVE"
	};


	/// FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk; //"fmt"
		WAVEFORMATEX fmt;  // 波形フォーマット
	};

	/// 音声データ
	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		BYTE* pBuffer;
		// バッファのサイズ
		unsigned int bufferSize;
		// 名前
		std::string name;
		// オーディオハンドル
		uint32_t audioHandle;
	};

	
	/// ボイスデータ
	struct Voice {
		uint32_t handle = 0;
		IXAudio2SourceVoice* sourceVoice = nullptr;
	};

	
	/// シングルトンインスタンスの取得
	static Audio* GetInstance();

	
	/// 解放処理
	void Finalize();

	
	/// 初期化
	void Initialize();

	
	/// 音声データの読み込み
	uint32_t SoundLoadWave(const char* filename);


	/// 音声データ開放
	void SoundUnload(SoundData* soundData);


	/// 音声再生
	void SoundPlayWave(uint32_t audioHandle, bool roopFlag, float volume);

	
	/// 音声停止
	void StopAudio(uint32_t audioHandle);

private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	const Audio& operator=(const Audio&) = delete;

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;
	std::array<SoundData, kMaxSoundData> soundDatas_{};
	std::set<Voice*> sourceVoices_{};
	uint32_t audioHandle_ = -1;
	uint32_t voiceHandle_ = -1;
};
