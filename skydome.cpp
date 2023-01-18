//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "debugproc.h"
#include "model.h"
#include "light.h"
#include "skydome.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_SKY		"data/MODEL/skydome.obj"			// 読み込むモデル名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static MDL		g_Sky;						// プレイヤー

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSky(void)
{
	//LoadModel(MODEL_SKY, &g_Sky.model, true);
	g_Sky.load = true;

	g_Sky.pos = { 0.0f, 0.0f, 0.0f };
	g_Sky.rot = { 0.0f, 0.0f, 0.0f };
	g_Sky.scl = { 11.0f, 11.0f, 11.0f };
	g_Sky.use = true;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSky(void)
{
	// モデルの解放処理
	if (g_Sky.load)
	{
		UnloadModel(&g_Sky.model);
		g_Sky.load = false;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSky(void)
{

	g_Sky.rot.y += 0.0001f;
	{	// ポイントライトのテスト
		LIGHT* light = GetLightData(1);
		XMFLOAT3 pos = g_Sky.pos;
		pos.y -= 50.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = true;
		SetLightData(1, light);
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSky(void)
{
	SetLightEnable(false);
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Sky.scl.x, g_Sky.scl.y, g_Sky.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Sky.rot.x, g_Sky.rot.y + XM_PI, g_Sky.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Sky.pos.x, g_Sky.pos.y, g_Sky.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Sky.mtxWorld, mtxWorld);

	// モデル描画
	DrawModel(&g_Sky.model);

	SetLightEnable(true);


	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}