//=============================================================================
//
// 当たり判定処理 [collision.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************


//=============================================================================
// BBによる当たり判定処理
// 回転は考慮しない
// 戻り値：当たってたらtrue
//=============================================================================
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh,
	XMFLOAT3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;	// 外れをセットしておく

	// 座標が中心点なので計算しやすく半分にしている
	mw /= 2;
	mh /= 2;
	yw /= 2;
	yh /= 2;

	// バウンディングボックス(BB)の処理
	if ((mpos.x + mw > ypos.x - yw) &&
		(mpos.x - mw < ypos.x + yw) &&
		(mpos.y + mh > ypos.y - yh) &&
		(mpos.y - mh < ypos.y + yh))
	{
		// 当たった時の処理
		ans = TRUE;
	}

	return ans;
}

//=============================================================================
// BCによる当たり判定処理
// サイズは半径
// 戻り値：当たってたらTRUE
//=============================================================================
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2)
{
	BOOL ans = FALSE;						// 外れをセットしておく

	float len = (r1 + r2) * (r1 + r2);		// 半径を2乗した物
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2点間の距離（2乗した物）
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);

	// 半径を2乗した物より距離が短い？
	if (len > lenSq)
	{
		ans = TRUE;	// 当たっている
	}

	return ans;
}


//=============================================================================
// 内積(dot)
//=============================================================================
float dotProduct(XMVECTOR *v1, XMVECTOR *v2)
{
#if 0
	float ans = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
#else
	// ダイレクトＸでは、、、
	XMVECTOR temp = XMVector3Dot(*v1, *v2);
	float ans = 0.0f;
	XMStoreFloat(&ans, temp);
#endif

	return(ans);
}

float dotProductF3(XMFLOAT3 *v1, XMFLOAT3 *v2)
{

	float ans = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;

	return(ans);
}

bool IsSameDir(XMVECTOR* v1, XMVECTOR* v2) {
	return dotProduct(v1, v2) > 0;
}


//=============================================================================
// 外積(cross)
//=============================================================================
void crossProduct(XMVECTOR *ret, XMVECTOR *v1, XMVECTOR *v2)
{
#if 0
	ret->x = v1->y * v2->z - v1->z * v2->y;
	ret->y = v1->z * v2->x - v1->x * v2->z;
	ret->z = v1->x * v2->y - v1->y * v2->x;
#else
	// ダイレクトＸでは、、、
	*ret = XMVector3Cross(*v1, *v2);
#endif

}

XMFLOAT3 CrossProductF3( XMFLOAT3* v1, XMFLOAT3* v2)
{
	XMFLOAT3 result;
	result.x = v1->y * v2->z - v1->z * v2->y;
	result.y = v1->z * v2->x - v1->x * v2->z;
	result.z = v1->x * v2->y - v1->y * v2->x;
	return result;


}


//=============================================================================
// レイキャスト
// p0, p1, p2　ポリゴンの３頂点
// pos0 始点
// pos1 終点
// hit　交点の返却用
// normal 法線ベクトルの返却用
// 当たっている場合、trueを返す
//=============================================================================
bool RayCast(XMFLOAT3 xp0, XMFLOAT3 xp1, XMFLOAT3 xp2, XMFLOAT3 xpos0, XMFLOAT3 xpos1, XMFLOAT3 *hit, XMFLOAT3 *normal)
{
	XMVECTOR	p0   = XMLoadFloat3(&xp0);
	XMVECTOR	p1   = XMLoadFloat3(&xp1);
	XMVECTOR	p2   = XMLoadFloat3(&xp2);
	XMVECTOR	pos0 = XMLoadFloat3(&xpos0);
	XMVECTOR	pos1 = XMLoadFloat3(&xpos1);

	XMVECTOR	nor;	// ポリゴンの法線
	XMVECTOR	vec1;
	XMVECTOR	vec2;
	float		d1, d2;

	{	// ポリゴンの外積をとって法線を求める(この処理は固定物なら予めInit()で行っておくと良い)
		vec1 = p1 - p0;
		vec2 = p2 - p0;
		crossProduct(&nor, &vec2, &vec1);
		nor = XMVector3Normalize(nor);		// 計算しやすいように法線をノーマライズしておく(このベクトルの長さを１にしている)
		XMStoreFloat3(normal, nor);			// 求めた法線を入れておく
	}

	// ポリゴン平面と線分の内積とって衝突している可能性を調べる（鋭角なら＋、鈍角ならー、直角なら０）
	vec1 = pos0 - p0;
	vec2 = pos1 - p0;
	{	// 求めたポリゴンの法線と２つのベクトル（線分の両端とポリゴン上の任意の点）の内積とって衝突している可能性を調べる
		d1 = dotProduct(&vec1, &nor);
		d2 = dotProduct(&vec2, &nor);
		if (((d1 * d2) > 0.0f) || (d1 == 0 && d2 == 0))
		{
			// 当たっている可能性は無い
			return(false);
		}
	}


	{	// ポリゴンと線分の交点を求める
		d1 = (float)fabs(d1);	// 絶対値を求めている
		d2 = (float)fabs(d2);	// 絶対値を求めている
		float a = d1 / (d1 + d2);							// 内分比

		XMVECTOR	vec3 = (1 - a) * vec1 + a * vec2;		// p0から交点へのベクトル
		XMVECTOR	p3 = p0 + vec3;							// 交点
		XMStoreFloat3(hit, p3);								// 求めた交点を入れておく

		{	// 求めた交点がポリゴンの中にあるか調べる

			// ポリゴンの各辺のベクトル
			XMVECTOR	v1 = p1 - p0;
			XMVECTOR	v2 = p2 - p1;
			XMVECTOR	v3 = p0 - p2;

			// 各頂点と交点とのベクトル
			XMVECTOR	v4 = p3 - p1;
			XMVECTOR	v5 = p3 - p2;
			XMVECTOR	v6 = p3 - p0;

			// 外積で各辺の法線を求めて、ポリゴンの法線との内積をとって符号をチェックする
			XMVECTOR	n1, n2, n3;

			crossProduct(&n1, &v4, &v1);
			if (dotProduct(&n1, &nor) < 0.0f) return(false);	// 当たっていない

			crossProduct(&n2, &v5, &v2);
			if (dotProduct(&n2, &nor) < 0.0f) return(false);	// 当たっていない
			
			crossProduct(&n3, &v6, &v3);
			if (dotProduct(&n3, &nor) < 0.0f) return(false);	// 当たっていない
		}
	}

	return(true);	// 当たっている！(hitには当たっている交点が入っている。normalには法線が入っている)
}

//=============================================================================
// ギルバート・ジョンソン・キールティ距離アルゴリズム
// 
// 当たっている場合、trueを返す
//=============================================================================

//ある方向に一番遠い頂点を見つける関数
XMVECTOR FindFurthestPointFrom(XMVECTOR direction, XMFLOAT3* gjkList , unsigned short size) {
	XMVECTOR furthestPoint;			//一番遠い頂点の保管場所
	float maxDistance = -FLT_MAX;	//一番遠い頂点への距離

	//ポリゴンの頂点全てをループ
	for (int i = 0; i < size; i++) {
		XMVECTOR vector = XMLoadFloat3(&gjkList[i]);
		float distance = dotProduct(&vector ,&direction);

		//現在一番遠い点より遠いのであれば更新
		if (distance > maxDistance) {
			maxDistance = distance;
			furthestPoint = vector;
		}
	}

	return furthestPoint;
}

//=============================================================================
// ミンコスキー差：
// ポリゴンAの頂点からポリゴンBの頂点を全て引くことで
// 新たなポリゴンCを形成する。
// このポリゴンCは全ての方向に一番遠い点（サポートポイントと呼ぶ）
// を結ぶことでできている。
// そして、この頂点はAからBを引いたため、このポリゴン内に原点が含まれると
// AとBは当たっていることとなる。
// 当たっている場合、trueを返す
//=============================================================================

//=============================================================================
// サポートポイントの計算方法
// サポートポイントはある方向の極地にあるため
// ある方向に一番遠いポリゴンAの頂点から
// ある方向に一番近いポリゴンBの頂点
// （ある方向の逆方向に遠いポリゴンBの頂点）を
// 引けば求めることが可能
//=============================================================================
XMVECTOR FindSupportPoint(XMVECTOR direction, XMFLOAT3* gjkListA,unsigned short aSize, XMFLOAT3* gjkListB, unsigned short bSize) {
	XMVECTOR result = FindFurthestPointFrom(direction, gjkListA, aSize);
	XMVECTOR sub = FindFurthestPointFrom(-direction, gjkListB, bSize);

	result -= sub;

	return result;
}

void PushPop(gjkpoint* points,XMVECTOR pos) {
	points->supportPoints[3] = points->supportPoints[2];
	points->supportPoints[2] = points->supportPoints[1];
	points->supportPoints[1] = points->supportPoints[0];
	XMStoreFloat3(&points->supportPoints[0],pos);
	points->listSize ++;
	if (points->listSize > 4)points->listSize = 4;
}

void Pop(gjkpoint* points, int idx) {
	for (int i = idx; i < 2; i++) {
		points->supportPoints[i] = points->supportPoints[i+1];
	}
	points->supportPoints[3] = XMFLOAT3(NULL,NULL,NULL);
	points->listSize--;
}

//=============================================================================
// ギルバート・ジョンソン・キールティ距離アルゴリズム
// 
// 当たっている場合、trueを返す
//=============================================================================
bool GJKHit(XMFLOAT3* gjkListA, unsigned short aSize, XMFLOAT3* gjkListB, unsigned short bSize)
{
	gjkpoint points;
	points.listSize = 0;				//リストの初期化

	//初期方向（更新されるため、どの方向でもいい）
	XMVECTOR direction = XMLoadFloat3(new XMFLOAT3(0.0f, 0.0f, 1.0f));
	XMVECTOR supportPoint = FindSupportPoint(direction, gjkListA, aSize, gjkListB, bSize);

	PushPop(&points, supportPoint);		//一番目のサポートポイントを保存

	//方向をサポートポイントの逆方向に修正
	//（ミンコスキー差に原点を含みたいため逆方向にする）
	direction = -supportPoint;			
	int loop = 0;
	while (true) {
		supportPoint = FindSupportPoint(direction, gjkListA, aSize, gjkListB, bSize);

		// 新たなサポートポイントが、新の方向と90度以上違うのであれば
		// 原点は含まれないため偽を返答
		float cross = dotProduct(&supportPoint, &direction);
		if (dotProduct(&supportPoint, &direction) <= 0 ) 
		{
			return false;
		}

		// 当たっている可能性がある為、頂点を保存
		PushPop(&points, supportPoint);

		// n次元シンプレクスができたため次の点の可能性を計算
		if (GJKCollide(&points, direction)) {
			return true;
		}
		loop++;
	}
	
	//当たらなかった
	return false;

}

//=============================================================================
// n次元シンプレクスができたため次の点の可能性を計算
// 2　一次元シンプレックスチェック
// 3　二次元シンプレックスチェック
// 4　三次元シンプレックスチェック
//=============================================================================
bool GJKCollide(gjkpoint* support, XMVECTOR& dir) {
	switch (support->listSize) {
	case 2:  return GJKLineCheck		(support, dir);
	case 3:	 return GJKTriangleCheck	(support, dir);
	case 4:	 return GJKTetrahedronCheck	(support, dir);
	default: return false;
	}
}
//=============================================================================
// 一次元シンプレックスチェック
//=============================================================================
bool GJKLineCheck(gjkpoint* support, XMVECTOR& dir) {
	XMVECTOR pointa = XMLoadFloat3(&support->supportPoints[0]);
	XMVECTOR pointb = XMLoadFloat3(&support->supportPoints[1]);
	  
	XMVECTOR vectorab = pointb - pointa;
	XMVECTOR vectorao = - pointa;
	
	// 頂点bから頂点aを求めたため原点はvectorbaと同じ（90度以内）方向である
	// この条件でベクトルaoとベクトルabが同じ方向であれば
	// 新たな点を探す（三次元の為外積を使い方向を確定する）
	if (IsSameDir(&vectorab, &vectorao)) {
		dir = XMVector3Cross(XMVector3Cross(vectorab, vectorao), vectorab);
	}
	// そうでない場合、この二点では判断できないため頂点bを削除
	// aoの方向に新たな点を探す
	else {
		Pop(support, 1);
		dir = vectorao;
	}

	//当然当たっていないため繰り返す
	return false;
}

//=============================================================================
// 二次元シンプレックスチェック
//=============================================================================
bool GJKTriangleCheck(gjkpoint* support, XMVECTOR& dir) {

	//三角形の場合原点は七か所に存在することができる
	//そのうち３か所は新たに加えた頂点aと同じ方向に無い為
	//面bcの方向には存在できない
	XMVECTOR pointa = XMLoadFloat3(&support->supportPoints[0]);
	XMVECTOR pointb = XMLoadFloat3(&support->supportPoints[1]);
	XMVECTOR pointc = XMLoadFloat3(&support->supportPoints[2]);

	XMVECTOR vectorab = pointb - pointa;
	XMVECTOR vectorac = pointc - pointa;
	XMVECTOR vectorao = -pointa;

	//ベクトルab,acに90度を成すベクトルabc
	XMVECTOR vectorabc = XMVector3Cross(vectorab, vectorac);

	//原点は三角形abcからac面の方向に三角形abc外にあるのか？
	if (IsSameDir(&XMVector3Cross(vectorabc, vectorac),&vectorao )) {

		//かつaoベクトルはacベクトルと同じ方向か？
		if (IsSameDir(&vectorac, &vectorao)) {
			//点bは面acの反対にあるか調べなければいけない
			//点bは点cとなり
			//新しく方向を設定
			XMStoreFloat3(&support->supportPoints[1], pointc);
			Pop(support, 2);
			dir = XMVector3Cross(XMVector3Cross(vectorac, vectorao), vectorac);
		}

		else {
			// 点cが求めている方向にいないため、
			// 削除＆再索敵
			Pop(support, 2);
			return GJKLineCheck(support, dir);
		}
	}

	else
	{
		// 同じくaoとabで調べる
		if (IsSameDir(&XMVector3Cross(vectorab, vectorabc), &vectorao)) {
			Pop(support, 2);
			return GJKLineCheck(support, dir);
		}

		else {
			//三角形abc内にある、あとはベクトルabcと同じ方向にあれば続ける
			if (IsSameDir(&vectorabc, &vectorao)) {
				dir = vectorabc;
			}
			//そうでなければ、方向を逆にして、点bとcの位置を入れ替え続ける
			else {
				XMStoreFloat3(&support->supportPoints[2], pointb);
				XMStoreFloat3(&support->supportPoints[1], pointc);
				dir = -vectorabc;
			}
		}
	}

	return false;
}

//=============================================================================
// 三次元シンプレックスチェック
//=============================================================================
bool GJKTetrahedronCheck(gjkpoint* support, XMVECTOR& dir) {
	XMVECTOR pointa = XMLoadFloat3(&support->supportPoints[0]);
	XMVECTOR pointb = XMLoadFloat3(&support->supportPoints[1]);
	XMVECTOR pointc = XMLoadFloat3(&support->supportPoints[2]);
	XMVECTOR pointd = XMLoadFloat3(&support->supportPoints[3]);

	XMVECTOR vectorab = pointb - pointa;
	XMVECTOR vectorac = pointc - pointa;
	XMVECTOR vectorad = pointd - pointa;
	XMVECTOR vectorao = -pointa;

	XMVECTOR vectorabc = XMVector3Cross(vectorab, vectorac);
	XMVECTOR vectoracd = XMVector3Cross(vectorac, vectorad);
	XMVECTOR vectoradb = XMVector3Cross(vectorad, vectorab);

	if (IsSameDir(&vectorabc, &vectorao)) {
		support->supportPoints[3] = XMFLOAT3(0.0f, 0.0f, 0.0f);
		support->listSize--;
		return GJKTriangleCheck(support, dir);
	}

	if (IsSameDir(&vectoracd, &vectorao)) {
		XMStoreFloat3(&support->supportPoints[1], pointc);
		XMStoreFloat3(&support->supportPoints[2], pointd);
		support->supportPoints[3] = XMFLOAT3(0.0f, 0.0f, 0.0f);
		support->listSize--;
		return GJKTriangleCheck(support, dir);
	}

	if (IsSameDir(&vectoradb, &vectorao)) {
		XMStoreFloat3(&support->supportPoints[1], pointd);
		XMStoreFloat3(&support->supportPoints[2], pointb);
		support->supportPoints[3] = XMFLOAT3(0.0f, 0.0f, 0.0f);
		support->listSize--;
		return GJKTriangleCheck(support, dir);
	}

	return true;
}
//=============================================================================


XMFLOAT3 AffineTransform(XMFLOAT3 pos, XMMATRIX mtxWorld) {
	XMFLOAT3 result = pos;
	XMVECTOR vectorPos = XMLoadFloat3(&result);
	vectorPos = XMVector3Transform(vectorPos, mtxWorld);
	XMStoreFloat3(&result, vectorPos);
	return result;
	
}

void AppliedTransform(XMFLOAT3* target,short amount, XMFLOAT3 posArray[], XMMATRIX mtxWorld) {
	for (int i = 0; i < amount; i++) {
		target[i] = AffineTransform(posArray[i], mtxWorld);
	}
}


void UpdateHitbox(XMFLOAT3* hitboxList, short listSize, XMFLOAT3 vertexPositionList[], XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale) {
	// 当たり判定用の変数を更新
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(scale.x, scale.y, scale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(position.x, position.y, position.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	AppliedTransform(hitboxList, listSize, vertexPositionList, mtxWorld);
}

