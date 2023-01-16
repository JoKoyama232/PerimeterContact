// 光カメラ処理 [lightCamera.h]
//制作者：小山　城
//主な役割：シャドウマップ
#include "main.h"
#include "input.h"
#include "lightCamera.h"
#include "debugproc.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// ビュー平面の視野角
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比	
#define	VIEW_NEAR_Z		(10.0f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z		(10000.0f)										// ビュー平面のFarZ値
#define SHADOWMAP_DIMENSION	(100)



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CAMERA			g_LightCamera;		// カメラデータ

static ID3D11Buffer* m_lightViewProjectionBuffer = NULL;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitLightCamera(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	ID3D11Device* pd3dDevice = GetDevice(); // Don't forget to initialize this

	ID3D11Texture2D* m_shadowMap = NULL;
	ID3D11DepthStencilView* m_shadowDepthView = NULL;
	ID3D11ShaderResourceView* m_shadowResourceView = NULL;
	ID3D11SamplerState* m_comparisonSampler_point = NULL;

	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 1;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	shadowMapDesc.Height = static_cast<UINT>(SHADOWMAP_DIMENSION);
	shadowMapDesc.Width = static_cast<UINT>(SHADOWMAP_DIMENSION);

	hr = pd3dDevice->CreateTexture2D(&shadowMapDesc, NULL, &m_shadowMap);
	if (FAILED(hr)) return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	
	hr = pd3dDevice->CreateDepthStencilView(
			m_shadowMap,
			&depthStencilViewDesc,
			&m_shadowDepthView
		);
	if (FAILED(hr)) return hr;

	
	hr = pd3dDevice->CreateShaderResourceView(
			m_shadowMap,
			&shaderResourceViewDesc,
			&m_shadowResourceView
		);
	if (FAILED(hr)) return hr;

	D3D11_SAMPLER_DESC comparisonSamplerDesc;
	ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.BorderColor[0] = 1.0f;
	comparisonSamplerDesc.BorderColor[1] = 1.0f;
	comparisonSamplerDesc.BorderColor[2] = 1.0f;
	comparisonSamplerDesc.BorderColor[3] = 1.0f;
	comparisonSamplerDesc.MinLOD = 0.f;
	comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	comparisonSamplerDesc.MipLODBias = 0.f;
	comparisonSamplerDesc.MaxAnisotropy = 0;
	comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

	// Point filtered shadows can be faster, and may be a good choice when
	// rendering on hardware with lower feature levels. This sample has a
	// UI option to enable/disable filtering so you can see the difference
	// in quality and speed.


	hr = pd3dDevice->CreateSamplerState(
			&comparisonSamplerDesc,
			&m_comparisonSampler_point
		);
	if (FAILED(hr)) return hr;

	D3D11_RASTERIZER_DESC drawingRenderStateDesc;
	ZeroMemory(&drawingRenderStateDesc, sizeof(D3D11_RASTERIZER_DESC));
	drawingRenderStateDesc.CullMode = D3D11_CULL_BACK;
	drawingRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	drawingRenderStateDesc.DepthClipEnable = true;

	// 定数バッファ生成
	D3D11_BUFFER_DESC viewProjectionConstantBufferDesc;
	viewProjectionConstantBufferDesc.ByteWidth = sizeof(XMMATRIX);
	viewProjectionConstantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	viewProjectionConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	viewProjectionConstantBufferDesc.CPUAccessFlags = 0;
	viewProjectionConstantBufferDesc.MiscFlags = 0;
	viewProjectionConstantBufferDesc.StructureByteStride = sizeof(float);

	hr = pd3dDevice->CreateBuffer(
		&viewProjectionConstantBufferDesc,
		nullptr,
		&m_lightViewProjectionBuffer
	);
	
	if (FAILED(hr)) return hr;

	{
		XMMATRIX lightPerspectiveMatrix = XMMatrixPerspectiveFovRH(
			XM_PIDIV2,
			1.0f,
			12.f,
			50.f
		);

		/*XMStoreFloat4x4(
			&m_lightViewProjectionBufferData.projection,
			XMMatrixTranspose(lightPerspectiveMatrix)
		);*/

		
	}
	return true;
}


//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitLightCamera(void)
{

}


//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateLightCamera(void)
{

}


//=============================================================================
// カメラの更新
//=============================================================================
void SetLightCamera(void)
{
	// Point light at (20, 15, 20), pointed at the origin. POV up-vector is along the y-axis.
	static const XMVECTORF32 eye = { 20.0f, 15.0f, 20.0f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	/*XMStoreFloat4x4(
		&m_lightViewProjectionBufferData.view,
		XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up))
	);*/

	// Store the light position to help calculate the shadow offset.
	//XMStoreFloat4(&m_lightViewProjectionBufferData.pos, eye);


}


//=============================================================================
// カメラの取得
//=============================================================================
CAMERA* GetLightCamera(void)
{
	return &g_LightCamera;
}
