//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : 小山　城
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER		(1)					// プレイヤーの数
#define	PLAYER_SIZE		(5.0f)				// 当たり判定の大きさ

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PLAYER
{
	XMFLOAT3			pos;		// ポリゴンの位置
	XMFLOAT3			rot;		// ポリゴンの向き(回転)
	XMFLOAT3			scl;		// ポリゴンの大きさ(スケール)

	XMFLOAT4X4			mtxWorld;	// ワールドマトリックス

	bool				load;
	DX11_MODEL			model;		// モデル情報
	VERTEX_DATA			points;
	CAPSULEHITBOX		hitbox;
	float				spd;		// 移動スピード
	float				dir;		// 向き
	float				size;		// 当たり判定の大きさ
	int					shadowIdx;	// 影のIndex
	bool				use;

	// 階層アニメーション用のメンバー変数
	float				time;				// 線形補間用
	int					tblNo;				// 行動データのテーブル番号
	int					tblMax;				// そのテーブルのデータ数
	int					hpMax;
	int					hpCurrent;

	// 親は、NULL、子供は親のアドレスを入れる(セーブ＆ロードの時は↓ここ気をつける事)
	PLAYER				*attachedTo;	// 自分が親ならNULL、自分が子供なら親のplayerアドレス


};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);
XMFLOAT3 GetLerp(XMFLOAT3 initialPos, XMFLOAT3 endingPos, float percentage);
float GetDistance3D(XMFLOAT3 pos1, XMFLOAT3 pos2);
RASTERIZER_MODE GetRasterizerMode(void);