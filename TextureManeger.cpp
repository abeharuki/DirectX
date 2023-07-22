#include "TextureManeger.h"
#include <cassert>
#include <winerror.h>
#include <combaseapi.h>
#include <dxcapi.h>
#include <format>
#include <cstdlib>
#include <d3d12.h>

#pragma comment(lib, "dxcompiler.lib")




TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize() {

}



// dxcCompilerを初期化
void TextureManager::InitializeCompiler() {
	HRESULT hr_ = S_FALSE;

	


}

void TextureManager::ResetAll() {
	

}