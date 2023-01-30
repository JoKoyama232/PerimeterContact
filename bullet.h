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
	XMFLOAT4X4	mtxWorld;		// ワールドマトリックス
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	*target;
	XMFLOAT3	targetPos;			// スケール
	XMFLOAT3	startPos;
	XMFLOAT3	middlePos;
	XMFLOAT3	rot;			// 角度
	MATERIAL	material;		// マテリアル
	
	DWORD		timer;
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	float		speed;
	float		progress;
	int			shadowIdx;		// 影ID
	bool		use;			// 使用しているかどうか


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

