//=============================================================================
//
// hud処理 [hud.h]
// Author : 小山　城
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct HUD
{
	XMFLOAT2	pos;			// テクスチャの位置
	XMFLOAT2	size;			// テクスチャの大きさ
	XMFLOAT2	texturePos;		// テクスチャの画像位置
	XMFLOAT2	texturePercent;	// テクスチャの画像の大きさ
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitHud(void);
void UninitHud(void);
void UpdateHud(void);
void DrawHud(void);
