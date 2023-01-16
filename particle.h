//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 acceleration, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, bool shadow, int textureID, int nLife);
void SetColorParticle(int nIdxParticle, XMFLOAT4 col);

