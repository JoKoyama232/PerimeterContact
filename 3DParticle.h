//=============================================================================
//
// エネミーモデル処理 [Particle3D.h]
// Author : 小山　城
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PARTICLE3D		(255)	// エネミーの数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PARTICLE3D
{
	// 3Dモデル変数群
	DX11_MODEL	modelInfo;			// モデル情報
	XMFLOAT4	modelDiffuse[MODEL_MAX_MATERIAL];	// モデルの色
	bool		load;				// モデル情報読み込みフラグ

	// ゲーム内位置変数群
	XMFLOAT4X4	mtxWorld;			// ワールドマトリックス
	XMFLOAT3	pos;				// モデルの位置
	XMFLOAT3	rot;				// モデルの向き(回転)
	XMFLOAT3	scl;				// モデルの大きさ(スケール)
	XMFLOAT3	velocity;			// モデルの速度	
	
	// ゲームロジック変数群
	float		showTime;			// 描画時間（フレーム数）
	bool		Center;				// 中心爆発フラグ
	bool		use;				// 描画フラグ
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle3D(void);
void UninitParticle3D(void);
void UpdateParticle3D(void);
void DrawParticle3D(void);

void SetParticle3D(XMFLOAT3 position, XMFLOAT3 deltaPosition, float ShowTime, bool center);
void Particle3DCenterExplsosion(PARTICLE3D* particle);
