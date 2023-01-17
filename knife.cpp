//=============================================================================
//
// ナイフ処理 [knife.cpp]
// Author : 小山　城
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "collision.h"
#include "knife.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_KNIFE			"data/MODEL/knife.obj"		// モデル名
#define	MODEL_KNIFE_PARTS	"data/MODEL/tracks.obj"		

#define	ROTATE_VALUE		(XM_PI * 0.02f)				// 回転量
#define	THROW_DIRECTION_POW	(4)				// 回転量
#define	THROW_HEIGHT_POW	(3.0f)				// 回転量

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static KNIFE			g_Knife[MAX_KNIFE];				// ナイフ


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitKnife(void)
{
	for (int i = 0; i < MAX_KNIFE; i++)
	{
		// 3Dモデル
		LoadModel(MODEL_KNIFE, &g_Knife[i].modelInfo,true,&g_Knife[i].modelVertexPosition);
		GetModelDiffuse(&g_Knife[i].modelInfo, &g_Knife[0].modelDiffuse[i]);
		g_Knife[i].load = true;

		// ゲーム内位置
		g_Knife[i].pos = XMFLOAT3(0.0f, -20.0f, 0.0f);
		g_Knife[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Knife[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		
		// ゲーム用ロジック
		g_Knife[i].attachedTo = NULL;
		g_Knife[i].velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Knife[i].acceleration = XMFLOAT3(0.0f, -0.5f, 0.0f);
		g_Knife[i].state = unused;
		
	}
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitKnife(void)
{
	for (int i = 0; i < MAX_KNIFE; i++)
	{
		if (!g_Knife[i].load) continue;
		
		UnloadModel(&g_Knife[i].modelInfo);
		g_Knife[i].load = false;
		delete[] g_Knife[i].modelVertexPosition.VertexArray;
		delete[] g_Knife[i].hitbox.list;
		
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateKnife(void)
{
	// ナイフを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_KNIFE; i++)
	{
		if (g_Knife[i].state == hit)
			UpdateHitbox(g_Knife[i].hitbox.list, g_Knife[i].modelVertexPosition.VertexNum,
				g_Knife[i].modelVertexPosition.VertexArray, g_Knife[i].pos, g_Knife[i].rot, g_Knife[i].scl);
		
		// 飛ばされているなら物理計算
		if (g_Knife[i].state != fired)	continue;
		
		// 移動処理
		XMVECTOR position = XMLoadFloat3(&g_Knife[i].pos);
		XMVECTOR velocity = XMLoadFloat3(&g_Knife[i].velocity);
		XMVECTOR acceleration = XMLoadFloat3(&g_Knife[i].acceleration);

		position += velocity;
		velocity += acceleration;

		XMStoreFloat3(&g_Knife[i].pos,position);
		XMStoreFloat3(&g_Knife[i].velocity, velocity);

		// 回転処理
		g_Knife[i].rot.x += ROTATE_VALUE;

		// ヒットボックス
		UpdateHitbox(g_Knife[i].hitbox.list, g_Knife[i].modelVertexPosition.VertexNum, 
			g_Knife[i].modelVertexPosition.VertexArray, g_Knife[i].pos, g_Knife[i].rot, g_Knife[i].scl);
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawKnife(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_WIREFRAME);

	for (int i = 0; i < MAX_KNIFE; i++)
	{
		if (g_Knife[i].state == unused) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Knife[i].scl.x, g_Knife[i].scl.y, g_Knife[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Knife[i].rot.x, g_Knife[i].rot.y, g_Knife[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Knife[i].pos.x, g_Knife[i].pos.y, g_Knife[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Knife[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Knife[i].modelInfo);

	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// ナイフの取得
//=============================================================================
KNIFE *GetKnife()
{
	return &g_Knife[0];
}

//=============================================================================
// ナイフを飛ばす
//=============================================================================
void fireKnife(XMFLOAT3 firePosition,XMFLOAT3 fireDirection) {
	for (int i = 0; i < MAX_KNIFE; i++)
	{
		if (!g_Knife[i].load || g_Knife[i].state != unused) continue;

		g_Knife[i].hitbox.list = new XMFLOAT3[g_Knife[i].modelVertexPosition.VertexNum];

		g_Knife[i].pos = firePosition;
		g_Knife[i].rot.y = fireDirection.y;
		g_Knife[i].velocity.x = -sinf(fireDirection.y)* THROW_DIRECTION_POW;
		g_Knife[i].velocity.z = -cosf(fireDirection.y)* THROW_DIRECTION_POW;
		g_Knife[i].velocity.y = THROW_HEIGHT_POW;
		g_Knife[i].state = fired;
		return;

	}
}