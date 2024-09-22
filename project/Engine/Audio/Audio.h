#pragma once
#include <xaudio2.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include <fstream>
#include <wrl.h>
#include <array>
#include <set>
#include <vector>
#include <filesystem>

#pragma comment (lib,"Mf.lib")
#pragma comment (lib,"mfplat.lib")
#pragma comment (lib,"Mfreadwrite.lib")
#pragma comment (lib,"mfuuid.lib")
#pragma comment (lib,"xaudio2.lib")

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
	struct SoundDataWav {
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファの先頭アドレス
		BYTE* pBuffer;
		//バッファのサイズ
		unsigned int bufferSize;
		//名前
		std::string name;
		//オーディオハンドル
		uint32_t audioHandle;
	};
	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		std::vector<BYTE> pBuffer;
		// バッファのサイズ
		size_t bufferSize;
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
	uint32_t SoundLoadMP3(const std::filesystem::path& filename);

	/// 音声データ開放
	void SoundUnload(SoundData* soundData);
	void SoundUnload(SoundDataWav* soundData);


	/// 音声再生
	void SoundPlayWave(uint32_t audioHandle, bool roopFlag, float volume);
	void SoundPlayMP3(uint32_t audioHandle, bool roopFlag, float volume);

	/// 音声停止
	void StopAudio(uint32_t audioHandle);

private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	const Audio& operator=(const Audio&) = delete;


	std::wstring ConvertString(const std::string& str)
	{
		if (str.empty())
		{
			return std::wstring();
		}

		auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
		if (sizeNeeded == 0)
		{
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
		return result;
	}

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;
	std::array<SoundData, kMaxSoundData> soundDatas_{};
	std::array<SoundDataWav, kMaxSoundData> soundDatasWav_{};
	std::set<Voice*> sourceVoices_{};
	//IXAudio2SourceVoice* sourceVoice_ = nullptr;
	uint32_t audioHandle_ = -1;
	uint32_t voiceHandle_ = -1;
};
struct AudioHelper {
	AudioHelper() = default;
	AudioHelper(const AudioHelper&) = default;
	AudioHelper(const std::string& filePath) {
		Audio* const audio = Audio::GetInstance();
		isWav_ = filePath.ends_with(".wav");

		// ハンドルにデータを格納する
		handle_ = not isWav_ ? audio->SoundLoadMP3(filePath) : audio->SoundLoadWave(filePath.c_str());
	}

	void Play(bool roopFlag, float volume) const;
	void Stop() const;

private:

	uint32_t handle_ = (std::numeric_limits<uint32_t>::max)();
	bool isWav_;
};