#include "DxLib.h"

//########## マクロ定義 ##########
#define GAME_WIDTH			1000	//画面の横の大きさ
#define GAME_HEIGHT			600	//画面の縦の大きさ
#define GAME_COLOR			32	//画面のカラービット

#define GAME_WINDOW_BAR		0				//タイトルバーはデフォルトにする
#define GAME_WINDOW_NAME	"GAME TITLE"	//ウィンドウのタイトル

//########## グローバル変数 ##########
int ModelHandle;						//3Dモデルのハンドルを入れる
char AllKeyState[256] = { '\0' };		//すべてのキーの状態(直前)が入る

//int MaguroScale = 0.05;				//最初のマグロの大きさ

VECTOR ModelPos = { 0.0f,0.0f,0.0f };	//モデルの座標を管理

VECTOR ModelScale = { 0.5,0.5,0.5 };//最初のマグロの大きさ

//########## プロトタイプ宣言 ##########
VOID MY_ALL_KEYDOWN_UPDATE(VOID);	//キーの入力状態を更新する

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE);								//ウィンドウモードに設定
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//指定の数値でウィンドウを表示する
	SetWindowStyleMode(GAME_WINDOW_BAR);				//タイトルバーはデフォルトにする
	SetMainWindowText(TEXT(GAME_WINDOW_NAME));			//ウィンドウのタイトルの文字
	SetAlwaysRunFlag(TRUE);								//非アクティブでも実行する

	//ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1) { return -1; }

	//Draw系関数は裏画面に描画
	SetDrawScreen(DX_SCREEN_BACK);

	//３Ｄモデルの読み込み
	//ModelHandle = MV1LoadModel("cube.mv1");
	ModelHandle = MV1LoadModel("mqoモデル\\マグロ\\マグロ.mqo");


	//画面に映る位置に３Ｄモデルを移動
	ModelPos = VGet(500.0f, 300.0f, 0.0f);
	//右,上,前後？
	//DXライブラリは左下が原点

	ModelScale = VGet(0.5f, 0.5f, 0.5f);

	//無限ループ
	while (TRUE)
	{
		//メッセージ処理の結果がエラーのとき、強制終了
		if (ProcessMessage() != 0) { break; }

		//画面を消去できなかったとき、強制終了
		if (ClearDrawScreen() != 0) { break; }

		//押しているキー状態を取得
		MY_ALL_KEYDOWN_UPDATE();

		//ESCキーを押すと、無限ループを終了する
		if (AllKeyState[KEY_INPUT_ESCAPE] != 0) { break; }

		//Wキーを押すと、モデルが上に移動する
		if (AllKeyState[KEY_INPUT_W] != 0) { ModelPos.y += 10.0f; }

		//Sキーを押すと、モデルが下に移動する
		if (AllKeyState[KEY_INPUT_S] != 0) { ModelPos.y -= 10.0f; }

		//Dキーを押すと、モデルが右に移動する
		if (AllKeyState[KEY_INPUT_D] != 0) { ModelPos.x += 10.0f; }

		//Aキーを押すと、モデルが左に移動する
		if (AllKeyState[KEY_INPUT_A] != 0) { ModelPos.x -= 10.0f; }

		DrawFormatString(0, 0, GetColor(255, 255, 255), "x : %lf", ModelPos.x);
		DrawFormatString(0, 20, GetColor(255, 255, 255), "y : %lf", ModelPos.y);
		DrawFormatString(0, 40, GetColor(255, 255, 255), "z : %lf", ModelPos.z);

		//画面に映る位置に３Ｄモデルを移動
		MV1SetPosition(ModelHandle, ModelPos);

		MV1SetScale(ModelHandle, ModelScale);

		// ３Ｄモデルの描画
		MV1DrawModel(ModelHandle);

		//モニタのリフレッシュレートの速さで裏画面を再描画
		ScreenFlip();

	}

	// モデルハンドルの削除
	MV1DeleteModel(ModelHandle);

	// ＤＸライブラリの後始末
	DxLib_End();

	// ソフトの終了
	return 0;
}

//########## キーの入力状態を更新する関数 ##########
VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{
	//参考：https://dxlib.xsrv.jp/function/dxfunc_input.html
	char TempKey[256];	//一時的に、現在のキーの入力状態を格納する

	GetHitKeyStateAll(TempKey); // 全てのキーの入力状態を得る

	for (int i = 0; i < 256; i++)
	{
		if (TempKey[i] != 0)	//押されているキーのキーコードを押しているとき
		{
			AllKeyState[i]++;	//押されている
		}
		else
		{
			AllKeyState[i] = 0;	//押されていない
		}
	}
	return;
}