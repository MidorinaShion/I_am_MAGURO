#include "DxLib.h"
#include <math.h>

//########## �}�N����` ##########

#define DEBUG_MODE			TRUE	//�f�o�b�O���[�h

#define GAME_WIDTH			1000	//��ʂ̉��̑傫��
#define GAME_HEIGHT			600	//��ʂ̏c�̑傫��
#define GAME_COLOR			32	//��ʂ̃J���[�r�b�g

#define GAME_WINDOW_BAR		0				//�^�C�g���o�[�̓f�t�H���g�ɂ���
#define GAME_WINDOW_NAME	"I am MAGURO"	//�E�B���h�E�̃^�C�g��

#define GAME_FPS			60		//FPS

//�p�X�̒���  255�����܂�
#define PATH_MAX	255
#define NAME_MAX	255

//3D���f���̃p�X
#define MODEL_MAGURO_PATH	TEXT(".\\mqo���f��\\�}�O��\\�}�O��.mqo")
#define MODEL_EBI_PATH		TEXT(".\\mqo���f��\\�G�r\\�G�r.mqo")
#define MODEL_IKA_PATH		TEXT(".\\mqo���f��\\�C�J\\�C�J.mqo")
#define MODEL_PURA_PATH		TEXT(".\\mqo���f��\\�v�����N�g��\\�v�����N�g��.mqo")

//�摜�̃p�X(���u��)
#define IMAGE_TITLE_BK_PATH		TEXT(".\\�摜\\�^�C�g���w�i.png")
#define IMAGE_TITLE_LOGO_PATH	TEXT(".\\�摜\\�^�C�g�����S.png")
#define IMAGE_TITLE_START_PATH	TEXT(".\\�摜\\�͂��߂�.png")
#define IMAGE_TITLE_END_PATH	TEXT(".\\�摜\\�����1.png")
#define IMAGE_TITLE_HOWTO_PATH	TEXT(".\\�摜\\�����т���.png")
#define IMAGE_HOWTO_BK_PATH		TEXT(".\\�摜\\�����т����w�i.png")

#define IMAGE_PLAY_BK_PATH	TEXT(".\\�摜\\�v���C�w�i.png")

//�J�����̐ݒ�
#define CAMERA_NEAR			500.0f		//�ǂ��܂ŋ߂����ʂ���
#define CAMERA_FAR			60000.0f	//�ǂ��܂ŉ������ʂ���
#define CAMERA_HEIGHT		0.0f		//�J�����̒����_�̍���
#define CAMERA_DISTANCE		1000.0f		//�J�����ƒ����_�̋���
#define CAMERA_INIT_V_ANGLE	0.0f		//�J�����̌����f�t�H���g(����) / -90���� 90����
#define CAMERA_INIT_H_ANGLE	180.0f		//�J�����̌����f�t�H���g(����) / -90���� 90���E
#define CAMERA_ANGLE_X_PLUS	1.0f		//�J�����̊p�x(X)�̉��Z�l
#define CAMERA_ANGLE_MAX_X	90.0f		//�J������MAX�p�x(X)
#define CAMERA_ANGLE_MAX_Y	90.0f		//�J������MAX�p�x(Y)

//�}�b�v
#define MAP_TATE_MAX	150
#define MAP_YOKO_MAX	150

#define MAP_TATE_SIZE	(100.0f * 2.0f) //�Ԋu��2.0�{�J����
#define MAP_YOKO_SIZE	(100.0f * 2.0f)

//�}�O���֌W
#define MAGURO_MOVE			10.0f	//�X�s�[�h
#define MAGURO_CAP_RADIUS	30.0f	//�����蔻��̃J�v�Z���̔��a

//�G���[���b�Z�[�W
#define ERR_LOAD_TITLE_IMAGE	TEXT("�摜�ǂݍ��݃G���[")
#define ERR_LOAD_TITLE_MODEL	TEXT("���f���ǂݍ��݃G���[")

//�I���_�C�A���O�p
#define CLOSE_WINDOW_TITLE		TEXT("�Q�[���I��")
#define CLOSE_WINDOW_CAPTION	TEXT("�Q�[�����I�����܂����H")

//�Q�[���̃V�[��
enum GAME_SCENE
{
	GAME_SCENE_START,
	GAME_SCENE_HOWTO,
	GAME_SCENE_PLAY,
	GAME_SCENE_END
};

//int�^��POINT�\����
typedef struct STRUCT_I_POINT
{
	//���W��������
	int x = -1;
	int y = -1;

}iPOINT;

//�摜�\����
typedef struct STRUCT_IMAGE
{
	char path[PATH_MAX] = { '\0' };	//�p�X
	int handle = -1;				//�n���h��
	int x = 0;						//X�ʒu
	int y = 0;						//Y�ʒu
	int width = 0;					//��
	int height = 0;					//����
	BOOL IsDraw = FALSE;			//�`��ł��邩(�f�t�H���g��FALSE)

}IMAGE;

//3D���f���\����
typedef struct STRUCT_MODEL
{
	char path[PATH_MAX] = { '\0' };		//�p�X
	int handle = -1;					//�n���h��
	VECTOR pos = { 0.0f,0.0f,0.0f };	//���f���̈ʒu
	VECTOR move = { 0.0f,0.0f,0.0f };	//���f���̓���
	float angle = 0.0f;		//���f���̉�]
	BOOL IsDraw = FALSE;	//�`��ł��邩(�f�t�H���g��FALSE)
	double VecSin = 0.0f;	//�v���C���[�̌����Ă�������F�ʓx(sin = Y)
	double VecCos = 0.0f;	//�v���C���[�̌����Ă�������F�ʓx(cos = X)
	float HAngle = 0.0f;	//�����p�x
	BOOL IsMove = TRUE;		//�����邩

}MODEL;

//�J�����\����
typedef struct STRUCT_CAMERA
{
	float CamNear = 0.0f;				//�ǂ��܂ŋ߂����ʂ���
	float CamFar = 0.0f;				//�ǂ��܂ŉ������ʂ���
	float CamHeight = 0.0f;				//�J�����̍����̈ʒu
	float VAngle = 0.0;					//�J�����̐����̊p�x
	float HAngle = 0.0;					//�J�����̐����̊p�x
	VECTOR pos = { 0.0f,0.0f,0.0f };	//�J�����̏ꏊ
	float angle = 90.0f;				//�J�����̊p�x

}CAMERA;

//########## �O���[�o���ϐ� ##########

//FPS�֘A
int StartTimeFps;				//����J�n����
int CountFps;					//�J�E���^
float CalcFps;					//�v�Z����
int SampleNumFps = GAME_FPS;	//���ς����T���v����

//�L�[�{�[�h�̓��͂��擾
char AllKeyState[256] = { '\0' };		//���ׂẴL�[�̏�Ԃ�����
char OldAllKeyState[256] = { '\0' };	//���ׂẴL�[�̏��(���O)������

//�Q�[���V�[�����Ǘ�
int GameScene;

//�J����
CAMERA camera;

//���f��
MODEL Maguro;	//�}�O��
MODEL Ebi;		//�G�r
MODEL Ika;		//�C�J
MODEL Pura;		//�v�����N�g��

//�摜
IMAGE ImageTitleBK;		//���u��
IMAGE ImageTitleLOGO;
IMAGE ImageTitleStart;
IMAGE ImageTitleEnd;
IMAGE ImageTitleHowTo;
IMAGE ImageHowToBK;

IMAGE ImagePlayBK;

//�}�O���̈ʒu���}�b�v�ƑΉ��t����
int MaguroToMapX;
int MaguroToMapZ;

//�}�O���̓����蔻��(�J�v�Z��)
VECTOR MaguroCollVecStart;						//�J�v�Z���̎n�_
VECTOR MaguroCollVecEnd;						//�J�v�Z���̏I�_
float MaguroCollRadius = MAGURO_CAP_RADIUS;		//�J�v�Z���̔��a

//�}�O�����������Ƃ��̍��W���L��
VECTOR MaguroMovePos;

//int MaguroScale = 0.05;				//�ŏ��̃}�O���̑傫��
//
//VECTOR ModelPos = { 0.0f,0.0f,0.0f };	//���f���̍��W���Ǘ�
//
//VECTOR ModelScale = { 0.5f,0.5f,0.5f };//�ŏ��̃}�O���̑傫��
//
//VECTOR AddScale_pla = { 1.05f,1.05f,1.05f };	//�v�����N�g����H�ׂ��Ƃ��ɉ��Z
//
//VECTOR AddScale_ebi = { 1.1f,1.1f,1.1f };	//�G�r��H�ׂ��Ƃ��ɉ��Z
//
//VECTOR AddScale_ika = { 1.3f,1.3f,1.3f };	//�C�J��H�ׂ��Ƃ��ɉ��Z
//
//MV1_COLL_RESULT_POLY HitPoly;		//�����蔻��H

//########## �v���g�^�C�v�錾 ##########

VOID MY_FPS_UPDATE(VOID);		//FPS�l���v���A�X�V����
VOID MY_FPS_DRAW(VOID);			//FPS�l��`�悷��
VOID MY_FPS_WAIT(VOID);			//FPS�l���v�����A�҂�

VOID MY_ALL_KEYDOWN_UPDATE(VOID);	//�L�[�̓��͏�Ԃ��X�V����
BOOL MY_KEY_DOWN(int);				//�L�[�������Ă��邩�A�L�[�R�[�h�Ŕ��f����
BOOL MY_KEY_UP(int);				//�L�[�������グ�����A�L�[�R�[�h�Ŕ��f����
BOOL MY_KEYDOWN_KEEP(int, int);		//�L�[�����������Ă��邩�A�L�[�R�[�h�Ŕ��f����

BOOL MY_GAME_INIT(VOID);	//�Q�[�����������

VOID MY_START(VOID);		//�X�^�[�g���
VOID MY_START_PROC(VOID);	//�X�^�[�g��ʂ̏���
VOID MY_START_DRAW(VOID);	//�X�^�[�g��ʂ̕`��

VOID MY_HOWTO(VOID);		//�����т������
VOID MY_HOWTO_PROC(VOID);	
VOID MY_HOWTO_DRAW(VOID);	

VOID MY_PLAY(VOID);			//�v���C���
VOID MY_PLAY_PROC(VOID);	//�v���C��ʂ̏���
VOID MY_PLAY_DRAW(VOID);	//�v���C��ʂ̕`��

VOID MY_END(VOID);			//�G���h���
VOID MY_END_PROC(VOID);		//�G���h��ʂ̏���
VOID MY_END_DRAW(VOID);		//�G���h��ʂ̕`��

BOOL MY_LOAD_IMAGE(VOID);		//�摜���܂Ƃ߂ēǂݍ��ފ֐�
VOID MY_DELETE_IMAGE(VOID);		//�摜���܂Ƃ߂č폜����֐�

BOOL MY_LOAD_MODEL(VOID);		//3D���f�����܂Ƃ߂ēǂݍ��ފ֐�
VOID MY_DELETE_MODEL(VOID);		//3D���f�����܂Ƃ߂č폜����֐�

VOID MY_PROC_MAGURO(VOID);		//�}�O���̏���

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE);								//�E�B���h�E���[�h�ɐݒ�
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//�w��̐��l�ŃE�B���h�E��\������
	SetWindowStyleMode(GAME_WINDOW_BAR);				//�^�C�g���o�[�̓f�t�H���g�ɂ���
	SetMainWindowText(TEXT(GAME_WINDOW_NAME));			//�E�B���h�E�̃^�C�g���̕���
	SetAlwaysRunFlag(TRUE);								//��A�N�e�B�u�ł����s����

	//�c�w���C�u��������������
	if (DxLib_Init() == -1) { return -1; }

	SetMouseDispFlag(TRUE);					//�}�E�X�J�[�\����\��	
	SetDrawScreen(DX_SCREEN_BACK);			//Draw�n�֐��͗���ʂɕ`��

	//�摜��ǂݍ���
	if (MY_LOAD_IMAGE() == FALSE) { return -1; }

	//3D���f����ǂݍ���
	if (MY_LOAD_MODEL() == FALSE) { return -1; }

	//�Q�[���̏�����
	if (MY_GAME_INIT() == FALSE) { return -1; }

	//�W�����C�g�̕�����ݒ肷��
	SetLightDirection(VGet(0.0f, 1.0f, 0.0f));

	//�������[�v
	while (TRUE)
	{
		//���b�Z�[�W�����̌��ʂ��G���[�̂Ƃ��A�����I��
		if (ProcessMessage() != 0) { break; }

		//��ʂ������ł��Ȃ������Ƃ��A�����I��
		if (ClearDrawScreen() != 0) { break; }

		//������I�ԂƃQ�[���I���_�C�A���O��\��(���u����ESC�L�[)
		if (AllKeyState[KEY_INPUT_ESCAPE] != 0)
		{
			int Ret = MessageBox(GetMainWindowHandle(), "�I�����܂����H", "�m�F", MB_YESNO);
		
			if (Ret == IDYES) { break; }
		
		}

		//�����Ă���L�[��Ԃ��擾
		MY_ALL_KEYDOWN_UPDATE();

		//FPS�̏���(�X�V)
		MY_FPS_UPDATE();

		//�V�[�����Ƃɏ������s��
		switch (GameScene)
		{

		case GAME_SCENE_START:
			MY_START();	//�X�^�[�g���
			break;

		case GAME_SCENE_HOWTO:
			MY_HOWTO();	//�����т������
			break;

		case GAME_SCENE_PLAY:
			MY_PLAY();	//�v���C���
			break;

		case GAME_SCENE_END:
			MY_END();	//�G���h���
			break;

		}

		MY_FPS_DRAW();	//FPS�̏���(�`��)

		ScreenFlip();	//���j�^�̃��t���b�V�����[�g�̑����ŉ�ʂ��ĕ`��

		MY_FPS_WAIT();	//FPS�̏���(�҂�)

	}

	// �c�w���C�u�����̌�n��
	DxLib_End();

	// �\�t�g�̏I��
	return 0;
}

//FPS���v���A�X�V����֐� ################################3
VOID MY_FPS_UPDATE(VOID)
{

	if (CountFps == 0) //1�t���[���ڂȂ玞�����L��
	{
		StartTimeFps = GetNowCount();
	}

	if (CountFps == SampleNumFps)
	{
		int now = GetNowCount();
		//���݂̎��Ԃ���A0�t���[���ڂ̎��Ԃ������AFPS�̐��l�Ŋ��遁1FPS�ӂ�̕��ώ��Ԃ��v�Z�����
		CalcFps = 1000.f / ((now - StartTimeFps) / (float)SampleNumFps);
		CountFps = 0;
		StartTimeFps = now;
	}

	CountFps++;
	return;

}

//FPS�l��`�悷��֐� ##############################
VOID MY_FPS_DRAW(VOID)
{
	//�������`��
	DrawFormatString(
		0,
		GAME_HEIGHT - 20,
		GetColor(255, 255, 255),
		"FPS : %.1f",
		CalcFps
	);
	return;
}

//FPS�l���v�����A�҂֐� ##########################
VOID MY_FPS_WAIT(VOID)
{
	int resultTime = GetNowCount() - StartTimeFps;			//������������
	int waitTime = CountFps * 1000 / GAME_FPS - resultTime;	//�҂ׂ�����

	if (waitTime > 0)
	{
		WaitTimer(waitTime);	//�҂�
	}

	return;

}

//########## �L�[�̓��͏�Ԃ��X�V����֐� ##########
VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{
	//�Q�l�Fhttps://dxlib.xsrv.jp/function/dxfunc_input.html

	char TempKey[256];	//�ꎞ�I�ɁA���݂̃L�[�̓��͏�Ԃ��i�[����

	//���O�̃L�[���͂��Ƃ��Ă���
	for (int i = 0; i < 256; i++)
	{
		OldAllKeyState[i] = AllKeyState[i];
	}

	GetHitKeyStateAll(TempKey); // �S�ẴL�[�̓��͏�Ԃ𓾂�

	for (int i = 0; i < 256; i++)
	{
		if (TempKey[i] != 0)	//������Ă���L�[�̃L�[�R�[�h�������Ă���Ƃ�
		{
			AllKeyState[i]++;	//������Ă���
		}
		else
		{
			AllKeyState[i] = 0;	//������Ă��Ȃ�
		}
	}
	return;
}

//�L�[�������Ă��邩�A�L�[�R�[�h�Ŕ��f����
//�����Fint : �L�[�R�[�h�FKEY_INPUT_???
BOOL MY_KEY_DOWN(int KEY_INPUT_)
{
	//�L�[�R�[�h�̃L�[�������Ă���Ƃ�
	if (AllKeyState[KEY_INPUT_] != 0)
	{
		return TRUE;	//�L�[�������Ă���
	}
	else
	{
		return FALSE;	//�L�[�������Ă��Ȃ�
	}
}

//�L�[�������グ�����A�L�[�R�[�h�Ŕ��f����
//�����Fint : �L�[�R�[�h�FKEY_INPUT_???
BOOL MY_KEY_UP(int KEY_INPUT_)
{
	if (OldAllKeyState[KEY_INPUT_] >= 1		//���O�͉����Ă���
		&& AllKeyState[KEY_INPUT_] == 0)	//���͉����Ă��Ȃ��Ƃ�
	{
		return TRUE;	//�L�[�������グ�Ă���
	}
	else
	{
		return FALSE;	//�L�[�������グ�Ă��Ȃ�
	}
}

//�L�[�����������Ă��邩�A�L�[�R�[�h�Ŕ��f����
//���@���Fint�F�L�[�R�[�h�FKEY_INPUT_???
//���@���Fint�F�L�[�����������鎞��
BOOL MY_KEYDOWN_KEEP(int KEY_INPUT_, int DownTime)
{
	//���������鎞�� = �b�� * FPS�l
	//��j60FPS�̃Q�[���ŁA1�b�ԉ�����������Ȃ�A1�b * 60FPS
	int UpdateTime = DownTime * GAME_FPS;

	if (AllKeyState[KEY_INPUT_] > UpdateTime)
	{
		return TRUE;	//���������Ă���
	}
	else
	{
		return FALSE;	//���������Ă��Ȃ�
	}

}

//�X�^�[�g���
VOID MY_START(VOID)
{
	MY_START_PROC();	//�X�^�[�g��ʂ̏���
	MY_START_DRAW();	//�X�^�[�g��ʂ̕`��

	return;

}

//�X�^�[�g��ʂ̏���
VOID MY_START_PROC(VOID)
{
	//�G���^�[�L�[�Ńv���C�V�[����
	if (MY_KEY_UP(KEY_INPUT_RETURN) == TRUE)
	{
		//�v���C�V�[���ֈړ�����
		GameScene = GAME_SCENE_PLAY;

		return;
	}
	else if (MY_KEY_DOWN(KEY_INPUT_SPACE) == TRUE)
	{
		//�����т����ֈړ�����
		GameScene = GAME_SCENE_HOWTO;

		return;
	}

	return;
}

//�X�^�[�g��ʂ̕`��
VOID MY_START_DRAW(VOID)
{
	//�w�i�`��
	DrawGraph(ImageTitleBK.x, ImageTitleBK.y, ImageTitleBK.handle, TRUE);

	//�^�C�g�����S�`��
	DrawGraph(ImageTitleLOGO.x, ImageTitleLOGO.y, ImageTitleLOGO.handle, TRUE);

	//�R�}���h��
	DrawGraph(ImageTitleStart.x, ImageTitleStart.y, ImageTitleStart.handle, TRUE);
	DrawGraph(ImageTitleEnd.x, ImageTitleEnd.y, ImageTitleEnd.handle, TRUE);
	DrawGraph(ImageTitleHowTo.x, ImageTitleHowTo.y, ImageTitleHowTo.handle, TRUE);

	DrawString(0, 0, "�X�^�[�g���", GetColor(255, 255, 255));
	return;
}

//�����т������
VOID MY_HOWTO(VOID)
{
	MY_HOWTO_PROC();	//�X�^�[�g��ʂ̏���
	MY_HOWTO_DRAW();	//�X�^�[�g��ʂ̕`��

	return;

}

//�����т�����ʂ̏���
VOID MY_HOWTO_PROC(VOID)
{
	//�G���^�[�L�[�Ń^�C�g����
	if (MY_KEY_UP(KEY_INPUT_RETURN) == TRUE)
	{
		//�^�C�g���ֈړ�����
		GameScene = GAME_SCENE_START;

		return;
	}

	return;
}

//�����т�����ʂ̕`��
VOID MY_HOWTO_DRAW(VOID)
{
	//�w�i�`��
	DrawGraph(ImageHowToBK.x, ImageHowToBK.y, ImageHowToBK.handle, TRUE);

	DrawString(0, 0, "�����т������", GetColor(255, 255, 255));
	return;
}

//�v���C���
VOID MY_PLAY(VOID)
{
	//�w�i�`��
	DrawGraph(ImagePlayBK.x, ImagePlayBK.y, ImagePlayBK.handle, TRUE);

	MY_PLAY_PROC();	//�v���C��ʂ̏���
	MY_PLAY_DRAW();	//�v���C��ʂ̕`��
	DrawString(0, 0, "�v���C���", GetColor(255, 255, 255));

	return;
}

//�v���C��ʂ̏���
VOID MY_PLAY_PROC(VOID)
{
	//�}�b�v�̊O�ɂ����瓮���Ȃ�
	if (Maguro.pos.x <= -20000.0f) { Maguro.IsMove = FALSE; }
	if (Maguro.pos.x >= 20000.0f) { Maguro.IsMove = FALSE; }

	//�`��̔��f
	for (int tate = 0; tate < MAP_TATE_MAX; tate++)
	{
		for (int yoko = 0; yoko < MAP_YOKO_MAX; yoko++)
		{
			//�ŏ��͑S�ĕ`�悷��
			/*ModelAste_ue[tate][yoko].IsDraw = TRUE;	//�G�T�ޒu���\��n
			ModelAste[tate][yoko].IsDraw = TRUE;
			ModelAste_sita[tate][yoko].IsDraw = TRUE;*/

			//�}�O���̑O�����郂�f���͕`�悵�Ȃ�
			if (MaguroToMapZ + 20 < tate)
			{
				/*ModelAste_ue[tate][yoko].IsDraw = FALSE;
				ModelAste[tate][yoko].IsDraw = FALSE;
				ModelAste_sita[tate][yoko].IsDraw = FALSE;*/
			}

			//�}�O���̌��̃��f���͕`�悵�Ȃ�
			if (MaguroToMapZ - 1 > tate)
			{
				/*ModelAste_ue[tate][yoko].IsDraw = FALSE;
				ModelAste[tate][yoko].IsDraw = FALSE;
				ModelAste_sita[tate][yoko].IsDraw = FALSE;*/
			}
		}
	}

	//�ЂƂ܂��̓X�y�[�X�L�[�ŃG���h��ʂ�
	if (MY_KEY_DOWN(KEY_INPUT_SPACE) == TRUE)
	{
		//�G���h�V�[���ֈړ�����
		GameScene = GAME_SCENE_END;

	}

	//�}�O���̈ړ�
	MY_PROC_MAGURO();

	//�}�O���̈ʒu���}�b�v�ƑΉ��t����
	MaguroToMapX = int(Maguro.pos.x / MAP_YOKO_SIZE);
	MaguroToMapZ = int(Maguro.pos.z / MAP_TATE_SIZE);

}

//�v���C��ʂ̕`��
VOID MY_PLAY_DRAW(VOID)
{
	// �J�����̃N���b�s���O������ݒ�
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	SetLightDirection(VGet(-1.0f, -1.0f, 1.0f));		// �W�����C�g�̕�����ݒ肷��
	MV1DrawModel(Maguro.handle);						//�}�O����`��

	//�f�o�b�O�p�\��
	if (DEBUG_MODE == TRUE)
	{
		DrawFormatString(0, 20, GetColor(255, 255, 255), "HAngle:%.2lf / VAngle:%.2lf", camera.HAngle, camera.VAngle);
		DrawFormatString(0, 40, GetColor(255, 255, 255), "MaguroToMapX:%2d / MaguroToMapZ:%2d", MaguroToMapX, MaguroToMapZ);
		DrawFormatString(0, 60, GetColor(255, 255, 255), "MaguroX:%.2lf / MaguroZ:%.2lf", Maguro.pos.x, Maguro.pos.z);

		//�f�o�b�O�p�̃J�v�Z���̕\��
		//int Bunkatsu = 8;
		//DrawCapsule3D(MaguroCollVecStart, MaguroCollVecEnd, MaguroCollRadius, Bunkatsu, GetColor(255, 255, 255), GetColor(255, 255, 255), FALSE);
	
		//�f�o�b�O�p�G�r
		MV1DrawModel(Ebi.handle);
	
	}

	return;
}

//�J�����̏���
VOID MY_PROC_MAGURO(VOID)
{
	//�}�O���̈ړ��ʂ�������
	Maguro.move = VGet(0.0f, 0.0f, 0.0f);

	if (Maguro.IsMove == TRUE)
	{
			//�}�O���������X�N���[��
			Maguro.move = VGet(0.0f, 0.0f, MAGURO_MOVE);
	}

	//���։�]
	if (MY_KEY_DOWN(KEY_INPUT_A) == TRUE)
	{
		//�p�x��ς���
		camera.HAngle += CAMERA_ANGLE_X_PLUS;
	}

	//�E�։�]
	if (MY_KEY_DOWN(KEY_INPUT_D) == TRUE)
	{
		//�p�x��ς���
		camera.HAngle -= CAMERA_ANGLE_X_PLUS;
	}

	//�J�����̌����ɍ��킹�āA�}�O�����ړ�������
	double SinParam = sin(double(camera.HAngle / 180.0f) * DX_PI_F);
	double CosParam = cos(double(camera.HAngle / 180.0f) * DX_PI_F);
	VECTOR TempMoveVector;
	TempMoveVector.x = Maguro.move.x * CosParam - Maguro.move.z * SinParam;
	TempMoveVector.y = 0.0f;
	TempMoveVector.z = Maguro.move.x * SinParam + Maguro.move.z * CosParam;
	Maguro.pos = VAdd(Maguro.pos, TempMoveVector);

	// �V�����������Z�b�g
	MV1SetRotationXYZ(Maguro.handle,
		VGet(
			0.0f,
			-(camera.HAngle / 180.0f * DX_PI_F),
			0.0f
		)
	);

	//�}�O���̈ʒu��ݒ�
	MV1SetPosition(Maguro.handle, Maguro.pos);

	//�J�����̐ݒ�
	VECTOR TempPosCalc1;
	VECTOR TempPosCalc2;
	VECTOR CameraLookAtPos;

	// �����_�̓L�����N�^�[���f���̍��W���� CAMERA_HEIGHT �����������ʒu
	CameraLookAtPos = Maguro.pos;
	CameraLookAtPos.y += CAMERA_HEIGHT;

	// �ŏ��ɐ����p�x�𔽉f�����ʒu���Z�o
	Maguro.VecSin = sin(double(camera.VAngle / 180.0f) * DX_PI_F);
	Maguro.VecCos = cos(double(camera.VAngle / 180.0f) * DX_PI_F);
	TempPosCalc1.x = 0.0f;
	TempPosCalc1.y = Maguro.VecSin * CAMERA_DISTANCE;
	TempPosCalc1.z = -Maguro.VecCos * CAMERA_DISTANCE;

	// ���ɐ����p�x�𔽉f�����ʒu���Z�o
	Maguro.VecSin = sin(double(camera.HAngle / 180.0f) * DX_PI_F);
	Maguro.VecCos = cos(double(camera.HAngle / 180.0f) * DX_PI_F);
	TempPosCalc2.x = Maguro.VecCos * TempPosCalc1.x - Maguro.VecSin * TempPosCalc1.z;
	TempPosCalc2.y = TempPosCalc1.y;
	TempPosCalc2.z = Maguro.VecSin * TempPosCalc1.x + Maguro.VecCos * TempPosCalc1.z;

	// �Z�o�������W�ɒ����_�̈ʒu�����Z�������̂��J�����̈ʒu
	camera.pos = VAdd(TempPosCalc2, CameraLookAtPos);

	// �J�����̐ݒ�ɔ��f����
	SetCameraPositionAndTarget_UpVecY(camera.pos, CameraLookAtPos);

	return;
}

//�G���h���
VOID MY_END(VOID)
{
	MY_END_PROC();	//�G���h��ʂ̏���
	MY_END_DRAW();	//�G���h��ʂ̕`��
	return;
}

//�G���h��ʂ̏���
VOID MY_END_PROC(VOID)
{
	//�G���^�[�L�[����������A�^�C�g���V�[���ֈړ�����
	if (MY_KEY_UP(KEY_INPUT_RETURN) == TRUE)
	{
		//�Q�[��������
		MY_GAME_INIT();

		//�^�C�g���V�[���ֈړ�����
		GameScene = GAME_SCENE_START;

		return;
	}

	return;
}

//�G���h��ʂ̕`��
VOID MY_END_DRAW(VOID)
{
	MY_PLAY_DRAW();	//�v���C��ʂ̕`��
	DrawString(0, 0, "�G���h���(�G���^�[�L�[�������Ă�������)", GetColor(255, 255, 255));

	return;
}

//�Q�[�����������
BOOL MY_GAME_INIT(VOID)
{

	return TRUE;
}

//�摜���܂Ƃ߂ēǂݍ��ފ֐�
BOOL MY_LOAD_IMAGE(VOID)
{
	//�^�C�g���w�i
	wsprintf(ImageTitleBK.path, IMAGE_TITLE_BK_PATH);	//�t�@�C���p�X���R�s�[
	ImageTitleBK.handle = LoadGraph(ImageTitleBK.path);	//�摜���������ɓǂݍ��݁A�n���h�����擾
	if (ImageTitleBK.handle == -1) { MessageBox(GetMainWindowHandle(), ImageTitleBK.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//�摜�T�C�Y���擾
	GetGraphSize(ImageTitleBK.handle, &ImageTitleBK.width, &ImageTitleBK.height);
	ImageTitleBK.x = GAME_WIDTH / 2 - ImageTitleBK.width / 2;	//��������
	ImageTitleBK.y = GAME_HEIGHT / 2 - ImageTitleBK.height / 2;	//��������

	//�^�C�g�����S
	wsprintf(ImageTitleLOGO.path, IMAGE_TITLE_LOGO_PATH);	//�t�@�C���p�X���R�s�[
	ImageTitleLOGO.handle = LoadGraph(ImageTitleLOGO.path);	//�摜���������ɓǂݍ��݁A�n���h�����擾
	if (ImageTitleLOGO.handle == -1) { MessageBox(GetMainWindowHandle(), ImageTitleLOGO.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//�摜�T�C�Y���擾
	GetGraphSize(ImageTitleLOGO.handle, &ImageTitleLOGO.width, &ImageTitleLOGO.height);
	ImageTitleLOGO.x = 151.0;	//X���W
	ImageTitleLOGO.y = 50.0;	//Y���W

	//�͂��߂�
	wsprintf(ImageTitleStart.path, IMAGE_TITLE_START_PATH);	//�t�@�C���p�X���R�s�[
	ImageTitleStart.handle = LoadGraph(ImageTitleStart.path);	//�摜���������ɓǂݍ��݁A�n���h�����擾
	if (ImageTitleStart.handle == -1) { MessageBox(GetMainWindowHandle(), ImageTitleStart.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//�摜�T�C�Y���擾
	GetGraphSize(ImageTitleStart.handle, &ImageTitleStart.width, &ImageTitleStart.height);
	ImageTitleStart.x = 145.5;	//X���W
	ImageTitleStart.y = 445.0;	//Y���W

	//�����
	wsprintf(ImageTitleEnd.path, IMAGE_TITLE_END_PATH);	//�t�@�C���p�X���R�s�[
	ImageTitleEnd.handle = LoadGraph(ImageTitleEnd.path);	//�摜���������ɓǂݍ��݁A�n���h�����擾
	if (ImageTitleEnd.handle == -1) { MessageBox(GetMainWindowHandle(), ImageTitleEnd.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//�摜�T�C�Y���擾
	GetGraphSize(ImageTitleEnd.handle, &ImageTitleEnd.width, &ImageTitleEnd.height);
	ImageTitleEnd.x = 422.0;	//X���W
	ImageTitleEnd.y = 445.0;	//Y���W

	//�����т���
	wsprintf(ImageTitleHowTo.path, IMAGE_TITLE_HOWTO_PATH);	//�t�@�C���p�X���R�s�[
	ImageTitleHowTo.handle = LoadGraph(ImageTitleHowTo.path);	//�摜���������ɓǂݍ��݁A�n���h�����擾
	if (ImageTitleHowTo.handle == -1) { MessageBox(GetMainWindowHandle(), ImageTitleHowTo.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//�摜�T�C�Y���擾
	GetGraphSize(ImageTitleHowTo.handle, &ImageTitleHowTo.width, &ImageTitleHowTo.height);
	ImageTitleHowTo.x = 654.5;	//X���W
	ImageTitleHowTo.y = 445.0;	//Y���W

	//�����т����w�i
	wsprintf(ImageHowToBK.path, IMAGE_HOWTO_BK_PATH);	//�t�@�C���p�X���R�s�[
	ImageHowToBK.handle = LoadGraph(ImageHowToBK.path);	//�摜���������ɓǂݍ��݁A�n���h�����擾
	if (ImageHowToBK.handle == -1) { MessageBox(GetMainWindowHandle(), ImageHowToBK.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//�摜�T�C�Y���擾
	GetGraphSize(ImageHowToBK.handle, &ImageHowToBK.width, &ImageHowToBK.height);
	ImageHowToBK.x = GAME_WIDTH / 2 - ImageHowToBK.width / 2;	//��������
	ImageHowToBK.y = GAME_HEIGHT / 2 - ImageHowToBK.height / 2;	//��������

	//�v���C�w�i
	wsprintf(ImagePlayBK.path, IMAGE_PLAY_BK_PATH);	//�t�@�C���p�X���R�s�[
	ImagePlayBK.handle = LoadGraph(ImagePlayBK.path);	//�摜���������ɓǂݍ��݁A�n���h�����擾
	if (ImagePlayBK.handle == -1) { MessageBox(GetMainWindowHandle(), ImagePlayBK.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//�摜�T�C�Y���擾
	GetGraphSize(ImagePlayBK.handle, &ImagePlayBK.width, &ImagePlayBK.height);
	ImagePlayBK.x = GAME_WIDTH / 2 - ImagePlayBK.width / 2;	//��������
	ImagePlayBK.y = GAME_HEIGHT / 2 - ImagePlayBK.height / 2;	//��������

	return TRUE;
}

//�摜���܂Ƃ߂č폜����֐�
VOID MY_DELETE_IMAGE(VOID)
{
	DeleteGraph(ImageTitleBK.handle);
	DeleteGraph(ImageTitleLOGO.handle);
	DeleteGraph(ImageTitleStart.handle);
	DeleteGraph(ImageTitleEnd.handle);
	DeleteGraph(ImageTitleHowTo.handle);
	DeleteGraph(ImagePlayBK.handle);

	return;
}

//3D���f�����܂Ƃ߂ēǂݍ��ފ֐�
BOOL MY_LOAD_MODEL(VOID)
{
	//�}�O����ǂݍ���
	strcpyDx(Maguro.path, MODEL_MAGURO_PATH);		//�p�X�̃R�s�[
	Maguro.handle = MV1LoadModel(Maguro.path);		//���f���ǂݍ���
	//�ǂݍ��݃G���[
	if (Maguro.handle == -1) 
	{ 
		MessageBox(
			GetMainWindowHandle(),
			Maguro.path,
			ERR_LOAD_TITLE_MODEL,
			MB_OK
		);

		return FALSE; 
	}

	//�G�r��ǂݍ���
	strcpyDx(Ebi.path, MODEL_EBI_PATH);		//�p�X�̃R�s�[
	Ebi.handle = MV1LoadModel(Ebi.path);		//���f���ǂݍ���
	//�ǂݍ��݃G���[
	if (Ebi.handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),
			Ebi.path,
			ERR_LOAD_TITLE_MODEL,
			MB_OK
		);

		return FALSE;
	}

	return TRUE;
}

//3D���f�����܂Ƃ߂č폜����֐�
VOID MY_DELETE_MODEL(VOID)
{
	MV1DeleteModel(Maguro.handle);
	MV1DeleteModel(Ebi.handle);

	return;
}

