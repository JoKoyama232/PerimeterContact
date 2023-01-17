//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"
#include "meshfield.h"
#include "collision.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/player.obj"		// 読み込むモデル名
#define	MODEL_ENEMY_PARTS	"data/MODEL/tracks.obj"

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ENEMY_OFFSET_Y		(40.0f)						// エネミーの足元をあわせる


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー
static ENEMY		g_Parts[ENEMY_PARTS_MAX];		// エネミーのパーツ用

int g_Enemy_load = 0;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model,true,&g_Enemy[i].points);
		g_Enemy[i].load = true;

		g_Enemy[i].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Enemy[i].hp = 20;
		g_Enemy[i].spd = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].move_time = 0.0f;	// 線形補間用のタイマーをクリア
		g_Enemy[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		g_Enemy[i].use = true;			// true:生きてる
		g_Enemy[i].attachedTo = NULL;

		//tracks
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{

			LoadModel(MODEL_ENEMY_PARTS, &g_Parts[j].model, true, &g_Parts[j].points);
			g_Parts[j].load = true;
			switch (j) {
			case 0:
				//Front Right
				g_Parts[j].pos = XMFLOAT3(30.0f, -ENEMY_OFFSET_Y * 2, -5.0f);
				g_Parts[j].rot = XMFLOAT3(XM_PI / 5, 0.0f, 0.0f);
				break;
			case 1:
				//Front Left
				g_Parts[j].pos = XMFLOAT3(-30.0f, -ENEMY_OFFSET_Y * 2, -5.0f);
				g_Parts[j].rot = XMFLOAT3(XM_PI / 5, 0.0f, 0.0f);
				break;
			case 2:
				//Back Right
				g_Parts[j].pos = XMFLOAT3(30.0f, -ENEMY_OFFSET_Y * 2, 0.0f);
				g_Parts[j].rot = XMFLOAT3(XM_PI / 5, XM_PI, 0.0f);
				break;
			case 3:
				//Back Left
				g_Parts[j].pos = XMFLOAT3(-30.0f, -ENEMY_OFFSET_Y * 2, 0.0f);
				g_Parts[j].rot = XMFLOAT3(XM_PI / 5, XM_PI, 0.0f);
				break;
			}
			g_Parts[j].scl = XMFLOAT3(2.0f, 2.0f, 2.0f);
			g_Parts[j].spd = 0.0f;
			g_Parts[j].size = ENEMY_SIZE;
			g_Parts[j].move_time = 0.0f;	// 線形補間用のタイマーをクリア
			g_Parts[j].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
			g_Parts[j].tbl_size = 0;		// 再生するアニメデータのレコード数をセット
			g_Parts[j].use = true;			// true:生きてる
			g_Parts[j].attachedTo = &g_Enemy[i];


		}
	}
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = false;
			delete[] g_Enemy[i].points.VertexArray;
		}
		for (int j = 0; j < ENEMY_PARTS_MAX; j++) {
			if (g_Parts[j].load)
			{
				UnloadModel(&g_Parts[j].model);
				g_Parts[j].load = false;
				delete[] g_Parts[j].points.VertexArray;
			}

		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use != true)	continue;		// このエネミーが使われている？
											// Yes
		if (g_Enemy[i].tbl_adr != NULL)	// 線形補間を実行する？
		{								// 線形補間の処理
			// 移動処理
			int		index = (int)g_Enemy[i].move_time;
			float	time = g_Enemy[i].move_time - index;
			int		size = g_Enemy[i].tbl_size;

			float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1フレームで進める時間
			g_Enemy[i].move_time += dt;							// アニメーションの合計時間に足す

			if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
			{
				g_Enemy[i].move_time = 0.0f;
				index = 0;
			}
			for (int j = 0; j < ENEMY_PARTS_MAX; j++) {
				if (!g_Parts[j].use) continue;

			}

			//// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
			//XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// 次の場所
			//XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// 現在の場所
			//XMVECTOR vec = p1 - p0;
			//XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

			//// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
			//XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// 次の角度
			//XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// 現在の角度
			//XMVECTOR rot = r1 - r0;
			//XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

			//// scaleを求める S = StartX + (EndX - StartX) * 今の時間
			//XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// 次のScale
			//XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// 現在のScale
			//XMVECTOR scl = s1 - s0;
			//XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

		}

		// 影もプレイヤーの位置に合わせる
		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		
	}

	for (int i = 0; i < ENEMY_PARTS_MAX * MAX_ENEMY; i++)
	{
		XMFLOAT3 trackpos = AffineTransform(g_Parts[i].points.VertexArray[15], XMLoadFloat4x4(&g_Parts[i].mtxWorld));
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_WIREFRAME);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == false) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Enemy[i].model);

		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			if (g_Parts[j].use == false) continue;

			// ワールドマトリックスの初期化
			mtxWorld = XMLoadFloat4x4(&g_Enemy[i].mtxWorld);

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Parts[j].scl.x, g_Parts[j].scl.y, g_Parts[j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[j].rot.x, g_Parts[j].rot.y + XM_PI, g_Parts[j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Parts[j].pos.x, g_Parts[j].pos.y, g_Parts[j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[j].attachedTo != NULL)
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].attachedTo->mtxWorld));
			}

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Parts[j].mtxWorld, mtxWorld);

			DrawModel(&g_Parts[j].model);
		}
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

ENEMY *GetEnemyParts(){
	return &g_Parts[0];
}