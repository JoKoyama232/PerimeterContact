//=============================================================================
//
// ���_�\������ [point.cpp]
// Author :	���R�@��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "model.h"
#include "light.h"
#include "player.h"
#include "enemy.h"
#include "point.h"
#include "collision.h"
#include "input.h"
#include "debugproc.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define	PARTICLE_SIZE_X		(1.0f)		// ���_�T�C�Y
#define	PARTICLE_SIZE_Y		(1.0f)		// ���_�T�C�Y

#define	MAX_POINTS		(512)		// �p�[�e�B�N���ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		scale;			// �X�P�[��
	MATERIAL		material;		// �}�e���A��
	bool			bUse;			// �g�p���Ă��邩�ǂ���

} VERTEXINFO;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexPoints(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView* g_Texture[1] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;					// �e�N�X�`���ԍ�
static int							g_pointidx;
static VERTEXINFO					g_PlayerModelVerticies[MAX_POINTS];		// �p�[�e�B�N�����[�N
static VERTEXINFO					g_EnemyModelVerticies[MAX_POINTS];		// �p�[�e�B�N�����[�N

static char* g_TextureName[] =
{
	"data/TEXTURE/effect000.jpg",
};

static BOOL							g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPoint(void)
{
	// ���_���̍쐬
	MakeVertexPoints();

	// �e�N�X�`������
	for (int i = 0; i < 1; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;
	g_pointidx = 0;
	PLAYER* player = GetPlayer();
	// �p�[�e�B�N�����[�N�̏�����
	for (short playerModelVertexCount = 0; playerModelVertexCount < player->points.VertexNum ; playerModelVertexCount++)
	{
		g_PlayerModelVerticies[playerModelVertexCount].pos = player->points.VertexArray[playerModelVertexCount];
		g_PlayerModelVerticies[playerModelVertexCount].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_PlayerModelVerticies[playerModelVertexCount].material, sizeof(g_PlayerModelVerticies[playerModelVertexCount].material));
		g_PlayerModelVerticies[playerModelVertexCount].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_PlayerModelVerticies[playerModelVertexCount].bUse = true;
	}

	ENEMY* enemy = GetEnemy();
	// �p�[�e�B�N�����[�N�̏�����
	for (short enemyModelVertexCount = 0; enemyModelVertexCount < enemy[0].points.VertexNum; enemyModelVertexCount++)
	{
		g_EnemyModelVerticies[enemyModelVertexCount].pos = enemy[0].points.VertexArray[enemyModelVertexCount];
		g_EnemyModelVerticies[enemyModelVertexCount].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_EnemyModelVerticies[enemyModelVertexCount].material, sizeof(g_EnemyModelVerticies[enemyModelVertexCount].material));
		g_EnemyModelVerticies[enemyModelVertexCount].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_EnemyModelVerticies[enemyModelVertexCount].bUse = true;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPoint(void)
{
	if (g_Load == FALSE) return;

	//�e�N�X�`���̉��
	for (int nCntTex = 0; nCntTex < 1; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePoint(void)
{
	PLAYER* player = GetPlayer();
	// �p�[�e�B�N�����[�N�̏�����
	for (short playerModelVertexCount = 0; playerModelVertexCount < player->points.VertexNum; playerModelVertexCount++)
	{
		g_PlayerModelVerticies[playerModelVertexCount].pos = AffineTransform(player->points.VertexArray[playerModelVertexCount] ,XMLoadFloat4x4(&player->mtxWorld));
	}

	ENEMY* enemy = GetEnemy();
	// �p�[�e�B�N�����[�N�̏�����
	for (short enemyModelVertexCount = 0; enemyModelVertexCount < enemy[0].points.VertexNum; enemyModelVertexCount++)
	{
		g_EnemyModelVerticies[enemyModelVertexCount].pos = AffineTransform(enemy[0].points.VertexArray[enemyModelVertexCount], XMLoadFloat4x4(&enemy[0].mtxWorld));
	}

	if (GetKeyboardPress(DIK_LSHIFT))
	{
		g_pointidx++;
		if (g_pointidx > enemy[0].points.VertexNum) {
			g_pointidx -= enemy[0].points.VertexNum;
		}
	}
	
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPoint(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// ���C�e�B���O�𖳌���
	SetLightEnable(false);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r����
	SetDepthEnable(false);

	// �t�H�O����
	SetFogEnable(false);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
	PLAYER* player = GetPlayer();
	for (int playerModelVertexCount = 0; playerModelVertexCount < player->points.VertexNum; playerModelVertexCount++)
	{
		if (!g_PlayerModelVerticies[playerModelVertexCount].bUse) continue;
		
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		//�r���[�}�g���b�N�X���擾
		mtxView = XMLoadFloat4x4(&cam->mtxView);

		////mtxWorld = XMMatrixInverse(nullptr, mtxView);
		////mtxWorld.r[3].m128_f32[0] = 0.0f;
		////mtxWorld.r[3].m128_f32[1] = 0.0f;
		////mtxWorld.r[3].m128_f32[2] = 0.0f;

		// ������������������
		mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
		mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
		mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

		mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
		mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
		mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

		mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
		mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
		mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_PlayerModelVerticies[playerModelVertexCount].scale.x, g_PlayerModelVerticies[playerModelVertexCount].scale.y, g_PlayerModelVerticies[playerModelVertexCount].scale.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_PlayerModelVerticies[playerModelVertexCount].pos.x, g_PlayerModelVerticies[playerModelVertexCount].pos.y, g_PlayerModelVerticies[playerModelVertexCount].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// �}�e���A���ݒ�
		SetMaterial(g_PlayerModelVerticies[playerModelVertexCount].material);

		// �|���S���̕`��
		GetDeviceContext()->Draw(4, 0);
		
	}

	ENEMY* enemy = GetEnemy();
	for (int enemyModelVertexCount = 0; enemyModelVertexCount < enemy[0].points.VertexNum; enemyModelVertexCount++)
	{
		if (!g_EnemyModelVerticies[enemyModelVertexCount].bUse) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		//�r���[�}�g���b�N�X���擾
		mtxView = XMLoadFloat4x4(&cam->mtxView);

		// ������������������
		mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
		mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
		mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

		mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
		mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
		mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

		mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
		mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
		mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_EnemyModelVerticies[enemyModelVertexCount].scale.x, g_EnemyModelVerticies[enemyModelVertexCount].scale.y, g_EnemyModelVerticies[enemyModelVertexCount].scale.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_EnemyModelVerticies[enemyModelVertexCount].pos.x, g_EnemyModelVerticies[enemyModelVertexCount].pos.y, g_EnemyModelVerticies[enemyModelVertexCount].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// �}�e���A���ݒ�
		SetMaterial(g_EnemyModelVerticies[enemyModelVertexCount].material);

		// �|���S���̕`��
		GetDeviceContext()->Draw(4, 0);

	}

	// ���C�e�B���O��L����
	SetLightEnable(true);

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r�L��
	SetDepthEnable(true);

	// �t�H�O�L��
	SetFogEnable(GetFogEnable());
	PrintDebugProc("Part:%d\n",g_pointidx);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexPoints(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// �@���̐ݒ�
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// ���ˌ��̐ݒ�
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}