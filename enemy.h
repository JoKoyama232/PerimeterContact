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
#define ENEMY_PARTS_MAX	(4)					// エネミーの部品数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY
{
	// 3Dモデル変数群
	DX11_MODEL			model;							// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色
	VERTEX_DATA			points;							// モデルの頂点座標
	HITBOX				gjkList;						// 当たり判定用座標
	CAPSULEHITBOX		hitbox;							// カプセルコリジョン用変数
	bool				load;							// モデルの読み込みフラグ

	// ゲーム内位置変数群
	XMFLOAT4X4			mtxWorld;						// ワールドマトリックス
	XMFLOAT3			pos;							// モデルの位置
	XMFLOAT3			rot;							// モデルの向き(回転)
	XMFLOAT3			scl;							// モデルの大きさ(スケール)
	int					tbl_size;						// 登録したテーブルのレコード総数
	INTERPOLATION_DATA* tbl_adr;						// アニメデータのテーブル先頭アドレス
	ENEMY* attachedTo;	// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// ゲームロジック変数群
	float				hp;								// ヒットポイント
	float				spd;							// 移動スピード
	float				size;							// 当たり判定の大きさ
	int					shadowIdx;						// 影のインデックス番号
	float				move_time;						// 実行時間
	bool				use;							// 描画フラグ
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
