//=============================================================================
//
// 弾発射処理 [bullet.h]
// Author : 小山　城
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_BULLET		(256)	// 弾最大数
#define	BULLET_WH		(5.0f)	// 当たり判定の大きさ

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	// 3Dモデル変数群
	DX11_MODEL	model;		// モデル情報
	VERTEX_DATA	points;		// モデルの頂点情報
	bool		load;		// モデルの読み込みフラグ

	// ゲーム内位置変数群
	XMFLOAT4X4	mtxWorld;	// ワールドマトリックス
	XMFLOAT3	pos;		// ポリゴンの位置
	XMFLOAT3	rot;		// ポリゴンの向き(回転)
	XMFLOAT3	scl;		// ポリゴンの大きさ(スケール)

	// ゲームロジック変数群
	XMFLOAT3	startPos;	// ベジェ曲線の始点
	XMFLOAT3	*target;	// ベジェ曲線の終点（追尾の為）
	XMFLOAT3	targetPos;	// ベジェ曲線の終点（追尾が切れた時）		
	XMFLOAT3	middlePos;	// ベジェ曲線の中間点
	
	DWORD		timer;		// 使用開始時からの時間（ms）
	float		speed;		// バレットの一秒間の速度
	float		progress;	// バレットの軌道上の位置
	bool		use;		// 使用しているかどうか

} BULLET;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 initialPos, XMFLOAT3 *targetPos, float speed);

BULLET *GetBullet(void);
XMFLOAT3 GetBezier(XMFLOAT3 initialPos, XMFLOAT3 middlePos, XMFLOAT3 endPos, float percentage);

