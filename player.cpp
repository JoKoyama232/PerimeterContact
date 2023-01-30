//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "particle.h"
#include "meshfield.h"
#include "collision.h"
#include "enemy.h"
#include "knife.h"
#include "player.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/enemy.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_HAND	"data/MODEL/hand.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_HEAD	"data/MODEL/head.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS	"data/MODEL/funnel_s.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_1	"data/MODEL/funnel_f.obj"			// 読み込むモデル名

#define	VALUE_MOVE			(4.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(1.0f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(10.0f)							// プレイヤーの足元をあわせる

#define PLAYER_PARTS_MAX	(7)								// プレイヤーのパーツの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー
static float		camRotY =0.0f;
static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用
static XMFLOAT3		dashTbl[2];
static int			particleWait = 0;
static RASTERIZER_MODE drawMethod = CULL_MODE_BACK;

static DWORD dashcd;
static DWORD attackcd;

// プレイヤーの階層アニメーションデータ
// プレイヤーの頭を左右に動かしているアニメデータ
static INTERPOLATION_DATA move_tbl_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(20.0f, 20.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(20.0f, 20.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },

};

static INTERPOLATION_DATA move_tbl_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },

};

static INTERPOLATION_DATA move_tbl_handLeft[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(20.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(20.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_handRight[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-20.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-20.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	move_tbl_right,
	move_tbl_left,
	move_tbl_handLeft,
	move_tbl_handRight,
};



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model,true,&g_Player.points);
	g_Player.load = true;

	g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, 0.0f };
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };
	g_Player.time = 0.0f;
	g_Player.spd = 0.0f;			// 移動スピードクリア
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ
	g_Player.tblNo = -1;
	g_Player.hitbox.positiona = { 0.0f, 3.0f, 0.0f };
	g_Player.hitbox.positionb = { 0.0f, PLAYER_OFFSET_Y * 2.0f -3.0f, 0.0f };
	g_Player.hitbox.radius = 3.0f;
	g_Player.use = true;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号



	// 階層アニメーション用の初期化処理
	g_Player.attachedTo = NULL;			// 本体（親）なのでNULLを入れる

	// パーツの初期化
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		
		g_Parts[i].use = false;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, XM_PI, 0.0f);
		g_Parts[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);

		// 親子関係
		g_Parts[i].attachedTo = &g_Player;		// ← ここに親のアドレスを入れる
	//	g_Parts[腕].attachedTo= &g_Player;		// 腕だったら親は本体（プレイヤー）
	//	g_Parts[手].attachedTo= &g_Paerts[腕];	// 指が腕の子供だった場合の例

		// 階層アニメーション用のメンバー変数の初期化
		g_Parts[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Parts[i].tblNo = -1;			// 再生する行動データテーブルNoをセット
		g_Parts[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Parts[i].load = 0;
	}
	g_Parts[0].pos = XMFLOAT3(20.0f, 15.0f, -10.0f);
	g_Parts[1].pos = XMFLOAT3(-20.0f, 15.0f, -10.0f);
	g_Parts[2].pos = XMFLOAT3(20.0f, 5.0f, 5.0f);
	g_Parts[3].pos = XMFLOAT3(-20.0f, 5.0f, 5.0f);

	//g_Parts[0].time = 0.0f;			// 線形補間用のタイマーをクリア
	//g_Parts[0].tblNo = 1;			// 再生する行動データテーブルNoをセット
	//g_Parts[0].tblMax = sizeof(move_tbl_left) / sizeof(INTERPOLATION_DATA);;			// 再生する行動データテーブルのレコード数をセット
	g_Parts[0].use = true;
	g_Parts[0].load = true;

	//g_Parts[1].time = 0.0f;			// 線形補間用のタイマーをクリア
	//g_Parts[1].tblNo = 0;			// 再生する行動データテーブルNoをセット
	//g_Parts[1].tblMax = sizeof(move_tbl_right) / sizeof(INTERPOLATION_DATA);;			// 再生する行動データテーブルのレコード数をセット
	g_Parts[1].use = true;
	g_Parts[1].load = true;

	g_Parts[2].use = true;
	g_Parts[2].load = true;
	g_Parts[2].rot.z = -XM_PI / 2;

	g_Parts[3].use = true;
	g_Parts[3].load = true;
	g_Parts[3].rot.z = XM_PI / 2;

	g_Parts[6].scl = XMFLOAT3(1.5f, 1.5f, 1.5f);
	g_Parts[6].use = true;
	g_Parts[6].load = true;

	//g_Parts[4].time = 0.0f;			// 線形補間用のタイマーをクリア
	//g_Parts[4].tblNo = 4;			// 再生する行動データテーブルNoをセット
	//g_Parts[4].tblMax = sizeof(move_tbl_handRight) / sizeof(INTERPOLATION_DATA);;			// 再生する行動データテーブルのレコード数をセット
	//g_Parts[4].use = true;
	//g_Parts[4].load = true;

	//g_Parts[5].time = 0.0f;			// 線形補間用のタイマーをクリア
	//g_Parts[5].tblNo = 3;			// 再生する行動データテーブルNoをセット
	//g_Parts[5].tblMax = sizeof(move_tbl_handLeft) / sizeof(INTERPOLATION_DATA);;			// 再生する行動データテーブルのレコード数をセット
	//g_Parts[5].use = true;
	//g_Parts[5].load = true;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = false;
	}
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load)
		{
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = false;
		}
		
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();
	DWORD cdTimer = timeGetTime();
	if (g_Player.tblNo == -1) {
		// 移動させちゃう
		if (GetKeyboardPress(DIK_A))
		{	// 左へ移動
			g_Player.spd = VALUE_MOVE;
			g_Player.dir = XM_PI / 2;
		}
		if (GetKeyboardPress(DIK_D))
		{	// 右へ移動
			g_Player.spd = VALUE_MOVE;
			g_Player.dir = -XM_PI / 2;
		}
		if (GetKeyboardPress(DIK_W))
		{	// 前へ移動
			g_Player.spd = VALUE_MOVE;
			g_Player.dir = XM_PI;
		}
		if (GetKeyboardPress(DIK_S))
		{	// 後ろへ移動
			g_Player.spd = VALUE_MOVE;
			g_Player.dir = 0.0f;
		}
		if (GetKeyboardPress(DIK_LSHIFT)) 
		{
			g_Player.spd *= 2;
		}
		if (GetKeyboardTrigger(DIK_SPACE) && cdTimer - dashcd > 3000)
		{
			//DASH
			g_Player.tblNo = 1;
			g_Player.time = 0.5f;
			dashcd = cdTimer;
			dashTbl[0] = g_Player.pos;
			dashTbl[1] = XMFLOAT3(g_Player.pos.x - sinf(g_Player.rot.y) * VALUE_MOVE * 60,
				g_Player.pos.y, g_Player.pos.z - cosf(g_Player.rot.y) * VALUE_MOVE * 60);
		}
		if (IsMouseRightPressed() == FALSE) {
			camRotY = cam->rot.y;
		}
		if (GetKeyboardTrigger(DIK_Q))
		{
			if (drawMethod == CULL_MODE_BACK) {
				drawMethod = CULL_MODE_WIREFRAME;
			}
			else {
				drawMethod = CULL_MODE_BACK;
			}

		}


#ifdef _DEBUG
		if (GetKeyboardPress(DIK_R))
		{
			g_Player.pos.z = g_Player.pos.x = 0.0f;
			g_Player.rot.y = g_Player.dir = 0.0f;
			g_Player.spd = 0.0f;
		}
#endif


		//移動量が０以上であれば移動する
		if (g_Player.spd > 0.3f)
		{
			g_Player.rot.y = g_Player.dir + camRotY;

			// 入力のあった方向へプレイヤーを向かせて移動させる
			//プレイヤーの向いている方向を保存
			float deltaX = sinf(g_Player.rot.y) * g_Player.spd;
			float deltaZ = cosf(g_Player.rot.y) * g_Player.spd;

			//移動を反映
			g_Player.pos.x -= deltaX;
			g_Player.pos.z -= deltaZ;

			//移動量が目に見える変化量の時、
			//パーティクルを生成（カウントを使用して、生成量を減らす）
			if (g_Player.spd > VALUE_MOVE &&particleWait > 3) {
				particleWait = 0;		//待機カウントリセット

				//パーティクルの生成用引数計算		
				XMFLOAT3 pos = g_Player.pos;									//プレイヤーの座標を起点に
				pos.y += rand() % (int)(PLAYER_OFFSET_Y * 2) - PLAYER_OFFSET_Y;	//プレイヤーの身長ないにランダムに配置
				pos.x += rand() % (int)(PLAYER_OFFSET_Y * 4) - PLAYER_OFFSET_Y * 2;	//プレイヤーの身長ないにランダムに配置

				deltaX *= 10.0f;			//X座標の移動量
				deltaZ *= 10.0f;			//Z座標の移動量

				//プレイヤーの速度によってパーティクルの移動距離も変化する
				XMFLOAT3 delta = XMFLOAT3(deltaX, 0.0f, deltaZ);

				XMFLOAT3 rot = g_Player.rot;
				rot.x += +XM_PI / 2;

				//パーティクルの移動量の変動値
				XMFLOAT3 acceleration = XMFLOAT3(0.7f, 0.0f, 0.7f);

				//パーティクルの色（RGBA 0.0f~1.0f）
				XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);

				float pWidth = 0.25f;	//パーティクルの横幅
				float pHeight = 0.5f;	//パーティクルの縦幅		
				int pLife = 25;	//パーティクルの表示時間（フレーム単位）

				SetParticle(pos, delta, acceleration, rot, color, pWidth, pHeight, false, 0, pLife);
			}
			particleWait++;
		}
	}
	else {
		g_Player.pos = GetLerp(dashTbl[0], dashTbl[1], ((1 - cosf(g_Player.time * XM_PI)) * 0.5f));
		g_Player.time += 0.1f;
		if (g_Player.time > 1.0f) {
			g_Player.tblNo = -1;
		}
	}
	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// レイキャストして足元の高さを求める
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	bool ans = RayHitField(g_Player.pos, &HitPosition, &Normal);
	if (ans)
	{
		g_Player.pos.y = HitPosition.y + PLAYER_OFFSET_Y;
	}
	else
	{
		g_Player.pos.y = PLAYER_OFFSET_Y;
		Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}

	// 弾発射処理
	if (IsMouseLeftTriggered())
	{
		//SetBullet(g_Player.pos, XMFLOAT3(0,100,0), XMFLOAT3(0, 0, 400));
		FireKnife(g_Player.pos, g_Player.rot);
	}

	g_Player.spd *= 0.5f;


	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// 使われているなら処理する
		if ((g_Parts[i].use == true)&&(g_Parts[i].tblMax > 0))
		{	// 線形補間の処理
			int nowNo = (int)g_Parts[i].time;			// 整数分であるテーブル番号を取り出している
			int maxNo = g_Parts[i].tblMax;				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
			INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Parts[i].tblNo];	// 行動テーブルのアドレスを取得

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

			float nowTime = g_Parts[i].time - nowNo;	// 時間部分である少数を取り出している

			Pos *= nowTime;								// 現在の移動量を計算している
			Rot *= nowTime;								// 現在の回転量を計算している
			Scl *= nowTime;								// 現在の拡大率を計算している

			// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
			XMStoreFloat3(&g_Parts[i].pos, nowPos + Pos);

			// 計算して求めた回転量を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[i].rot, nowRot + Rot);

			// 計算して求めた拡大率を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[i].scl, nowScl + Scl);

			// frameを使て時間経過処理をする
			g_Parts[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
			if ((int)g_Parts[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				g_Parts[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
			}

		}
		else {
			if (i == 6) { 
				g_Parts[i].time += 0.01f; 
				g_Parts[i].pos.y -= 0.05f * sinf(2*g_Parts[i].time);
			}
			else {
				g_Parts[i].time += 0.05f; 
				g_Parts[i].pos.y += 0.2f * sinf(g_Parts[i].time);
			}
			
			
		}

	}



	{	// ポイントライトのテスト
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = true;
		SetLightData(1, light);
	}
	
	
	{//当たり判定
		if (true) {
			//knockBack away from leg or sword
		}
	
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	SetLightEnable(false);
	// カリング無効
	SetCullingMode(drawMethod);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// モデル描画
	DrawModel(&g_Player.model);
	XMFLOAT3* playerVerts = new XMFLOAT3[g_Player.points.VertexNum];
		for (int p = 0; p < g_Player.points.VertexNum; p++) {
			playerVerts[p] = AffineTransform(g_Player.points.VertexArray[p], XMLoadFloat4x4(&g_Player.mtxWorld));
		}


	
	// パーツの階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].attachedTo != NULL)	// 子供だったら親と結合する
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].attachedTo->mtxWorld));
			// ↑
			// g_Player.mtxWorldを指している
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
		if (g_Parts[i].use == false) continue;

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);


		// モデル描画
		DrawModel(&g_Parts[i].model);

	}
	SetLightEnable(true);


	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

XMFLOAT3 GetLerp(XMFLOAT3 initialPos, XMFLOAT3 endingPos, float percentage) {
	XMFLOAT3 result;
	XMVECTOR pos0 = XMLoadFloat3(&initialPos);
	XMVECTOR pos1 = XMLoadFloat3(&endingPos);

	pos0 += (pos1 - pos0) * percentage;
	XMStoreFloat3(&result, pos0);
	return result;
}

//337
float GetDistance3D(XMFLOAT3 pos1, XMFLOAT3 pos2) {
	return (float)sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y) + (pos1.z - pos2.z) * (pos1.z - pos2.z));
}

RASTERIZER_MODE GetRasterizerMode(void) {
	return drawMethod;
}
