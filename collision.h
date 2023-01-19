//=============================================================================
//
// 当たり判定処理 [collision.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct gjkpoint {
	XMFLOAT3 supportPoints[4];

	int listSize = 0;
};

struct gjkList {
	XMFLOAT3* list;
	unsigned short length;
};

struct CapsuleHitBox {
	XMFLOAT3 positiona;
	XMFLOAT3 positionb;
	float radius;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh, XMFLOAT3 ypos, float yw, float yh);
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2);

float dotProduct(XMVECTOR *v1, XMVECTOR *v2);
float dotProductF3(XMFLOAT3 *v1, XMFLOAT3 *v2);


XMVECTOR FindFurthestPointFrom(XMVECTOR direction, XMFLOAT3* gjkList, unsigned short size);
XMVECTOR FindSupportPoint(XMVECTOR direction, XMFLOAT3* gjkListA, unsigned short aSize, XMFLOAT3* gjkListB, unsigned short bSize);

bool IsSameDir(XMVECTOR* v1, XMVECTOR* v2);
void PushPop(gjkpoint* points,XMVECTOR pos);

void crossProduct(XMVECTOR *ret, XMVECTOR *v1, XMVECTOR *v2);
XMFLOAT3 CrossProductF3(XMFLOAT3* v1, XMFLOAT3* v2);
bool RayCast(XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 pos0, XMFLOAT3 pos1, XMFLOAT3 *hit, XMFLOAT3 *normal);

bool GJKCollide(gjkpoint* support, XMVECTOR& dir);
bool GJKLineCheck(gjkpoint* support, XMVECTOR& dir);
bool GJKTriangleCheck(gjkpoint* support, XMVECTOR& dir);
bool GJKTetrahedronCheck(gjkpoint* support, XMVECTOR& dir);
bool GJKHit(XMFLOAT3* gjkListA, unsigned short aSize, XMFLOAT3* gjkListB, unsigned short bSize);

XMFLOAT3 AffineTransform(XMFLOAT3 pos, XMMATRIX mtxWorld);
void AppliedTransform(XMFLOAT3* target, short amount, XMFLOAT3 posArray[], XMMATRIX mtxWorld);
void UpdateHitbox(XMFLOAT3* hitboxList, short listSize, XMFLOAT3 vertexPositionList[], XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale);