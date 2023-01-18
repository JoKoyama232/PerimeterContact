//=============================================================================
//
// �����蔻�菈�� [collision.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************


//=============================================================================
// BB�ɂ�铖���蔻�菈��
// ��]�͍l�����Ȃ�
// �߂�l�F�������Ă���true
//=============================================================================
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh,
	XMFLOAT3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;	// �O����Z�b�g���Ă���

	// ���W�����S�_�Ȃ̂Ōv�Z���₷�������ɂ��Ă���
	mw /= 2;
	mh /= 2;
	yw /= 2;
	yh /= 2;

	// �o�E���f�B���O�{�b�N�X(BB)�̏���
	if ((mpos.x + mw > ypos.x - yw) &&
		(mpos.x - mw < ypos.x + yw) &&
		(mpos.y + mh > ypos.y - yh) &&
		(mpos.y - mh < ypos.y + yh))
	{
		// �����������̏���
		ans = TRUE;
	}

	return ans;
}

//=============================================================================
// BC�ɂ�铖���蔻�菈��
// �T�C�Y�͔��a
// �߂�l�F�������Ă���TRUE
//=============================================================================
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2)
{
	BOOL ans = FALSE;						// �O����Z�b�g���Ă���

	float len = (r1 + r2) * (r1 + r2);		// ���a��2�悵����
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2�_�Ԃ̋����i2�悵�����j
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);

	// ���a��2�悵������苗�����Z���H
	if (len > lenSq)
	{
		ans = TRUE;	// �������Ă���
	}

	return ans;
}


//=============================================================================
// ����(dot)
//=============================================================================
float dotProduct(XMVECTOR *v1, XMVECTOR *v2)
{
#if 0
	float ans = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
#else
	// �_�C���N�g�w�ł́A�A�A
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
// �O��(cross)
//=============================================================================
void crossProduct(XMVECTOR *ret, XMVECTOR *v1, XMVECTOR *v2)
{
#if 0
	ret->x = v1->y * v2->z - v1->z * v2->y;
	ret->y = v1->z * v2->x - v1->x * v2->z;
	ret->z = v1->x * v2->y - v1->y * v2->x;
#else
	// �_�C���N�g�w�ł́A�A�A
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
// ���C�L���X�g
// p0, p1, p2�@�|���S���̂R���_
// pos0 �n�_
// pos1 �I�_
// hit�@��_�̕ԋp�p
// normal �@���x�N�g���̕ԋp�p
// �������Ă���ꍇ�Atrue��Ԃ�
//=============================================================================
bool RayCast(XMFLOAT3 xp0, XMFLOAT3 xp1, XMFLOAT3 xp2, XMFLOAT3 xpos0, XMFLOAT3 xpos1, XMFLOAT3 *hit, XMFLOAT3 *normal)
{
	XMVECTOR	p0   = XMLoadFloat3(&xp0);
	XMVECTOR	p1   = XMLoadFloat3(&xp1);
	XMVECTOR	p2   = XMLoadFloat3(&xp2);
	XMVECTOR	pos0 = XMLoadFloat3(&xpos0);
	XMVECTOR	pos1 = XMLoadFloat3(&xpos1);

	XMVECTOR	nor;	// �|���S���̖@��
	XMVECTOR	vec1;
	XMVECTOR	vec2;
	float		d1, d2;

	{	// �|���S���̊O�ς��Ƃ��Ė@�������߂�(���̏����͌Œ蕨�Ȃ�\��Init()�ōs���Ă����Ɨǂ�)
		vec1 = p1 - p0;
		vec2 = p2 - p0;
		crossProduct(&nor, &vec2, &vec1);
		nor = XMVector3Normalize(nor);		// �v�Z���₷���悤�ɖ@�����m�[�}���C�Y���Ă���(���̃x�N�g���̒������P�ɂ��Ă���)
		XMStoreFloat3(normal, nor);			// ���߂��@�������Ă���
	}

	// �|���S�����ʂƐ����̓��ςƂ��ďՓ˂��Ă���\���𒲂ׂ�i�s�p�Ȃ�{�A�݊p�Ȃ�[�A���p�Ȃ�O�j
	vec1 = pos0 - p0;
	vec2 = pos1 - p0;
	{	// ���߂��|���S���̖@���ƂQ�̃x�N�g���i�����̗��[�ƃ|���S����̔C�ӂ̓_�j�̓��ςƂ��ďՓ˂��Ă���\���𒲂ׂ�
		d1 = dotProduct(&vec1, &nor);
		d2 = dotProduct(&vec2, &nor);
		if (((d1 * d2) > 0.0f) || (d1 == 0 && d2 == 0))
		{
			// �������Ă���\���͖���
			return(false);
		}
	}


	{	// �|���S���Ɛ����̌�_�����߂�
		d1 = (float)fabs(d1);	// ��Βl�����߂Ă���
		d2 = (float)fabs(d2);	// ��Βl�����߂Ă���
		float a = d1 / (d1 + d2);							// ������

		XMVECTOR	vec3 = (1 - a) * vec1 + a * vec2;		// p0�����_�ւ̃x�N�g��
		XMVECTOR	p3 = p0 + vec3;							// ��_
		XMStoreFloat3(hit, p3);								// ���߂���_�����Ă���

		{	// ���߂���_���|���S���̒��ɂ��邩���ׂ�

			// �|���S���̊e�ӂ̃x�N�g��
			XMVECTOR	v1 = p1 - p0;
			XMVECTOR	v2 = p2 - p1;
			XMVECTOR	v3 = p0 - p2;

			// �e���_�ƌ�_�Ƃ̃x�N�g��
			XMVECTOR	v4 = p3 - p1;
			XMVECTOR	v5 = p3 - p2;
			XMVECTOR	v6 = p3 - p0;

			// �O�ςŊe�ӂ̖@�������߂āA�|���S���̖@���Ƃ̓��ς��Ƃ��ĕ������`�F�b�N����
			XMVECTOR	n1, n2, n3;

			crossProduct(&n1, &v4, &v1);
			if (dotProduct(&n1, &nor) < 0.0f) return(false);	// �������Ă��Ȃ�

			crossProduct(&n2, &v5, &v2);
			if (dotProduct(&n2, &nor) < 0.0f) return(false);	// �������Ă��Ȃ�
			
			crossProduct(&n3, &v6, &v3);
			if (dotProduct(&n3, &nor) < 0.0f) return(false);	// �������Ă��Ȃ�
		}
	}

	return(true);	// �������Ă���I(hit�ɂ͓������Ă����_�������Ă���Bnormal�ɂ͖@���������Ă���)
}

//=============================================================================
// �M���o�[�g�E�W�����\���E�L�[���e�B�����A���S���Y��
// 
// �������Ă���ꍇ�Atrue��Ԃ�
//=============================================================================

//��������Ɉ�ԉ������_��������֐�
XMVECTOR FindFurthestPointFrom(XMVECTOR direction, XMFLOAT3* gjkList , unsigned short size) {
	XMVECTOR furthestPoint;			//��ԉ������_�̕ۊǏꏊ
	float maxDistance = -FLT_MAX;	//��ԉ������_�ւ̋���

	//�|���S���̒��_�S�Ă����[�v
	for (int i = 0; i < size; i++) {
		XMVECTOR vector = XMLoadFloat3(&gjkList[i]);
		float distance = dotProduct(&vector ,&direction);

		//���݈�ԉ����_��艓���̂ł���΍X�V
		if (distance > maxDistance) {
			maxDistance = distance;
			furthestPoint = vector;
		}
	}

	return furthestPoint;
}

//=============================================================================
// �~���R�X�L�[���F
// �|���S��A�̒��_����|���S��B�̒��_��S�Ĉ������Ƃ�
// �V���ȃ|���S��C���`������B
// ���̃|���S��C�͑S�Ă̕����Ɉ�ԉ����_�i�T�|�[�g�|�C���g�ƌĂԁj
// �����Ԃ��Ƃłł��Ă���B
// �����āA���̒��_��A����B�����������߁A���̃|���S�����Ɍ��_���܂܂���
// A��B�͓������Ă��邱�ƂƂȂ�B
// �������Ă���ꍇ�Atrue��Ԃ�
//=============================================================================

//=============================================================================
// �T�|�[�g�|�C���g�̌v�Z���@
// �T�|�[�g�|�C���g�͂�������̋ɒn�ɂ��邽��
// ��������Ɉ�ԉ����|���S��A�̒��_����
// ��������Ɉ�ԋ߂��|���S��B�̒��_
// �i��������̋t�����ɉ����|���S��B�̒��_�j��
// �����΋��߂邱�Ƃ��\
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
// �M���o�[�g�E�W�����\���E�L�[���e�B�����A���S���Y��
// 
// �������Ă���ꍇ�Atrue��Ԃ�
//=============================================================================
bool GJKHit(XMFLOAT3* gjkListA, unsigned short aSize, XMFLOAT3* gjkListB, unsigned short bSize)
{
	gjkpoint points;
	points.listSize = 0;				//���X�g�̏�����

	//���������i�X�V����邽�߁A�ǂ̕����ł������j
	XMVECTOR direction = XMLoadFloat3(new XMFLOAT3(0.0f, 0.0f, 1.0f));
	XMVECTOR supportPoint = FindSupportPoint(direction, gjkListA, aSize, gjkListB, bSize);

	PushPop(&points, supportPoint);		//��Ԗڂ̃T�|�[�g�|�C���g��ۑ�

	//�������T�|�[�g�|�C���g�̋t�����ɏC��
	//�i�~���R�X�L�[���Ɍ��_���܂݂������ߋt�����ɂ���j
	direction = -supportPoint;			
	int loop = 0;
	while (true) {
		supportPoint = FindSupportPoint(direction, gjkListA, aSize, gjkListB, bSize);

		// �V���ȃT�|�[�g�|�C���g���A�V�̕�����90�x�ȏ�Ⴄ�̂ł����
		// ���_�͊܂܂�Ȃ����ߋU��ԓ�
		float cross = dotProduct(&supportPoint, &direction);
		if (dotProduct(&supportPoint, &direction) <= 0 ) 
		{
			return false;
		}

		// �������Ă���\��������ׁA���_��ۑ�
		PushPop(&points, supportPoint);

		// n�����V���v���N�X���ł������ߎ��̓_�̉\�����v�Z
		if (GJKCollide(&points, direction)) {
			return true;
		}
		loop++;
	}
	
	//������Ȃ�����
	return false;

}

//=============================================================================
// n�����V���v���N�X���ł������ߎ��̓_�̉\�����v�Z
// 2�@�ꎟ���V���v���b�N�X�`�F�b�N
// 3�@�񎟌��V���v���b�N�X�`�F�b�N
// 4�@�O�����V���v���b�N�X�`�F�b�N
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
// �ꎟ���V���v���b�N�X�`�F�b�N
//=============================================================================
bool GJKLineCheck(gjkpoint* support, XMVECTOR& dir) {
	XMVECTOR pointa = XMLoadFloat3(&support->supportPoints[0]);
	XMVECTOR pointb = XMLoadFloat3(&support->supportPoints[1]);
	  
	XMVECTOR vectorab = pointb - pointa;
	XMVECTOR vectorao = - pointa;
	
	// ���_b���璸�_a�����߂����ߌ��_��vectorba�Ɠ����i90�x�ȓ��j�����ł���
	// ���̏����Ńx�N�g��ao�ƃx�N�g��ab�����������ł����
	// �V���ȓ_��T���i�O�����̈׊O�ς��g���������m�肷��j
	if (IsSameDir(&vectorab, &vectorao)) {
		dir = XMVector3Cross(XMVector3Cross(vectorab, vectorao), vectorab);
	}
	// �����łȂ��ꍇ�A���̓�_�ł͔��f�ł��Ȃ����ߒ��_b���폜
	// ao�̕����ɐV���ȓ_��T��
	else {
		Pop(support, 1);
		dir = vectorao;
	}

	//���R�������Ă��Ȃ����ߌJ��Ԃ�
	return false;
}

//=============================================================================
// �񎟌��V���v���b�N�X�`�F�b�N
//=============================================================================
bool GJKTriangleCheck(gjkpoint* support, XMVECTOR& dir) {

	//�O�p�`�̏ꍇ���_�͎������ɑ��݂��邱�Ƃ��ł���
	//���̂����R�����͐V���ɉ��������_a�Ɠ��������ɖ�����
	//��bc�̕����ɂ͑��݂ł��Ȃ�
	XMVECTOR pointa = XMLoadFloat3(&support->supportPoints[0]);
	XMVECTOR pointb = XMLoadFloat3(&support->supportPoints[1]);
	XMVECTOR pointc = XMLoadFloat3(&support->supportPoints[2]);

	XMVECTOR vectorab = pointb - pointa;
	XMVECTOR vectorac = pointc - pointa;
	XMVECTOR vectorao = -pointa;

	//�x�N�g��ab,ac��90�x�𐬂��x�N�g��abc
	XMVECTOR vectorabc = XMVector3Cross(vectorab, vectorac);

	//���_�͎O�p�`abc����ac�ʂ̕����ɎO�p�`abc�O�ɂ���̂��H
	if (IsSameDir(&XMVector3Cross(vectorabc, vectorac),&vectorao )) {

		//����ao�x�N�g����ac�x�N�g���Ɠ����������H
		if (IsSameDir(&vectorac, &vectorao)) {
			//�_b�͖�ac�̔��΂ɂ��邩���ׂȂ���΂����Ȃ�
			//�_b�͓_c�ƂȂ�
			//�V����������ݒ�
			XMStoreFloat3(&support->supportPoints[1], pointc);
			Pop(support, 2);
			dir = XMVector3Cross(XMVector3Cross(vectorac, vectorao), vectorac);
		}

		else {
			// �_c�����߂Ă�������ɂ��Ȃ����߁A
			// �폜���č��G
			Pop(support, 2);
			return GJKLineCheck(support, dir);
		}
	}

	else
	{
		// ������ao��ab�Œ��ׂ�
		if (IsSameDir(&XMVector3Cross(vectorab, vectorabc), &vectorao)) {
			Pop(support, 2);
			return GJKLineCheck(support, dir);
		}

		else {
			//�O�p�`abc���ɂ���A���Ƃ̓x�N�g��abc�Ɠ��������ɂ���Α�����
			if (IsSameDir(&vectorabc, &vectorao)) {
				dir = vectorabc;
			}
			//�����łȂ���΁A�������t�ɂ��āA�_b��c�̈ʒu�����ւ�������
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
// �O�����V���v���b�N�X�`�F�b�N
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
	// �����蔻��p�̕ϐ����X�V
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(scale.x, scale.y, scale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(position.x, position.y, position.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	AppliedTransform(hitboxList, listSize, vertexPositionList, mtxWorld);
}

