#pragma once
#include "StringUtility.h"
#include "ResourceObject.h"
#include <wrl.h>

class TextureManager {
public:
	
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static TextureManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// コンパイラー初期化
	/// </summary>
	void InitializeCompiler();

	void ResetAll();


	

private:
	
	Utility* utility_;
};