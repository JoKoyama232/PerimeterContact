//=============================================================================
//
// HUD���� [hud.cpp]
// Author : ���R�@��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "sprite.h"
#include "model.h"
#include "collision.h"
#include "enemy.h"
#include "player.h"
#include "hud.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					(4)				// �e�N�X�`���̐�


enum {
	playerHpFrame = 0,
	playerHpBar,
	enemyHpFrame,
	enemyHpBar,
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/HPFrame.png",
	"data/TEXTURE/HPBar.png",
	"data/TEXTURE/HPFrame.png",
	"data/TEXTURE/HPBar.png",
};

HUD	hud[TEXTURE_MAX];
static XMFLOAT4 hudColorPlayer;
static XMFLOAT4 hudColorEnemy;
static bool		g_Use = true;
static bool		g_Load = false;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitHud(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	hudColorPlayer = { 1.0f,1.0f,1.0f,0.75f };
	hudColorEnemy = { 1.0f,0.0f,0.0f,0.75f };

	hud[playerHpFrame].pos = { SCREEN_WIDTH / 2,50.0f };
	hud[playerHpFrame].size = { 640.0f,80.0f };
	hud[playerHpFrame].texturePos = { 0.0f,0.0f };
	hud[playerHpFrame].texturePercent = { 1.0f,1.0f };

	hud[playerHpBar].pos = { SCREEN_WIDTH / 2,50.0f };
	hud[playerHpBar].size = { 640.0f,80.0f };
	hud[playerHpBar].texturePos = { 0.0f,0.0f};
	hud[playerHpBar].texturePercent = { 1.0f,1.0f };

	hud[enemyHpFrame].pos = { SCREEN_WIDTH / 2,400.0f };
	hud[enemyHpFrame].size = { 640.0f,80.0f };
	hud[enemyHpFrame].texturePos = { 0.0f,0.0f };
	hud[enemyHpFrame].texturePercent = { 1.0f,1.0f };
		
	hud[enemyHpBar].pos = { SCREEN_WIDTH / 2,400.0f };
	hud[enemyHpBar].size = { 640.0f,80.0f };
	hud[enemyHpBar].texturePos = { 0.0f,0.0f };
	hud[enemyHpBar].texturePercent = { 1.0f,1.0f };



	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����
	g_Use = true;

	// BGM�Đ�


	g_Load = true;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitHud(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateHud(void)
{
	PLAYER *player = GetPlayer();
	float percentHp = player->hpCurrent / (float)player->hpMax;
	hud[playerHpBar].texturePercent.x = percentHp;
	hud[playerHpBar].size.x = 640.0f * percentHp;
	hud[playerHpBar].pos.x = SCREEN_CENTER_X -  320.0f *(1- percentHp);
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawHud(void)
{
	// 2D�̕���`�悷�鏈��
	// Z��r�Ȃ�
	SetDepthEnable(false);

	// ���C�e�B���O�𖳌�
	SetLightEnable(false);
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		if (i < 1) {
			SetSpriteColor(g_VertexBuffer, hud[i].pos.x, hud[i].pos.y, hud[i].size.x, hud[i].size.y,
				hud[i].texturePos.x, hud[i].texturePos.y, hud[i].texturePercent.x, hud[i].texturePercent.y,
				hudColorEnemy);
		}
		else {
			SetSpriteColor(g_VertexBuffer, hud[i].pos.x, hud[i].pos.y, hud[i].size.x, hud[i].size.y,
				hud[i].texturePos.x, hud[i].texturePos.y, hud[i].texturePercent.x, hud[i].texturePercent.y,
				hudColorPlayer);
		}

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// ���C�e�B���O��L����
	SetLightEnable(true);

	// Z��r����
	SetDepthEnable(true);
}




