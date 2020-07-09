#include "DxLib.h"
#include <math.h>

//########## �}�N����` ##########
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
#define IMAGE_TITLE_BK_PATH		//�^�C�g���̔w�i�摜
#define IMAGE_TITLE_LOGO_PATH	//�^�C�g�����S
#define IMAGE_TITLE_START_PATH	//�͂��߂�
#define IMAGE_TITLE_END_PATH	//�����
#define IMAGE_TITLE_HOWTO_PATH	//�����т���

//�J�����̐ݒ�
#define CAMERA_NEAR			1.0f		//�ǂ��܂ŋ߂����ʂ���
#define CAMERA_FAR			60000.0f	//�ǂ��܂ŉ������ʂ���
#define CAMERA_HEIGHT		300.0f		//�J�����̒����_�̍���
#define CAMERA_DISTANCE		10.0f		//�J�����ƒ����_�̋���
#define CAMERA_INIT_V_ANGLE	0.0f		//�J�����̌����f�t�H���g(����) / -90���� 90����
#define CAMERA_INIT_H_ANGLE	0.0f		//�J�����̌����f�t�H���g(����) / -90���� 90���E
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

VOID MY_START(VOID);
VOID MY_START_PROC(VOID);
VOID MY_START_DRAW(VOID);

VOID MY_PLAY(VOID);
VOID MY_PLAY_PROC(VOID);
VOID MY_PLAY_DRAW(VOID);

VOID MY_END(VOID);
VOID MY_END_PROC(VOID);
VOID MY_END_DRAW(VOID);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE);								//�E�B���h�E���[�h�ɐݒ�
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//�w��̐��l�ŃE�B���h�E��\������
	SetWindowStyleMode(GAME_WINDOW_BAR);				//�^�C�g���o�[�̓f�t�H���g�ɂ���
	SetMainWindowText(TEXT(GAME_WINDOW_NAME));			//�E�B���h�E�̃^�C�g���̕���
	SetAlwaysRunFlag(TRUE);								//��A�N�e�B�u�ł����s����

	//�c�w���C�u��������������
	if (DxLib_Init() == -1) { return -1; }

	//Draw�n�֐��͗���ʂɕ`��
	SetDrawScreen(DX_SCREEN_BACK);

	//�R�c���f���̓ǂݍ���
	//ModelHandle = MV1LoadModel("cube.mv1");
	ModelHandle = MV1LoadModel(".\\mqo���f��\\�}�O��\\�}�O��.mqo");


	//��ʂɉf��ʒu�ɂR�c���f�����ړ�
	ModelPos = VGet(500.0f, 300.0f, 0.0f);
	//�E,��,�O��H
	//DX���C�u�����͍��������_

	ModelScale = VGet(0.3f, 0.3f, 0.3f);



	//�������[�v
	while (TRUE)
	{
		//���b�Z�[�W�����̌��ʂ��G���[�̂Ƃ��A�����I��
		if (ProcessMessage() != 0) { break; }

		//��ʂ������ł��Ȃ������Ƃ��A�����I��
		if (ClearDrawScreen() != 0) { break; }

		//�����Ă���L�[��Ԃ��擾
		MY_ALL_KEYDOWN_UPDATE();

		//ESC�L�[�������ƁA�������[�v���I������
		if (AllKeyState[KEY_INPUT_ESCAPE] != 0) { break; }

		//W�L�[�������ƁA���f������Ɉړ�����
		if (AllKeyState[KEY_INPUT_W] != 0) { ModelPos.y += 10.0f; }

		//S�L�[�������ƁA���f�������Ɉړ�����
		if (AllKeyState[KEY_INPUT_S] != 0) { ModelPos.y -= 10.0f; }

		//D�L�[�������ƁA���f�����E�Ɉړ�����
		if (AllKeyState[KEY_INPUT_D] != 0) { ModelPos.x += 10.0f; }

		//A�L�[�������ƁA���f�������Ɉړ�����
		if (AllKeyState[KEY_INPUT_A] != 0) { ModelPos.x -= 10.0f; }

		DrawFormatString(0, 0, GetColor(255, 255, 255), "x : %lf", ModelPos.x);
		DrawFormatString(0, 20, GetColor(255, 255, 255), "y : %lf", ModelPos.y);
		DrawFormatString(0, 40, GetColor(255, 255, 255), "z : %lf", ModelPos.z);

		//�}�O������ɉ���
		ModelPos.z += 10.0f;

		//��ʂɉf��ʒu�ɂR�c���f�����ړ�
		MV1SetPosition(ModelHandle, ModelPos);

		MV1SetScale(ModelHandle, ModelScale);

		MV1SetRotationXYZ(ModelHandle, VGet(0.0f, 180.0f * DX_PI_F / 180.0f, 0.0f));

		//���f���̓����蔻��
		MV1SetupCollInfo(ModelHandle, 0, 1, 1, 1);

		// �R�c���f���̕`��
		MV1DrawModel(ModelHandle);

		//���j�^�̃��t���b�V�����[�g�̑����ŗ���ʂ��ĕ`��
		ScreenFlip();

	}

	// ���f���n���h���̍폜
	MV1DeleteModel(ModelHandle);

	// �c�w���C�u�����̌�n��
	DxLib_End();

	// �\�t�g�̏I��
	return 0;
}

//########## �L�[�̓��͏�Ԃ��X�V����֐� ##########
VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{
	//�Q�l�Fhttps://dxlib.xsrv.jp/function/dxfunc_input.html
	char TempKey[256];	//�ꎞ�I�ɁA���݂̃L�[�̓��͏�Ԃ��i�[����

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