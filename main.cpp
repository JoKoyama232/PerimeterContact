//=============================================================================
//
// ���C������ [main.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "collision.h"
#include "player.h"
#include "enemy.h"
#include "shadow.h"
#include "light.h"
#include "meshfield.h"
#include "meshwall.h"
#include "tree.h"
#include "bullet.h"
#include "score.h"
#include "sound.h"
#include "title.h"
#include "result.h"
#include "tutorial.h"
#include "particle.h"
#include "point.h"
#include "knife.h"
#include "skydome.h"
#include "fade.h"
#include "3DParticle.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"Desert Duel"		// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void CheckHit(void);


//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;
bool firstLoad = true;
tagRECT window;


#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

int	g_Mode = MODE_TITLE;					// �N�����̉�ʂ�ݒ�


//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// �E�B���h�E�̍����W
		CW_USEDEFAULT,																		// �E�B���h�E�̏���W
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,
		NULL,
		hInstance,
		NULL);

	// ����������(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, true)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000/60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif
				RECT window;
				GetWindowRect(hWnd, &window);
				SetWindowInfo(window);
				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitSound(hWnd);

	InitLight();

	InitCamera();

	// ���͏����̏�����
	InitInput(hInstance, hWnd);
	//-----
	InitFade();

	//PlaySound(SOUND_LABEL_BGM_sample001);


	// ���C�g��L����
	SetLightEnable(true);

	// �w�ʃ|���S�����J�����O
	SetCullingMode(CULL_MODE_BACK);
	SetMode(g_Mode);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	UninitSound();

	UninitFade();
	// �J�����̏I������
	UninitCamera();

	//���͂̏I������
	UninitInput();

	// �����_���[�̏I������
	UninitRenderer();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// ���͂̍X�V����
	UpdateInput();

	switch (g_Mode) {
	case MODE_TITLE:
		UpdateTitle();
		break;
	case MODE_TUTORIAL:
		UpdateTutorial();
		break;
	case MODE_GAME:
		UpdateSky();
		// �n�ʏ����̍X�V
		UpdateMeshField();

		// �Ǐ����̍X�V
		UpdateMeshWall();

		// �v���C���[�̍X�V����
		UpdatePlayer();

		// �G�l�~�[�̍X�V����
		UpdateEnemy();

		// �؂̍X�V����
		UpdateTree();

		// �e�̍X�V����
		UpdateBullet();

		UpdateParticle();
		UpdateParticle3D();
		UpdateKnife();

		// �e�̍X�V����
		UpdateShadow();

		// �����蔻��
		CheckHit();

		// �X�R�A�̍X�V����
		UpdateScore();
		break;
	case MODE_RESULT:
		UpdateResult();
		break;

	}
	// �J�����X�V
	UpdateCamera();

	UpdateFade();
	

}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();
	SetViewPort(TYPE_FULL_SCREEN);
	switch (g_Mode) {
	case MODE_TITLE:
		DrawTitle();
		break;
	case MODE_TUTORIAL:
		DrawTutorial();
		break;
	case MODE_GAME:
		// �v���C���[���_
		XMFLOAT3 pos = GetPlayer()->pos;
		pos.y += 10.0f;			// �J����������h�����߂ɃN���A���Ă���
		SetCameraPos(pos);
		SetCamera();
		DrawSky();
		// �n�ʂ̕`�揈��
		DrawMeshField();

		// �e�̕`�揈��
		DrawShadow();

		// �v���C���[�̕`�揈��
		//DrawPlayer();

		// �G�l�~�[�̕`�揈��
		DrawEnemy();
		UpdatePoint();
		// �e�̕`�揈��
		DrawBullet();

		// �ǂ̕`�揈��
		DrawMeshWall();
		DrawKnife();
		// �؂̕`�揈��
		//DrawTree();

		DrawParticle();
		DrawParticle3D();
		DrawPoint();

		// �X�R�A�̕`�揈��
		//DrawScore();

		break;
	case MODE_RESULT:
		DrawResult();
		break;

	}
	//SetViewPort(TYPE_RIGHT_HALF_SCREEN);
	DrawFade();





#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif



//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY  *enemy  = GetEnemy();	// �G�l�~�[�̃|�C���^�[��������
	ENEMY  *enemyPart = GetEnemyParts();
	PLAYER *player = GetPlayer();	// �v���C���[�̃|�C���^�[��������
	KNIFE  *knife  = GetKnife();	// �i�C�t�̃|�C���^�[��������
	CAMERA* cam = GetCamera();
	
	XMFLOAT3 *playerVerts = new XMFLOAT3[player->points.VertexNum];
	CAPSULEHITBOX playerCapsule = player->hitbox;
	playerCapsule.positiona = AffineTransform(player->hitbox.positiona, XMLoadFloat4x4(&player->mtxWorld));
	playerCapsule.positionb = AffineTransform(player->hitbox.positionb, XMLoadFloat4x4(&player->mtxWorld));
	
	if (!playerVerts == NULL) {

		AppliedTransform(playerVerts, player->points.VertexNum, player->points.VertexArray, XMLoadFloat4x4(&player->mtxWorld));
		//PrintDebugProc("Player:X:%f Y:%f Z:%f\n", playerCapsule.positiona.x, playerCapsule.positiona.y, playerCapsule.positiona.z);
		//PrintDebugProc("Player:X:%f Y:%f Z:%f\n", playerCapsule.positionb.x, playerCapsule.positionb.y, playerCapsule.positionb.z);
		//PrintDebugProc("Player:vert:%d\n", player->points.VertexNum);
		// �G�ƃv���C���[�L����
		for (int i = 0; i < MAX_ENEMY; i++)
		{
			//�G�̗L���t���O���`�F�b�N����
			if (enemy[i].use == false)
				continue;

			XMFLOAT3* enemyVerts = new XMFLOAT3[enemy[i].points.VertexNum];
			CAPSULEHITBOX enemyCapsule = enemy[i].hitbox;
			enemyCapsule.positiona = AffineTransform(enemyCapsule.positiona, XMLoadFloat4x4(&enemy[i].mtxWorld));
			enemyCapsule.positionb = AffineTransform(enemyCapsule.positionb, XMLoadFloat4x4(&enemy[i].mtxWorld));
			if (!enemyVerts == NULL) {
				for (int p = 0; p < enemy[i].points.VertexNum; p++) {
					enemyVerts[p] = AffineTransform(enemy[i].points.VertexArray[p], XMLoadFloat4x4(&enemy[i].mtxWorld));
				}
				//PrintDebugProc("Enemyhit:X:%f Y:%f Z:%f\n", enemyCapsule.positiona.x, enemyCapsule.positiona.y, enemyCapsule.positiona.z);
				//PrintDebugProc("Enemyhit:X:%f Y:%f Z:%f\n", enemyCapsule.positionb.x, enemyCapsule.positionb.y, enemyCapsule.positionb.z);
				
				//GJKHit(playerVerts, player->points.VertexNum, enemyVerts, enemy[i].points.VertexNum)
				//BC�̓����蔻��
				if (CapsuleCollision(playerCapsule,enemyCapsule))
				{

					// �G�L�����N�^�[�͓|�����
					/*enemy[i].use = false;*/
					ReleaseShadow(enemy[i].shadowIdx);

					// �X�R�A�𑫂�
					AddScore(100);

				}
			}
			delete[] enemyVerts;
		}
	}
	delete[] playerVerts;
	


	// �v���C���[�̒e�ƓG
	for (int i = 0; i < MAX_KNIFE; i++)
	{
		//�i�C�t�̗L���t���O���`�F�b�N����
		if (knife[i].state == unused)
			continue;

		// �G�Ɠ������Ă邩���ׂ�

		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//for (int k = 0; k < ENEMY_PARTS_MAX; k++) {
			//	if (enemyPart[k].use == false
			//		||knife[i].state == hit
			//		|| !GJKHit(knife[i].hitbox.list, knife[i].modelVertexPosition.VertexNum, enemyPart[k].gjkList.list, enemyPart[k].points.VertexNum))
			//		continue;
			//	XMMATRIX mtxWorld = XMMatrixIdentity();
			//	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(knife[i].rot.x, knife[i].rot.y, knife[i].rot.z);

			//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			//	// ������������i�C�t��ԍX�V
			//	knife[i].state = hit;
			//	knife[i].attachedTo = &enemyPart[k];
			//	knife[i].parentRot = knife[i].attachedTo->rot;
			//	XMVECTOR knifeInfo = XMLoadFloat3(&knife[i].pos);
			//	XMVECTOR enemyInfo = XMLoadFloat3(&knife[i].attachedTo->pos);
			//	XMFLOAT3 position;
			//	knifeInfo -= enemyInfo;
			//	XMStoreFloat3(&position, knifeInfo);

			//	 mtxWorld = XMMatrixTranspose(XMLoadFloat4x4(&enemyPart[k].mtxWorld));
			//	AffineTransform(position, mtxWorld);
			//	knife[i].pos = position;
			//	break;
			//}
			//�G�̗L���t���O���`�F�b�N����
			if (enemy[j].use == false
				|| !GJKHit(knife[i].hitbox.list, knife[i].modelVertexPosition.VertexNum, enemy[j].gjkList.list, enemy[j].points.VertexNum))
				continue;

			//GJK�̓����蔻��
			// ������������i�C�t��ԍX�V
			//for (int h = 0; h < 10; h++) {
			//	SetParticle(knife[i].pos, { (rand() % 30 - 15) * 2.0f,5.0f,(rand() % 30 - 15) * 2.0f }, { 0.0f,-2.0f,0.0f }, cam->rot,
			//		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), 0.1f, 0.1f, false, 0, 240);
			//	

			//}
			//SetParticle3D(knife[i].pos, {0,0,0},16,true);

			knife[i].state = hit;
			knife[i].attachedTo = &enemy[j];
			knife[i].parentRot = knife[i].attachedTo->rot;
			XMVECTOR knifeInfo = XMLoadFloat3(&knife[i].pos);
			XMVECTOR enemyInfo = XMLoadFloat3(&knife[i].attachedTo->pos);
			XMFLOAT3 position;
			knifeInfo -= enemyInfo;
			XMStoreFloat3(&position, knifeInfo/2);

			knifeInfo = XMLoadFloat3(&knife[i].scl);
			enemyInfo = XMLoadFloat3(&knife[i].attachedTo->scl);
			knifeInfo /= enemyInfo;
			XMStoreFloat3(&knife[i].scl, knifeInfo);

			XMMATRIX mtxWorld = XMMatrixTranspose(XMLoadFloat4x4(&enemy[j].mtxWorld));
			AffineTransform(position, mtxWorld );
			knife[i].pos = position;

			//�����������̏���
			PlaySound(SOUND_LABEL_SE_hit);
			
			//enemy[0].hp -= 10;
			if (enemy[0].hp <= 0) {
				enemy[0].use = false;
				//SetMode(MODE_RESULT);
			}
			// �X�R�A�𑫂�
			AddScore(10);

		}

	}


	// �G�l�~�[���S�����S�������ԑJ��
	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == false) continue;
		enemy_count++;
	}

	// �G�l�~�[���O�C�H
	if (enemy_count == 0)
	{
		 SetMode(MODE_RESULT);

	}

}

void SetMode(int mode) {
	g_Mode = mode;
	StopSound();

	// �^�C�g����ʂ̏I������
	UninitTitle();

	//
	UninitTutorial();
	UninitPoint();
	//�Q�[����ʂ̏I������
	// �X�R�A�̏I������
	UninitScore();
	UninitSky();
	// �e�̏I������
	UninitBullet();

	// �؂̏I������
	UninitTree();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();

	// �ǂ̏I������
	UninitMeshWall();

	// �n�ʂ̏I������
	UninitMeshField();

	UninitParticle();
	UninitParticle3D();
	UninitKnife();
	//���U���g�����̏I������
	UninitResult();

	

	switch (g_Mode) {
	case MODE_TITLE:
		InitTitle();
		//PlaySound(SOUND_LABEL_BGM_sample000);
		break;
	case MODE_TUTORIAL:
		InitTutorial();
		//PlaySound(SOUND_LABEL_BGM_sample001);
		break;
	case MODE_GAME:
		// �t�B�[���h�̏�����
		InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 20, 20, 100.0f, 100.0f);
		firstLoad = false;
		//// �ǂ̏�����
		//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
		//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
		//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
		//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

		//// ��(�����p�̔�����)
		//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, XM_PI, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
		//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
		//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
		//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

		// �e�̏���������
		InitShadow();
		InitSky();
		// �v���C���[�̏�����
		InitPlayer();

		// �G�l�~�[�̏�����
		InitEnemy();

		InitKnife();

		InitPoint();
		// �؂𐶂₷
		InitTree();

		// �e�̏�����
		InitBullet();
		InitParticle3D();
		InitParticle();
		// �X�R�A�̏�����
		InitScore();
		//PlaySound(SOUND_LABEL_BGM_sample002);
		break;
	case MODE_RESULT:
		InitResult();
		//PlaySound(SOUND_LABEL_BGM_sample003);
		break;
	}
}

int	GetMode(void) {
	return g_Mode;
}

bool IsFirstLoad(void) {
	return firstLoad;
}

void setLoad(bool update) {
	firstLoad = update;
}

