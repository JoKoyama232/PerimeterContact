//=============================================================================
//
// �e���ˏ��� [bullet.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "shadow.h"
#include "bullet.h"
#include "collision.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�

#define	BULLET_WIDTH		(20.0f)			// ���_�T�C�Y
#define	BULLET_HEIGHT		(20.0f)			// ���_�T�C�Y

#define	BULLET_SPEED		(5.0f)			// �e�̈ړ��X�s�[�h


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexBullet(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static BULLET						g_Bullet[MAX_BULLET];	// �؃��[�N
static int							g_TexNo;				// �e�N�X�`���ԍ�

static char *g_TextureName[] =
{
	"data/TEXTURE/bullet00.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
{
	MakeVertexBullet();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// �e���[�N�̏�����
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		ZeroMemory(&g_Bullet[nCntBullet].material, sizeof(g_Bullet[nCntBullet].material));
		g_Bullet[nCntBullet].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_Bullet[nCntBullet].pos = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].rot = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].targetPos = { 1.0f, 1.0f, 1.0f };
		g_Bullet[nCntBullet].fWidth = BULLET_WIDTH;
		g_Bullet[nCntBullet].fHeight = BULLET_HEIGHT;
		g_Bullet[nCntBullet].use = false;

	}


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBullet(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBullet(void)
{

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
			DWORD deltaTimeMs = timeGetTime() - g_Bullet[i].timer;
			g_Bullet[i].timer = timeGetTime();
			if (g_Bullet[i].progress < 0.9f) {
				g_Bullet[i].targetPos = { g_Bullet[i].target->x, g_Bullet[i].target->y, g_Bullet[i].target->z };
			}

			g_Bullet[i].pos = GetBezier(g_Bullet[i].startPos, g_Bullet[i].middlePos, g_Bullet[i].targetPos, g_Bullet[i].progress);

			if(g_Bullet[i].progress > 0.5f)g_Bullet[i].pos.y += BULLET_WIDTH * 0.5f*sinf(g_Bullet[i].progress * 8 *XM_PI);
			g_Bullet[i].progress +=  0.001f;

			if (g_Bullet[i].progress > 1.0f) 
			{ 
				g_Bullet[i].use = false; 
				ReleaseShadow(g_Bullet[i].shadowIdx);
			}
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_Bullet[i].shadowIdx, XMFLOAT3(g_Bullet[i].pos.x, 0.1f, g_Bullet[i].pos.z));


			// �t�B�[���h�̊O�ɏo����e����������
			if (g_Bullet[i].pos.x < MAP_LEFT
				|| g_Bullet[i].pos.x > MAP_RIGHT
				|| g_Bullet[i].pos.z < MAP_DOWN
				|| g_Bullet[i].pos.z > MAP_TOP)
			{
				g_Bullet[i].use = false;
				ReleaseShadow(g_Bullet[i].shadowIdx);
			}

		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBullet(void)
{
	// ���C�e�B���O�𖳌�
	SetLightEnable(false);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Bullet[i].rot.x, g_Bullet[i].rot.y + XM_PI, g_Bullet[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Bullet[i].pos.x, g_Bullet[i].pos.y, g_Bullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Bullet[i].mtxWorld, mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Bullet[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(true);

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexBullet(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = BULLET_WIDTH;
	float fHeight = BULLET_HEIGHT;

	// ���_���W�̐ݒ�
	vertex[0].Position = { -fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[1].Position = {  fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[2].Position = { -fWidth / 2.0f, 0.0f, -fHeight / 2.0f };
	vertex[3].Position = {  fWidth / 2.0f, 0.0f, -fHeight / 2.0f };

	// �g�U���̐ݒ�
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �e�̃p�����[�^���Z�b�g
//=============================================================================
int SetBullet(XMFLOAT3 initialPos, XMFLOAT3 *targetPos,float speed)
{
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		if (!g_Bullet[nCntBullet].use)
		{
			g_Bullet[nCntBullet].startPos = initialPos;
			g_Bullet[nCntBullet].target = targetPos;
			g_Bullet[nCntBullet].timer = timeGetTime();
			XMVECTOR base = XMLoadFloat3(&initialPos);
			XMFLOAT3 direction = { (g_Bullet[nCntBullet].target->x - initialPos.x),0.0f,(g_Bullet[nCntBullet].target->y- initialPos.x) };
			XMVECTOR normalDirectionVector = XMVector3Normalize( XMLoadFloat3(&direction));
			XMVECTOR up = { (0.0f,1.0f,0.0f) };
			XMVECTOR side;
			crossProduct(&side, &normalDirectionVector, &up);
			XMVECTOR middlePoint = base - 200 * normalDirectionVector + (rand() % 10 - 5) * side;
			XMStoreFloat3(&g_Bullet[nCntBullet].middlePos, middlePoint);
			g_Bullet[nCntBullet].middlePos.y = g_Bullet[nCntBullet].target->y + 100.0f;
			g_Bullet[nCntBullet].progress = 0.0f;
			g_Bullet[nCntBullet].use = true;

			// �e�̐ݒ�
			g_Bullet[nCntBullet].shadowIdx = CreateShadow(g_Bullet[nCntBullet].pos, 0.25f, 0.25f);

			nIdxBullet = nCntBullet;

		//	PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxBullet;
}

//=============================================================================
// �e�̎擾
//=============================================================================
BULLET *GetBullet(void)
{
	return &(g_Bullet[0]);
}

XMFLOAT3 GetBezier(XMFLOAT3 initialPos, XMFLOAT3 middlePos, XMFLOAT3 endPos, float percentage) {
	XMFLOAT3 result = initialPos;
	float invP = 1 - percentage;
	result.x = invP * invP * initialPos.x + invP * 2 * percentage * middlePos.x + percentage * percentage * endPos.x;
	result.y = invP * invP * initialPos.y + invP * 2 * percentage * middlePos.y + percentage * percentage * endPos.y;
	result.z = invP * invP * initialPos.z + invP * 2 * percentage * middlePos.z + percentage * percentage * endPos.z;
	return result;

}

