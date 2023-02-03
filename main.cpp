//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "collision.h"
#include "player.h"
#include "enemy.h"
#include "shadow.h"
#include "light.h"
#include "meshfield.h"
#include "meshwall.h"
#include "tree.h"
#include "bullet.h"
#include "score.h"
#include "sound.h"
#include "title.h"
#include "result.h"
#include "tutorial.h"
#include "particle.h"
#include "point.h"
#include "knife.h"
#include "skydome.h"
#include "fade.h"
#include "3DParticle.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"Desert Duel"		// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void CheckHit(void);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;
bool firstLoad = true;
tagRECT window;


#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

int	g_Mode = MODE_TITLE;					// 起動時の画面を設定


//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, true)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000/60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif
				RECT window;
				GetWindowRect(hWnd, &window);
				SetWindowInfo(window);
				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitSound(hWnd);

	InitLight();

	InitCamera();

	// 入力処理の初期化
	InitInput(hInstance, hWnd);
	//-----
	InitFade();

	//PlaySound(SOUND_LABEL_BGM_sample001);


	// ライトを有効化
	SetLightEnable(true);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);
	SetMode(g_Mode);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	UninitSound();

	UninitFade();
	// カメラの終了処理
	UninitCamera();

	//入力の終了処理
	UninitInput();

	// レンダラーの終了処理
	UninitRenderer();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();

	switch (g_Mode) {
	case MODE_TITLE:
		UpdateTitle();
		break;
	case MODE_TUTORIAL:
		UpdateTutorial();
		break;
	case MODE_GAME:
		UpdateSky();
		// 地面処理の更新
		UpdateMeshField();

		// 壁処理の更新
		UpdateMeshWall();

		// プレイヤーの更新処理
		UpdatePlayer();

		// エネミーの更新処理
		UpdateEnemy();

		// 木の更新処理
		UpdateTree();

		// 弾の更新処理
		UpdateBullet();

		UpdateParticle();
		UpdateParticle3D();
		UpdateKnife();

		// 影の更新処理
		UpdateShadow();

		// 当たり判定
		CheckHit();

		// スコアの更新処理
		UpdateScore();
		break;
	case MODE_RESULT:
		UpdateResult();
		break;

	}
	// カメラ更新
	UpdateCamera();

	UpdateFade();
	

}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();
	SetViewPort(TYPE_FULL_SCREEN);
	switch (g_Mode) {
	case MODE_TITLE:
		DrawTitle();
		break;
	case MODE_TUTORIAL:
		DrawTutorial();
		break;
	case MODE_GAME:
		// プレイヤー視点
		XMFLOAT3 pos = GetPlayer()->pos;
		pos.y += 10.0f;			// カメラ酔いを防ぐためにクリアしている
		SetCameraPos(pos);
		SetCamera();
		DrawSky();
		// 地面の描画処理
		DrawMeshField();

		// 影の描画処理
		DrawShadow();

		// プレイヤーの描画処理
		//DrawPlayer();

		// エネミーの描画処理
		DrawEnemy();
		UpdatePoint();
		// 弾の描画処理
		DrawBullet();

		// 壁の描画処理
		DrawMeshWall();
		DrawKnife();
		// 木の描画処理
		//DrawTree();

		DrawParticle();
		DrawParticle3D();
		DrawPoint();

		// スコアの描画処理
		//DrawScore();

		break;
	case MODE_RESULT:
		DrawResult();
		break;

	}
	//SetViewPort(TYPE_RIGHT_HALF_SCREEN);
	DrawFade();





#ifdef _DEBUG
	// デバッグ表示
	DrawDebugProc();
#endif

	// バックバッファ、フロントバッファ入れ替え
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif



//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY  *enemy  = GetEnemy();	// エネミーのポインターを初期化
	ENEMY  *enemyPart = GetEnemyParts();
	PLAYER *player = GetPlayer();	// プレイヤーのポインターを初期化
	KNIFE  *knife  = GetKnife();	// ナイフのポインターを初期化
	CAMERA* cam = GetCamera();
	
	XMFLOAT3 *playerVerts = new XMFLOAT3[player->points.VertexNum];
	CAPSULEHITBOX playerCapsule = player->hitbox;
	playerCapsule.positiona = AffineTransform(player->hitbox.positiona, XMLoadFloat4x4(&player->mtxWorld));
	playerCapsule.positionb = AffineTransform(player->hitbox.positionb, XMLoadFloat4x4(&player->mtxWorld));
	
	if (!playerVerts == NULL) {

		AppliedTransform(playerVerts, player->points.VertexNum, player->points.VertexArray, XMLoadFloat4x4(&player->mtxWorld));
		//PrintDebugProc("Player:X:%f Y:%f Z:%f\n", playerCapsule.positiona.x, playerCapsule.positiona.y, playerCapsule.positiona.z);
		//PrintDebugProc("Player:X:%f Y:%f Z:%f\n", playerCapsule.positionb.x, playerCapsule.positionb.y, playerCapsule.positionb.z);
		//PrintDebugProc("Player:vert:%d\n", player->points.VertexNum);
		// 敵とプレイヤーキャラ
		for (int i = 0; i < MAX_ENEMY; i++)
		{
			//敵の有効フラグをチェックする
			if (enemy[i].use == false)
				continue;

			XMFLOAT3* enemyVerts = new XMFLOAT3[enemy[i].points.VertexNum];
			CAPSULEHITBOX enemyCapsule = enemy[i].hitbox;
			enemyCapsule.positiona = AffineTransform(enemyCapsule.positiona, XMLoadFloat4x4(&enemy[i].mtxWorld));
			enemyCapsule.positionb = AffineTransform(enemyCapsule.positionb, XMLoadFloat4x4(&enemy[i].mtxWorld));
			if (!enemyVerts == NULL) {
				for (int p = 0; p < enemy[i].points.VertexNum; p++) {
					enemyVerts[p] = AffineTransform(enemy[i].points.VertexArray[p], XMLoadFloat4x4(&enemy[i].mtxWorld));
				}
				//PrintDebugProc("Enemyhit:X:%f Y:%f Z:%f\n", enemyCapsule.positiona.x, enemyCapsule.positiona.y, enemyCapsule.positiona.z);
				//PrintDebugProc("Enemyhit:X:%f Y:%f Z:%f\n", enemyCapsule.positionb.x, enemyCapsule.positionb.y, enemyCapsule.positionb.z);
				
				//GJKHit(playerVerts, player->points.VertexNum, enemyVerts, enemy[i].points.VertexNum)
				//BCの当たり判定
				if (CapsuleCollision(playerCapsule,enemyCapsule))
				{

					// 敵キャラクターは倒される
					/*enemy[i].use = false;*/
					ReleaseShadow(enemy[i].shadowIdx);

					// スコアを足す
					AddScore(100);

				}
			}
			delete[] enemyVerts;
		}
	}
	delete[] playerVerts;
	


	// プレイヤーの弾と敵
	for (int i = 0; i < MAX_KNIFE; i++)
	{
		//ナイフの有効フラグをチェックする
		if (knife[i].state == unused)
			continue;

		// 敵と当たってるか調べる

		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//for (int k = 0; k < ENEMY_PARTS_MAX; k++) {
			//	if (enemyPart[k].use == false
			//		||knife[i].state == hit
			//		|| !GJKHit(knife[i].hitbox.list, knife[i].modelVertexPosition.VertexNum, enemyPart[k].gjkList.list, enemyPart[k].points.VertexNum))
			//		continue;
			//	XMMATRIX mtxWorld = XMMatrixIdentity();
			//	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(knife[i].rot.x, knife[i].rot.y, knife[i].rot.z);

			//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			//	// 当たったからナイフ状態更新
			//	knife[i].state = hit;
			//	knife[i].attachedTo = &enemyPart[k];
			//	knife[i].parentRot = knife[i].attachedTo->rot;
			//	XMVECTOR knifeInfo = XMLoadFloat3(&knife[i].pos);
			//	XMVECTOR enemyInfo = XMLoadFloat3(&knife[i].attachedTo->pos);
			//	XMFLOAT3 position;
			//	knifeInfo -= enemyInfo;
			//	XMStoreFloat3(&position, knifeInfo);

			//	 mtxWorld = XMMatrixTranspose(XMLoadFloat4x4(&enemyPart[k].mtxWorld));
			//	AffineTransform(position, mtxWorld);
			//	knife[i].pos = position;
			//	break;
			//}
			//敵の有効フラグをチェックする
			if (enemy[j].use == false
				|| !GJKHit(knife[i].hitbox.list, knife[i].modelVertexPosition.VertexNum, enemy[j].gjkList.list, enemy[j].points.VertexNum))
				continue;

			//GJKの当たり判定
			// 当たったからナイフ状態更新
			//for (int h = 0; h < 10; h++) {
			//	SetParticle(knife[i].pos, { (rand() % 30 - 15) * 2.0f,5.0f,(rand() % 30 - 15) * 2.0f }, { 0.0f,-2.0f,0.0f }, cam->rot,
			//		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), 0.1f, 0.1f, false, 0, 240);
			//	

			//}
			//SetParticle3D(knife[i].pos, {0,0,0},16,true);

			knife[i].state = hit;
			knife[i].attachedTo = &enemy[j];
			knife[i].parentRot = knife[i].attachedTo->rot;
			XMVECTOR knifeInfo = XMLoadFloat3(&knife[i].pos);
			XMVECTOR enemyInfo = XMLoadFloat3(&knife[i].attachedTo->pos);
			XMFLOAT3 position;
			knifeInfo -= enemyInfo;
			XMStoreFloat3(&position, knifeInfo/2);

			knifeInfo = XMLoadFloat3(&knife[i].scl);
			enemyInfo = XMLoadFloat3(&knife[i].attachedTo->scl);
			knifeInfo /= enemyInfo;
			XMStoreFloat3(&knife[i].scl, knifeInfo);

			XMMATRIX mtxWorld = XMMatrixTranspose(XMLoadFloat4x4(&enemy[j].mtxWorld));
			AffineTransform(position, mtxWorld );
			knife[i].pos = position;

			//当たった時の処理
			PlaySound(SOUND_LABEL_SE_hit);
			
			//enemy[0].hp -= 10;
			if (enemy[0].hp <= 0) {
				enemy[0].use = false;
				//SetMode(MODE_RESULT);
			}
			// スコアを足す
			AddScore(10);

		}

	}


	// エネミーが全部死亡したら状態遷移
	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == false) continue;
		enemy_count++;
	}

	// エネミーが０匹？
	if (enemy_count == 0)
	{
		 SetMode(MODE_RESULT);

	}

}

void SetMode(int mode) {
	g_Mode = mode;
	StopSound();

	// タイトル画面の終了処理
	UninitTitle();

	//
	UninitTutorial();
	UninitPoint();
	//ゲーム画面の終了処理
	// スコアの終了処理
	UninitScore();
	UninitSky();
	// 弾の終了処理
	UninitBullet();

	// 木の終了処理
	UninitTree();

	// エネミーの終了処理
	UninitEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

	// 壁の終了処理
	UninitMeshWall();

	// 地面の終了処理
	UninitMeshField();

	UninitParticle();
	UninitParticle3D();
	UninitKnife();
	//リザルト処理の終了処理
	UninitResult();

	

	switch (g_Mode) {
	case MODE_TITLE:
		InitTitle();
		//PlaySound(SOUND_LABEL_BGM_sample000);
		break;
	case MODE_TUTORIAL:
		InitTutorial();
		//PlaySound(SOUND_LABEL_BGM_sample001);
		break;
	case MODE_GAME:
		// フィールドの初期化
		InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 20, 20, 100.0f, 100.0f);
		firstLoad = false;
		//// 壁の初期化
		//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
		//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
		//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
		//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

		//// 壁(裏側用の半透明)
		//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, XM_PI, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
		//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
		//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
		//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

		// 影の初期化処理
		InitShadow();
		InitSky();
		// プレイヤーの初期化
		InitPlayer();

		// エネミーの初期化
		InitEnemy();

		InitKnife();

		InitPoint();
		// 木を生やす
		InitTree();

		// 弾の初期化
		InitBullet();
		InitParticle3D();
		InitParticle();
		// スコアの初期化
		InitScore();
		//PlaySound(SOUND_LABEL_BGM_sample002);
		break;
	case MODE_RESULT:
		InitResult();
		//PlaySound(SOUND_LABEL_BGM_sample003);
		break;
	}
}

int	GetMode(void) {
	return g_Mode;
}

bool IsFirstLoad(void) {
	return firstLoad;
}

void setLoad(bool update) {
	firstLoad = update;
}

