//=============================================================================
//
// 頂点表示処理 [point.cpp]
// Author :	小山　城
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
// マクロ定義
//*****************************************************************************

#define	PARTICLE_SIZE_X		(1.0f)		// 頂点サイズ
#define	PARTICLE_SIZE_Y		(1.0f)		// 頂点サイズ

#define	MAX_POINTS		(512)		// パーティクル最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// 位置
	XMFLOAT3		scale;			// スケール
	MATERIAL		material;		// マテリアル
	bool			bUse;			// 使用しているかどうか

} VERTEXINFO;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexPoints(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView* g_Texture[1] = { NULL };	// テクスチャ情報
static int							g_TexNo;					// テクスチャ番号
static int							g_pointidx;
static VERTEXINFO					g_PlayerModelVerticies[MAX_POINTS];		// パーティクルワーク
static VERTEXINFO					g_EnemyModelVerticies[MAX_POINTS];		// パーティクルワーク

static char* g_TextureName[] =
{
	"data/TEXTURE/effect000.jpg",
};

static BOOL							g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPoint(void)
{
	// 頂点情報の作成
	MakeVertexPoints();

	// テクスチャ生成
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
	// パーティクルワークの初期化
	for (short playerModelVertexCount = 0; playerModelVertexCount < player->points.VertexNum ; playerModelVertexCount++)
	{
		g_PlayerModelVerticies[playerModelVertexCount].pos = player->points.VertexArray[playerModelVertexCount];
		g_PlayerModelVerticies[playerModelVertexCount].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_PlayerModelVerticies[playerModelVertexCount].material, sizeof(g_PlayerModelVerticies[playerModelVertexCount].material));
		g_PlayerModelVerticies[playerModelVertexCount].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_PlayerModelVerticies[playerModelVertexCount].bUse = true;
	}

	ENEMY* enemy = GetEnemy();
	// パーティクルワークの初期化
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
// 終了処理
//=============================================================================
void UninitPoint(void)
{
	if (g_Load == FALSE) return;

	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < 1; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePoint(void)
{
	PLAYER* player = GetPlayer();
	// パーティクルワークの初期化
	for (short playerModelVertexCount = 0; playerModelVertexCount < player->points.VertexNum; playerModelVertexCount++)
	{
		g_PlayerModelVerticies[playerModelVertexCount].pos = AffineTransform(player->points.VertexArray[playerModelVertexCount] ,XMLoadFloat4x4(&player->mtxWorld));
	}

	ENEMY* enemy = GetEnemy();
	// パーティクルワークの初期化
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
// 描画処理
//=============================================================================
void DrawPoint(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// ライティングを無効に
	SetLightEnable(false);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較無し
	SetDepthEnable(false);

	// フォグ無効
	SetFogEnable(false);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
	PLAYER* player = GetPlayer();
	for (int playerModelVertexCount = 0; playerModelVertexCount < player->points.VertexNum; playerModelVertexCount++)
	{
		if (!g_PlayerModelVerticies[playerModelVertexCount].bUse) continue;
		
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		//ビューマトリックスを取得
		mtxView = XMLoadFloat4x4(&cam->mtxView);

		////mtxWorld = XMMatrixInverse(nullptr, mtxView);
		////mtxWorld.r[3].m128_f32[0] = 0.0f;
		////mtxWorld.r[3].m128_f32[1] = 0.0f;
		////mtxWorld.r[3].m128_f32[2] = 0.0f;

		// 処理が速いしお勧め
		mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
		mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
		mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

		mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
		mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
		mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

		mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
		mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
		mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

		// スケールを反映
		mtxScl = XMMatrixScaling(g_PlayerModelVerticies[playerModelVertexCount].scale.x, g_PlayerModelVerticies[playerModelVertexCount].scale.y, g_PlayerModelVerticies[playerModelVertexCount].scale.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_PlayerModelVerticies[playerModelVertexCount].pos.x, g_PlayerModelVerticies[playerModelVertexCount].pos.y, g_PlayerModelVerticies[playerModelVertexCount].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// マテリアル設定
		SetMaterial(g_PlayerModelVerticies[playerModelVertexCount].material);

		// ポリゴンの描画
		GetDeviceContext()->Draw(4, 0);
		
	}

	ENEMY* enemy = GetEnemy();
	for (int enemyModelVertexCount = 0; enemyModelVertexCount < enemy[0].points.VertexNum; enemyModelVertexCount++)
	{
		if (!g_EnemyModelVerticies[enemyModelVertexCount].bUse) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		//ビューマトリックスを取得
		mtxView = XMLoadFloat4x4(&cam->mtxView);

		// 処理が速いしお勧め
		mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
		mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
		mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

		mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
		mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
		mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

		mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
		mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
		mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

		// スケールを反映
		mtxScl = XMMatrixScaling(g_EnemyModelVerticies[enemyModelVertexCount].scale.x, g_EnemyModelVerticies[enemyModelVertexCount].scale.y, g_EnemyModelVerticies[enemyModelVertexCount].scale.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_EnemyModelVerticies[enemyModelVertexCount].pos.x, g_EnemyModelVerticies[enemyModelVertexCount].pos.y, g_EnemyModelVerticies[enemyModelVertexCount].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// マテリアル設定
		SetMaterial(g_EnemyModelVerticies[enemyModelVertexCount].material);

		// ポリゴンの描画
		GetDeviceContext()->Draw(4, 0);

	}

	// ライティングを有効に
	SetLightEnable(true);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(true);

	// フォグ有効
	SetFogEnable(GetFogEnable());
	PrintDebugProc("Part:%d\n",g_pointidx);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexPoints(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}