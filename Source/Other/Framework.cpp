#include <memory>
#include <sstream>

#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Framework.h"
#include "Audio/BgmManager.h"
#include "Game/Effect/EffectManager.h"
#include "Game/Scene/SceneManager.h"
#include "Game/Scene/SceneTitle.h"
#include "Game/Scene/SceneGameSelect.h"
#include "Game/Scene/G0_Onigokko.h"
#include "Game/Scene/G1_DarumasangaKoronda.h"
#include "Game/Scene/G2_Sundome.h"
#include "Game/Scene/G3_SoratobuHusenWari.h"
#include "Game/Scene/G4_OssanTataki.h"
#include "Game/Scene/G5_Asibawatari.h"

//static SceneGameSelect sceneGame;

// 垂直同期間隔設定
static const int syncInterval = 1;

// コンストラクタ
Framework::Framework(HWND hWnd)
	: hWnd(hWnd)
	, input(hWnd)
	, graphics(hWnd)
{
	// エフェクトマネージャー初期化
	EffectManager::Instance().Initialize();

	// シーン初期化
	//sceneGame.Initialize();
	SceneManager::Instance().ChangeScene(std::make_unique<SceneTitle>());
}

// デストラクタ
Framework::~Framework()
{
	// シーン終了化
	//sceneGame.Finalize();
	SceneManager::Instance().Clear();

	// エフェクトマネージャー終了化
	EffectManager::Instance().Finalize();
}

// 更新処理
void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// 入力更新処理
	input.Update();

	// シーン更新処理
	//sceneGame.Update(elapsedTime);
	SceneManager::Instance().Update(elapsedTime);
}

// 描画処理
void Framework::Render(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// 別スレッド中にデバイスコンテキストが使われていた場合に
	// 同時アクセスしないように排他制御する
	std::lock_guard<std::mutex> lock(graphics.GetMutex());

	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// IMGUIフレーム開始処理
	graphics.GetImGuiRenderer()->NewFrame();

	// シーン描画処理
	//sceneGame.Render();
	SceneManager::Instance().Render();

	// IMGUIデモウインドウ描画（IMGUI機能テスト用）
	//ImGui::ShowDemoWindow();

	// シーン切り替えGUI
	SceneSelectGUI();

	// IMGUI描画
	graphics.GetImGuiRenderer()->Render(dc);

	// バックバッファに描画した画を画面に表示する。
	graphics.GetSwapChain()->Present(syncInterval, 0);
}

template<class T>
void Framework::ChangeSceneButtonGUI(const char* name)
{
	//TODO:クリックでシーンを切り替える
	if (ImGui::Button(name))
	{
		BgmManager::Instance().StopBgm();
		SceneManager::Instance().ChangeScene(std::make_unique<T>());
	}
}

// シーン切り替えGUI
void Framework::SceneSelectGUI()
{
#ifdef _DEBUG

	ImVec2 displaySize = ImGui::GetIO().DisplaySize;

	ImGui::SetNextWindowPos(ImVec2(1000, 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(250, 200), ImGuiCond_Once);

	if (ImGui::Begin("Scene"))
	{
		ChangeSceneButtonGUI<SceneTitle>(u8"00.タイトル画面");

		ChangeSceneButtonGUI<SceneGameSelect>(u8"01.ゲーム画面");

		if (ImGui::TreeNode(u8"ゲームシーン"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			ChangeSceneButtonGUI<G0_Onigokko>          (u8"00.おにごっこ");
			ChangeSceneButtonGUI<G1_DarumasangaKoronda>(u8"01.だるまさんが転んだ");
			ChangeSceneButtonGUI<G2_Sundome>           (u8"02.寸止め");
			ChangeSceneButtonGUI<G3_SoratobuHusenWari> (u8"03.空飛ぶ風船割り");
			ChangeSceneButtonGUI<G4_OssanTataki>       (u8"04.おにごっこ");
			ChangeSceneButtonGUI<G5_Asibawatari>       (u8"05.おにごっこ");

			ImGui::TreePop();
		}
	}
	ImGui::End();

#endif // !_DEBUG
}

// フレームレート計算
void Framework::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int frames = 0;
	static float time_tlapsed = 0.0f;

	frames++;

	// Compute averages over one second period.
	if ((timer.TimeStamp() - time_tlapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frames); // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::ostringstream outs;
		outs.precision(6);
		outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
		SetWindowTextA(hWnd, outs.str().c_str());

		// Reset for next average.
		frames = 0;
		time_tlapsed += 1.0f;
	}
}

// アプリケーションループ
int Framework::Run()
{
	MSG msg = {};

	HDC hDC = GetDC(hWnd);

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.Tick();
			CalculateFrameStats();

			// TODO: elapsedTime(キャラクターの移動速度に影響)
			float elapsedTime = syncInterval == 0
				? timer.TimeInterval()
				: syncInterval / static_cast<float>(GetDeviceCaps(hDC, VREFRESH))
				;
			Update(elapsedTime);
			Render(elapsedTime);
		}
	}
	
	ReleaseDC(hWnd, hDC);

	return static_cast<int>(msg.wParam);
}

// メッセージハンドラ
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Graphics::Instance().GetImGuiRenderer()->HandleMessage(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.Stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.Start();
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
