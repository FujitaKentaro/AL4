#include "GameScene.h"
#include "Model.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include"Collision.h"
#include<sstream>
#include<iomanip>

using namespace DirectX;

GameScene::GameScene(){
}

GameScene::~GameScene()
{
	safe_delete(spriteBG);
	safe_delete(objSkydome);
	safe_delete(objGround);
	safe_delete(objFighter);
	safe_delete(modelSkydome);
	safe_delete(modelGround);
	safe_delete(modelFighter);
	safe_delete(camera);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);

	this->dxCommon = dxCommon;
	this->input = input;

	// デバッグテキスト用テクスチャ読み込み
	Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png");
	// デバッグテキスト初期化
	debugText = DebugText::GetInstance();
	debugText->Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	Sprite::LoadTexture(1, L"Resources/background.png");

	// カメラ生成
	camera = new DebugCamera(WinApp::window_width, WinApp::window_height, input);

	// カメラ注視点をセット
	camera->SetTarget({ 0, 1, 0 });
	camera->SetDistance(3.0f);

	// 3Dオブジェクトにカメラをセット
	Object3d::SetCamera(camera);

	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });
	// 3Dオブジェクト生成
	objSkydome = Object3d::Create();
	objGround = Object3d::Create();
	objFighter = Object3d::Create();

	// テクスチャ2番に読み込み
	Sprite::LoadTexture(2, L"Resources/texture.png");

	modelSkydome = Model::CreateFromOBJ("skydome");
	modelGround = Model::CreateFromOBJ("Ground");
	modelFighter = Model::CreateFromOBJ("chr_sword");

	objSkydome->SetModel(modelSkydome);
	objGround->SetModel(modelGround);
	objFighter->SetModel(modelFighter);

	//球の初期値を設定
	sphere.center = XMVectorSet(0, 2, 0, 1);//中心座標
	sphere.radius = 1.0f;//半径

	//球の初期値を設定
	plane.normal = XMVectorSet(0, 1, 0, 0);//法線ベクトル
	plane.distance = 0.0f;//原点(0.0.0)からの距離


	//三角形の初期化を設定
	triangle.p0 = XMVectorSet(-1.0f, 0, -1.0f, 1);//左手前
	triangle.p1 = XMVectorSet(-1.0f, 0, +1.0f, 1);//右奥
	triangle.p2 = XMVectorSet(+1.0f, 0, -1.0f, 1);//右手前
	triangle.normal = XMVectorSet(0.0f, 1.0f, 0.0f, 1);//上向き
}

void GameScene::Update()
{
	camera->Update();

	objSkydome->Update();
	objGround->Update();
	objFighter->Update();

	//球移動
	{
		XMVECTOR moveY = XMVectorSet(0, 0.01f, 0, 0);
		if (input->PushKey(DIK_NUMPAD8)) { sphere.center += moveY; }
		else if (input->PushKey(DIK_NUMPAD2)) { sphere.center -= moveY; }

		XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
		if (input->PushKey(DIK_NUMPAD6)) { sphere.center += moveX; }
		else if (input->PushKey(DIK_NUMPAD4)) { sphere.center -= moveX; }
	}

	////球と平面の当たり判定
	//bool hit = Collision::CheckSphere2Plane(sphere, plane);
	//if (hit) {
	//	//stringstreamで変数の値を埋め込んで整形する
	//	std::ostringstream spherestr;
	//	spherestr << "Sphere:("
	//		<< std::fixed << std::setprecision(2)
	//		<< sphere.center.m128_f32[0] << ","
	//		<< sphere.center.m128_f32[1] << ","
	//		<< sphere.center.m128_f32[2] << ")";

	//	debugText.Print("HIT", 50, 200, 1.0f);

	//	debugText.Print(spherestr.str(), 50, 180, 1.0f);
	//}

	//球と三角形の当たり判定
	XMVECTOR inter;
	bool hit = Collision::CheckSphere2Triangle(sphere, triangle, &inter);
	if (hit)
	{
		debugText->Print("HIT", 50, 200, 1.0f);
		//stringstreamをリセットし、交点座標を埋め込む
		std::ostringstream spherestr;
		spherestr.str("");
		spherestr.clear();
		spherestr << "("
			<< std::fixed << std::setprecision(2)
			<< inter.m128_f32[0] << ","
			<< inter.m128_f32[1] << ","
			<< inter.m128_f32[2] << ")";

		debugText->Print(spherestr.str(), 50, 220, 1.0f);
	}

	debugText->Print("AD: move camera LeftRight", 50, 50, 1.0f);
	debugText->Print("WS: move camera UpDown", 50, 70, 1.0f);
	debugText->Print("ARROW: move camera FrontBack", 50, 90, 1.0f);
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	//spriteBG->Draw();

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(cmdList);

	// 3Dオブクジェクトの描画
	objSkydome->Draw();
	objGround->Draw();
	objFighter->Draw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	//// 描画
	//sprite1->Draw();
	//sprite2->Draw();

	// デバッグテキストの描画
	debugText->DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}