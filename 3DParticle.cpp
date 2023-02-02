//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "3DParticle.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PARTICLE3D	"data/MODEL/smoke.obj"		// 読み込むモデル名

#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PARTICLE3D		g_Particle3D[MAX_PARTICLE3D];				// エネミー

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticle3D(void)
{
	for (int i = 0; i < MAX_PARTICLE3D; i++)
	{
		LoadModel(MODEL_PARTICLE3D, &g_Particle3D[i].modelInfo, false, new VERTEX_DATA);
		GetModelDiffuse(&g_Particle3D[i].modelInfo, &g_Particle3D[i].modelDiffuse[0]);
		g_Particle3D[i].load = true;

		g_Particle3D[i].pos = XMFLOAT3(0.0f, -20.0f, 20.0f);
		g_Particle3D[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Particle3D[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Particle3D[i].showTime = 0.0f;
		g_Particle3D[i].Center = false;
		g_Particle3D[i].use = false;

	}
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticle3D(void)
{

	for (int i = 0; i < MAX_PARTICLE3D; i++)
	{
		if (g_Particle3D[i].load)
		{
			UnloadModel(&g_Particle3D[i].modelInfo);
			g_Particle3D[i].load = false;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticle3D(void)
{
	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_PARTICLE3D; i++)
	{
		if (g_Particle3D[i].use != true)	
			continue;		// このエネミーが使われている？
	
		g_Particle3D[i].showTime--;
		g_Particle3D[i].rot.y += 0.01f;
		g_Particle3D[i].rot.z += 0.01f;
		g_Particle3D[i].rot.x += 0.01f;
		if (g_Particle3D[i].Center) 
		{
			Particle3DCenterExplsosion(&g_Particle3D[i]);
		}


	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticle3D(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_BACK);

	for (int i = 0; i < MAX_PARTICLE3D; i++)
	{
		if (g_Particle3D[i].use == false) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Particle3D[i].scl.x, g_Particle3D[i].scl.y, g_Particle3D[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Particle3D[i].rot.x, g_Particle3D[i].rot.y, g_Particle3D[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Particle3D[i].pos.x, g_Particle3D[i].pos.y, g_Particle3D[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Particle3D[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Particle3D[i].modelInfo);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// パーティクルの設定
//=============================================================================
void SetParticle3D(XMFLOAT3 position, XMFLOAT3 deltaPosition, float ShowTime, bool center) 
{
	for (int i = 0; i < MAX_PARTICLE3D; i++)
	{
		if (g_Particle3D[i].use)
			continue;

		g_Particle3D[i].Center = center;
		g_Particle3D[i].pos = position;
		g_Particle3D[i].velocity = deltaPosition;
		g_Particle3D[i].showTime = 16;
		g_Particle3D[i].use = true;
	}
}

void Particle3DCenterExplsosion(PARTICLE3D *particle) {
	XMVECTOR size = {1.0f,1.0f,1.0f};
	if (particle->showTime < 0) 
	{ 
		particle->use = false;
		return; 
	}
	
	if (particle->showTime > 8) {
		size *= (8 - particle->showTime - 8) / 8 * 3.0f;
		XMStoreFloat3(&particle->scl, size);
		return;
	}
	if (particle->showTime < 8) {

	}
	
	
}