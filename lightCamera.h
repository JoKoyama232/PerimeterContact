// 光カメラ処理 [lightCamera.h]
//制作者：小山　城
//主な役割：シャドウマップ
#pragma once


//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct CAMERA
{
	XMFLOAT4X4			mtxView;		// ビューマトリックス
	XMFLOAT4X4			mtxInvView;		// ビューマトリックス
	XMFLOAT4X4			mtxProjection;	// プロジェクションマトリックス

	XMFLOAT3			eye;			// カメラの視点(位置)
	XMFLOAT3			at;				// カメラの注視点
	XMFLOAT3			up;				// カメラの上方向ベクトル

	float				len;			// カメラの視点と注視点の距離

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitLightCamera(void);
void UninitLightCamera(void);
void UpdateLightCamera(void);
void SetLightCamera(void);

CAMERA* GetLightCamera(void);

void SetViewPort(int type);
int GetViewPortType(void);

void SetLightCameraAT(XMFLOAT3 pos);
