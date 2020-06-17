#include "DxLib.h"

//########## �}�N����` ##########
#define GAME_WIDTH			1000	//��ʂ̉��̑傫��
#define GAME_HEIGHT			600	//��ʂ̏c�̑傫��
#define GAME_COLOR			32	//��ʂ̃J���[�r�b�g

#define GAME_WINDOW_BAR		0				//�^�C�g���o�[�̓f�t�H���g�ɂ���
#define GAME_WINDOW_NAME	"GAME TITLE"	//�E�B���h�E�̃^�C�g��

//########## �O���[�o���ϐ� ##########
int ModelHandle;						//3D���f���̃n���h��������
char AllKeyState[256] = { '\0' };		//���ׂẴL�[�̏��(���O)������

//int MaguroScale = 0.05;				//�ŏ��̃}�O���̑傫��

VECTOR ModelPos = { 0.0f,0.0f,0.0f };	//���f���̍��W���Ǘ�

VECTOR ModelScale = { 0.5,0.5,0.5 };//�ŏ��̃}�O���̑傫��

//########## �v���g�^�C�v�錾 ##########
VOID MY_ALL_KEYDOWN_UPDATE(VOID);	//�L�[�̓��͏�Ԃ��X�V����

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
	ModelHandle = MV1LoadModel("mqo���f��\\�}�O��\\�}�O��.mqo");


	//��ʂɉf��ʒu�ɂR�c���f�����ړ�
	ModelPos = VGet(500.0f, 300.0f, 0.0f);
	//�E,��,�O��H
	//DX���C�u�����͍��������_

	ModelScale = VGet(0.5f, 0.5f, 0.5f);

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

		//��ʂɉf��ʒu�ɂR�c���f�����ړ�
		MV1SetPosition(ModelHandle, ModelPos);

		MV1SetScale(ModelHandle, ModelScale);

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