//=============================================================================
//
// エネミーモデル処理 [enemy.h]
// Author : 小山　城
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ENEMY		(1)					// エネミーの数

#define	ENEMY_SIZE		(5.0f)				// 当たり判定の大きさ
#define ENEMY_PARTS_MAX		(4)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	bool				use;
	bool				load;
	DX11_MODEL			model;				// モデル情報
	VERTEX_DATA			points;
	CAPSULEHITBOX		hitbox;
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色
	float				hp;
	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号
	
	INTERPOLATION_DATA	*tbl_adr;			// アニメデータのテーブル先頭アドレス
	int					tbl_size;			// 登録したテーブルのレコード総数
	float				move_time;			// 実行時間


	ENEMY* attachedTo;	// 自分が親ならNULL、自分が子供なら親のplayerアドレス

};

struct ENEMYHitBox {
	HITBOX* hitbox;
	ENEMY* attachedTo;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
ENEMY* GetEnemyParts();
