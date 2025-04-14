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
#ifndef _DEBUG

	//! 位置設定
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	//! 大きさ設定
	ImGui::SetNextWindowSize(ImVec2(320, 720), ImGuiCond_Always);

	/*! ImGui設定 */
#if 1
	//! 不透明度
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.5f, 0.0f, 0.0f, 0.8f));

	/*! 区切り線 */
	ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));         // 通常の色
	ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, ImVec4(1.0f, 0.7f, 0.3f, 1.0f));  // ホバー時の色
	ImGui::PushStyleColor(ImGuiCol_SeparatorActive, ImVec4(1.0f, 1.0f, 0.5f, 1.0f));   // クリック時の色

	/*! ヘッダー */
	// 背景色
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
	// ホバー時の色
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.6f, 0.8f, 1.0f));
	// クリック時の色
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.5f, 0.8f, 1.0f, 1.0f));

	/*! ボタン */
	// 背景色
	ImGui::PushStyleColor(ImGuiCol_Button,
		ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
	// ホバー時の色
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
		ImVec4(0.3f, 0.6f, 0.8f, 1.0f));
	// クリック時の色
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,
		ImVec4(0.9f, 0.7f, 0.2f, 1.0f));

	/*! フレーム */
	// 背景色
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
	// ホバー時の色
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,
		ImVec4(0.3f, 0.6f, 0.8f, 1.0f));
	// クリック時の色
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive,
		ImVec4(0.5f, 0.8f, 1.0f, 1.0f));

	/*! チェックマークの色 */
	ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	//! 縁の色
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
	//! 線の太さを2.0に設定
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);


	//! ボタンの角丸の度合い
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
	//! ボタンの内側の余白
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));

#endif

	if (ImGui::Begin("GameDebug"))
	{
		/*! カメラなどの処理 */
#if 1
		//! カメラコントローラー
		if (ImGui::CollapsingHeader("CameraController"))
#if 1
		{
			// デバッグカメラ
			ImGui::Checkbox(u8"デバッグカメラ", &CameraController::debugCamera);
			ImGui::Checkbox(u8"デバッグカメラの移動停止", &CameraController::noMoveDebugCamera);

			// ターゲット
			ImGui::DragFloat3(u8"ターゲット", &CameraController::target.x, 0.1f);

			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(CameraController::angle.x);
			a.y = DirectX::XMConvertToDegrees(CameraController::angle.y);
			a.z = DirectX::XMConvertToDegrees(CameraController::angle.z);
			ImGui::DragFloat3(u8"角度", &a.x, 0.1f);
			CameraController::angle.x = DirectX::XMConvertToRadians(a.x);
			CameraController::angle.y = DirectX::XMConvertToRadians(a.y);
			CameraController::angle.z = DirectX::XMConvertToRadians(a.z);

			// カメラの範囲
			ImGui::DragFloat(u8"範囲", &CameraController::range, 0.1f);
		}

#endif
		//! ライト
		if (ImGui::TreeNode(u8"ライトなど"))
#if 1
		{
			ImGui::Checkbox(u8"数値をデバッグで変更する", &Scene::debugControll);
			if (ImGui::CollapsingHeader(u8"ライト情報"))
			{
				ImGui::DragFloat4(u8"向き", &Scene::debugLightDirection.x, 0.01f);
				ImGui::ColorEdit4(u8"色(ライト)", &Scene::debugLightColor.x);
				ImGui::InputFloat3(u8"色の数値", &Scene::debugLightColor.x);
				ImGui::DragFloat3(u8"位置", &Scene::debugLightPosition.x, 0.1f);
				ImGui::DragFloat(u8"範囲", &Scene::debugLightRange, 0.1f);
			}
			if (ImGui::CollapsingHeader(u8"シャドウマップ"))
			{
				ImGui::DragFloat3(u8"位置のオフセット", &Scene::debugShadowMapEyeOffset.x, 0.1f);
				ImGui::DragFloat(u8"バイアス", &Scene::debugShadowMapBias, 0.001f);
				ImGui::DragFloat(u8"横の範囲", &Scene::debugShadowMapWidth, 0.01f);
				ImGui::DragFloat(u8"縦の範囲", &Scene::debugShadowMapLength, 0.01f);
				ImGui::DragFloat(u8"開始(影)", &Scene::debugShadowMapStart, 0.01f);
				ImGui::DragFloat(u8"終了(影)", &Scene::debugShadowMapEnd, 0.01f);
			}
			if (ImGui::CollapsingHeader(u8"光の影響"))
			{
				ImGui::DragFloat(u8"環境光", &Scene::debugAmbientStrength, 0.01f);
				ImGui::DragFloat(u8"拡散光", &Scene::debugDiffuseStrength, 0.01f);
				ImGui::DragFloat(u8"スペキュラー光", &Scene::debugSpecularStrength, 0.01f);
			}
			//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
			//-----------------------------------------------------------------------------------------------------//
			if (ImGui::CollapsingHeader(u8"フォグ"))
			{
				ImGui::ColorEdit3(u8"色(フォグ)", &Scene::debugFogColor.x);
				ImGui::DragFloat(u8"開始(フォグ)", &Scene::debugFogStart);
				ImGui::DragFloat(u8"終了(フォグ)", &Scene::debugFogEnd);
			}

			ImGui::TreePop();
		}
#endif
		//! ポストエフェクト
		if (ImGui::TreeNode(u8"ポストエフェクト"))
#if 1
		{
			//! 数値をリセットする
			ImGui::Checkbox(u8"数値をリセットする", &Camera::postEffectReset);

			if (ImGui::CollapsingHeader(u8"色関連"))
			{
				// コントラスト(数値が大きいほど、明るい部分はより明るく、暗い部分はより暗くなる)
				ImGui::DragFloat(u8"コントラスト", &Camera::postEffect.contrast, 0.01f);
				// サチュレーション(色の彩度)
				ImGui::DragFloat(u8"彩度", &Camera::postEffect.saturation, 0.01f);
				// カラーフィルター(色フィルター)
				ImGui::DragFloat3(u8"色フィルター", &Camera::postEffect.colorFilter.x, 0.01f);
				// クロマティックアベレーション(色収差(色ズレ))
				ImGui::DragFloat(u8"色ズレ", &Camera::postEffect.chromaticAberration, 0.01f);
				// 色相シフト
				ImGui::DragFloat(u8"色相シフト", &Camera::postEffect.hueShift, 0.01f, 0.0f, 1.0f);
			}
			//-------------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
			//-------------------------------------------------------------------------------------------------------//
			if (ImGui::CollapsingHeader(u8"ビネット"))
			{
				// ビネットの強度
				ImGui::DragFloat(u8"ビネットの強度", &Camera::postEffect.vignetteIntensity, 0.01f, 0.0f, 1.0f);
				// ビネットの不透明度
				ImGui::DragFloat(u8"ビネットの不透明度", &Camera::postEffect.vignetteOpacity, 0.01f, 0.0f, 1.0f);
			}
			//-------------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
			//-------------------------------------------------------------------------------------------------------//
			if (ImGui::CollapsingHeader(u8"ブラー関連"))
			{
				// ブラー強度
				ImGui::DragFloat(u8"ブラー強度", &Camera::postEffect.blurStrength, 0.01f);
				// ピントを合わせる距離
				ImGui::DragFloat(u8"ピントを合わせる距離", &Camera::postEffect.focusDistance, 0.1f);
				// ピントが合う範囲
				ImGui::DragFloat(u8"ピントが合う範囲", &Camera::postEffect.focusRange, 0.1f);
				// 被写界深度のブラー強度
				ImGui::DragFloat(u8"被写界深度のブラー強度", &Camera::postEffect.dofBlurStrength, 0.1f);
			}
			//-------------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
			//-------------------------------------------------------------------------------------------------------//
			if (ImGui::CollapsingHeader(u8"グレア"))
			{
				// グレアの強度
				ImGui::DragFloat(u8"グレアの閾値", &Camera::postEffect.bloomThreshold, 0.01f);
				// グレアの強度
				ImGui::DragFloat(u8"グレアの強度", &Camera::postEffect.bloomIntensity, 0.01f);
			}
			//-------------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
			//-------------------------------------------------------------------------------------------------------//
			if (ImGui::CollapsingHeader(u8"カメラシェイク"))
			{
				// シェイクのオフセット
				ImGui::DragFloat2(u8"シェイクのオフセット", &Camera::postEffect.shakeOffset.x, 0.01f);
				// シェイクの速さ
				ImGui::DragFloat(u8"シェイクの速さ", &Camera::postEffect.shakeSpeed, 0.1f);
				// シェイクの強度
				ImGui::DragFloat(u8"シェイクの強度", &Camera::postEffect.shakeStrength, 0.01f);
				// シェイクの時間
				ImGui::DragFloat(u8"シェイクの時間", &Camera::postEffect.shakeTime, 0.01f);
			}

			ImGui::TreePop();
		}
#endif

#endif
	}
	// 終了
	ImGui::End();

	//! 位置設定
	ImGui::SetNextWindowPos(ImVec2(1050, 0), ImGuiCond_Always);
	//! 大きさ設定
	ImGui::SetNextWindowSize(ImVec2(230, 500), ImGuiCond_Always);

	if (ImGui::Begin("Scene"))
	{
		ChangeSceneButtonGUI<SceneTitle>(u8"00.タイトル画面");
		ChangeSceneButtonGUI<SceneGameSelect>(u8"01.ゲーム画面");

		if (ImGui::TreeNode(u8"ゲームシーン"))
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
	ImGui::PopStyleVar(3);    // PushStyleVarで指定した数だけPopする
	ImGui::PopStyleColor(15); // PushStyleColorで指定した数だけPopする

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
		outs << "大阪平野ランド " "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
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
