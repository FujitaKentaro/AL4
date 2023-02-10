#include "GameScene.h"
#include "Model.h"
#include <cassert>
#include "Collision.h"
#include <sstream>
#include <iomanip>


using namespace DirectX;

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete spriteBG;
	delete objSkydome;
	delete objGround;
	delete objFighter;
	delete objSphere;
	delete objSphere1;
	delete modelSkydome;
	delete modelGround;
	delete modelFighter;
	delete modelSphere;
	delete modelSphere1;

	delete objRay;
	delete objRay1;
	delete modelRay;
	delete modelRay1;

	delete camera;
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);

	this->dxCommon = dxCommon;
	this->input = input;

	// デバッグテキスト用テクスチャ読み込み
	Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png");
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	Sprite::LoadTexture(1, L"Resources/background.png");

	// カメラ生成
	camera = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight, input);

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
	objSphere = Object3d::Create();
	objSphere1 = Object3d::Create();
	objRay = Object3d::Create();
	objRay1 = Object3d::Create();

	// テクスチャ2番に読み込み
	Sprite::LoadTexture(2, L"Resources/texture.png");

	modelSkydome = Model::CreateFromOBJ("skydome");
	modelGround = Model::CreateFromOBJ("ground");
	modelFighter = Model::CreateFromOBJ("chr_sword");
	modelSphere = Model::CreateFromOBJ("bullet");
	modelSphere1 = Model::CreateFromOBJ("bullet1");
	modelRay = Model::CreateFromOBJ("ray");
	modelRay1 = Model::CreateFromOBJ("ray1");

	objSkydome->SetModel(modelSkydome);
	objGround->SetModel(modelGround);
	objFighter->SetModel(modelFighter);
	objSphere->SetModel(modelSphere);
	objSphere1->SetModel(modelSphere1);
	objRay->SetModel(modelRay);

	//球の初期値を設定
	sphere.center = XMVectorSet(0, 2, 0, 1); //中心点座標
	sphere.radius = 0.2f;//半径

	//平面の初期値を設定
	plane.normal = XMVectorSet(0, 1, 0, 0); //法線ベクトル
	plane.distance = 0.0;

	//三角形の初期値を設定
	triangle.p0 = XMVectorSet(-1.0f, 0, -1.0f, 1); //左手前
	triangle.p1 = XMVectorSet(-1.0f, 0, +1.0f, 1); //左奥
	triangle.p2 = XMVectorSet(+1.0f, 0, -1.0f, 1); //右手前
	triangle.normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0); //上向き

	//レイの初期値を設定
	ray.start = XMVectorSet(0, 20, 0, 1); //原点やや上
	ray.dir = XMVectorSet(0, -1, 0, 0);

	scene = 0;
}

void GameScene::Update()
{
	camera->Update();
	switch (scene)
	{
	case 0:
		if (input->TriggerKey(DIK_SPACE)) { scene = 1; }
		break;
	case 1:
	{
		if (input->TriggerKey(DIK_SPACE)) { scene = 0; }

		//球移動
		{
			XMVECTOR moveY = XMVectorSet(0, 0.01f, 0, 0);
			if (input->PushKey(DIK_UP)) { sphere.center += moveY; }
			else if (input->PushKey(DIK_DOWN)) { sphere.center -= moveY; }
			XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
			if (input->PushKey(DIK_LEFT)) { sphere.center -= moveX; }
			else if (input->PushKey(DIK_RIGHT)) { sphere.center += moveX; }
		}
		std::ostringstream spherestr;
		spherestr << "Sphere:("
			<< std::fixed << std::setprecision(2) //小数点以下2桁まで
			<< sphere.center.m128_f32[0] << ","   //x
			<< sphere.center.m128_f32[1] << ","   //y
			<< sphere.center.m128_f32[2] << ")";   //z
		debugText.Print(spherestr.str(), 50, 180, 1.0f);

		////レイ操作
		//{
		//	XMVECTOR moveZ = XMVectorSet(0, 0, 0.01f, 0);
		//	if (input->PushKey(DIK_W)) { ray.start += moveZ; }
		//	else if (input->PushKey(DIK_S)) { ray.start -= moveZ; }
		//	XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
		//	if (input->PushKey(DIK_A)) { ray.start += moveX; }
		//	else if (input->PushKey(DIK_D)) { ray.start -= moveX; }
		//}

		std::ostringstream raystr;
		raystr << "Sphere:("
			<< std::fixed << std::setprecision(2) //小数点以下2桁まで
			<< ray.start.m128_f32[0] << ","   //x
			<< ray.start.m128_f32[1] << ","   //y
			<< ray.start.m128_f32[2] << ")";   //z
		debugText.Print(raystr.str(), 50, 200, 1.0f);
	}
	{
		//球と平面の当たり判定
		XMVECTOR inter;
		bool hit = Collision::CheckSphere2Plane(sphere, plane, &inter);
		if (hit) {
			objSphere->SetModel(modelSphere1);
		}
		else {
			objSphere->SetModel(modelSphere);
		}
		//レイと球の当たり判定
		float distance;
		bool hit2 = Collision::CheckRay2Sphere(ray, sphere, &distance, &inter);
		if (hit2) {
			objSphere->SetModel(modelSphere1);
			objRay->SetModel(modelRay1);
		}
		else {
			objRay->SetModel(modelRay);
		}
		////レイと三角形の当たり判定
		//float distance;
		//bool hit2 = Collision::CheckRay2Triangle(ray, triangle, &distance, &inter);
		//if (hit2) {
		//
		//}
		//球と平面の当たり判定		
		//bool hit3 = Collision::CheckSphere2Triangle(sphere, triangle, &inter);
		//if (hit3) {
		//	std::ostringstream raystr;
		//	raystr << "球と平面の当たり判定";
		//	debugText.Print(raystr.str(), 50, 220, 1.0f);
		//}
		//レイと平面の当たり判定
		float distance4;
		bool hit4 = Collision::CheckRay2Plane(ray, plane, &distance4, &inter);
		if (hit4) {
			std::ostringstream raystr;
			raystr << " Ray+Plane = HIT";
			debugText.Print(raystr.str(), 50, 240, 1.0f);
		}


	}

	objSphere->SetPosition(XMFLOAT3(
		sphere.center.m128_f32[0],
		sphere.center.m128_f32[1],
		sphere.center.m128_f32[2]));
	objRay->SetPosition(XMFLOAT3(
		ray.start.m128_f32[0],
		ray.start.m128_f32[1],
		ray.start.m128_f32[2]));


	objSkydome->Update();
	objGround->Update();
	//objFighter->Update();
	objSphere->Update();
	objRay->Update();

	debugText.Print("AD: move camera LeftRight", 50, 50, 1.0f);
	debugText.Print("WS: move camera UpDown", 50, 70, 1.0f);
	debugText.Print("ARROW: move camera FrontBack", 50, 90, 1.0f);
	break;
	}

	debugText.Print("SPACE: SCENE change", 550, 50, 1.0f);

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
	if (scene == 1) {
		// 3Dオブクジェクトの描画
		objSkydome->Draw();
		objGround->Draw();
		//objFighter->Draw();
		objSphere->Draw();
		objRay->Draw();
	}
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
	debugText.DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}
