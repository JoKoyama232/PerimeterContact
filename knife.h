//=============================================================================
//
// ナイフ処理 [knife.h]
// Author : 小山　城
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_KNIFE		(20)					// エネミーの数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
enum KNIFESTATE {
	unused = 0,
	fired,
	hit,
};

struct KNIFE
{
	// 3Dモデル変数群
	DX11_MODEL	modelInfo;							// モデル情報
	XMFLOAT4	modelDiffuse[MODEL_MAX_MATERIAL];	// モデルの色
	VERTEX_DATA	modelVertexPosition;				// モデルの頂点座標
	HITBOX		hitbox;								// 当たり判定用座標
	bool		load;								// モデルの読み込みフラグ

	// ゲーム内位置変数群
	XMFLOAT4X4	mtxWorld;							// ワールドマトリックス
	XMFLOAT3	pos;								// モデルの位置
	XMFLOAT3	rot;								// モデルの向き(回転)
	XMFLOAT3	scl;								// モデルの大きさ(スケール)
	XMFLOAT3	parentRot;							// 当たったオブジェクトの向き

	// ゲームロジック変数群
	ENEMY*		attachedTo;							// 当たったエネミー
	XMFLOAT3	velocity;							// ナイフの速度
	XMFLOAT3	acceleration;						// ナイフの加速度
	DWORD		latestUpdate;						// 最後に更新された時間
	KNIFESTATE	state;								// ナイフ現在の状態
	

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitKnife(void);
void UninitKnife(void);
void UpdateKnife(void);
void DrawKnife(void);

KNIFE* GetKnife(void);
void FireKnife(XMFLOAT3 firePosition, XMFLOAT3 fireDirection);
void explodeKnife(void);
