#include "DxLib.h"
#include <math.h>

//########## マクロ定義 ##########
#define GAME_WIDTH			1000	//画面の横の大きさ
#define GAME_HEIGHT			600	//画面の縦の大きさ
#define GAME_COLOR			32	//画面のカラービット

#define GAME_WINDOW_BAR		0				//タイトルバーはデフォルトにする
#define GAME_WINDOW_NAME	"I am MAGURO"	//ウィンドウのタイトル

#define GAME_FPS			60		//FPS

//パスの長さ  255文字まで
#define PATH_MAX	255
#define NAME_MAX	255

//3Dモデルのパス
#define MODEL_MAGURO_PATH	TEXT(".\\mqoモデル\\マグロ\\マグロ.mqo")
#define MODEL_EBI_PATH		TEXT(".\\mqoモデル\\エビ\\エビ.mqo")
#define MODEL_IKA_PATH		TEXT(".\\mqoモデル\\イカ\\イカ.mqo")
#define MODEL_PURA_PATH		TEXT(".\\mqoモデル\\プランクトン\\プランクトン.mqo")

//画像のパス(仮置き)
#define IMAGE_TITLE_BK_PATH		//タイトルの背景画像
#define IMAGE_TITLE_LOGO_PATH	//タイトルロゴ
#define IMAGE_TITLE_START_PATH	//はじめる
#define IMAGE_TITLE_END_PATH	//おわる
#define IMAGE_TITLE_HOWTO_PATH	//あそびかた

//カメラの設定
#define CAMERA_NEAR			1.0f		//どこまで近くを写すか
#define CAMERA_FAR			60000.0f	//どこまで遠くを写すか
#define CAMERA_HEIGHT		300.0f		//カメラの注視点の高さ
#define CAMERA_DISTANCE		10.0f		//カメラと注視点の距離
#define CAMERA_INIT_V_ANGLE	0.0f		//カメラの向きデフォルト(垂直) / -90が上 90が下
#define CAMERA_INIT_H_ANGLE	0.0f		//カメラの向きデフォルト(水平) / -90が左 90が右
#define CAMERA_ANGLE_X_PLUS	1.0f		//カメラの角度(X)の加算値
#define CAMERA_ANGLE_MAX_X	90.0f		//カメラのMAX角度(X)
#define CAMERA_ANGLE_MAX_Y	90.0f		//カメラのMAX角度(Y)

//マップ
#define MAP_TATE_MAX	150
#define MAP_YOKO_MAX	150

#define MAP_TATE_SIZE	(100.0f * 2.0f) //間隔を2.0倍開ける
#define MAP_YOKO_SIZE	(100.0f * 2.0f)

//マグロ関係
#define MAGURO_MOVE			10.0f	//スピード
#define MAGURO_CAP_RADIUS	30.0f	//当たり判定のカプセルの半径

//エラーメッセージ
#define ERR_LOAD_TITLE_IMAGE	TEXT("画像読み込みエラー")
#define ERR_LOAD_TITLE_MODEL	TEXT("モデル読み込みエラー")

//終了ダイアログ用
#define CLOSE_WINDOW_TITLE		TEXT("ゲーム終了")
#define CLOSE_WINDOW_CAPTION	TEXT("ゲームを終了しますか？")

//ゲームのシーン
enum GAME_SCENE
{
	GAME_SCENE_START,
	GAME_SCENE_PLAY,
	GAME_SCENE_END
};

//int型のPOINT構造体
typedef struct STRUCT_I_POINT
{
	//座標を初期化
	int x = -1;
	int y = -1;

}iPOINT;

//画像構造体
typedef struct STRUCT_IMAGE
{
	char path[PATH_MAX] = { '\0' };	//パス
	int handle = -1;				//ハンドル
	int x = 0;						//X位置
	int y = 0;						//Y位置
	int width = 0;					//幅
	int height = 0;					//高さ
	BOOL IsDraw = FALSE;			//描画できるか(デフォルトはFALSE)

}IMAGE;

//3Dモデル構造体
typedef struct STRUCT_MODEL
{
	char path[PATH_MAX] = { '\0' };		//パス
	int handle = -1;					//ハンドル
	VECTOR pos = { 0.0f,0.0f,0.0f };	//モデルの位置
	VECTOR move = { 0.0f,0.0f,0.0f };	//モデルの動作
	float angle = 0.0f;		//モデルの回転
	BOOL IsDraw = FALSE;	//描画できるか(デフォルトはFALSE)
	double VecSin = 0.0f;	//プレイヤーの向いている向き：弧度(sin = Y)
	double VecCos = 0.0f;	//プレイヤーの向いている向き：弧度(cos = X)
	float HAngle = 0.0f;	//水平角度
	BOOL IsMove = TRUE;		//動けるか

}MODEL;

//カメラ構造体
typedef struct STRUCT_CAMERA
{
	float CamNear = 0.0f;				//どこまで近くを写すか
	float CamFar = 0.0f;				//どこまで遠くを写すか
	float CamHeight = 0.0f;				//カメラの高さの位置
	float VAngle = 0.0;					//カメラの垂直の角度
	float HAngle = 0.0;					//カメラの水平の角度
	VECTOR pos = { 0.0f,0.0f,0.0f };	//カメラの場所
	float angle = 90.0f;				//カメラの角度

}CAMERA;

//########## グローバル変数 ##########

//FPS関連
int StartTimeFps;				//測定開始時刻
int CountFps;					//カウンタ
float CalcFps;					//計算結果
int SampleNumFps = GAME_FPS;	//平均を取るサンプル数

//キーボードの入力を取得
char AllKeyState[256] = { '\0' };		//すべてのキーの状態が入る
char OldAllKeyState[256] = { '\0' };	//すべてのキーの状態(直前)が入る

//ゲームシーンを管理
int GameScene;

//カメラ
CAMERA camera;

//モデル
MODEL Maguro;	//マグロ
MODEL Ebi;		//エビ
MODEL Ika;		//イカ
MODEL Pura;		//プランクトン

//画像
IMAGE ImageTitleBK;		//仮置き
IMAGE ImageTitleLOGO;
IMAGE ImageTitleStart;
IMAGE ImageTitleEnd;
IMAGE ImageTitleHowTo;

//マグロの位置をマップと対応付ける
int MaguroToMapX;
int MaguroToMapZ;

//マグロの当たり判定(カプセル)
VECTOR MaguroCollVecStart;						//カプセルの始点
VECTOR MaguroCollVecEnd;						//カプセルの終点
float MaguroCollRadius = MAGURO_CAP_RADIUS;		//カプセルの半径

//マグロが動けたときの座標を記憶
VECTOR MaguroMovePos;

//int MaguroScale = 0.05;				//最初のマグロの大きさ
//
//VECTOR ModelPos = { 0.0f,0.0f,0.0f };	//モデルの座標を管理
//
//VECTOR ModelScale = { 0.5f,0.5f,0.5f };//最初のマグロの大きさ
//
//VECTOR AddScale_pla = { 1.05f,1.05f,1.05f };	//プランクトンを食べたときに加算
//
//VECTOR AddScale_ebi = { 1.1f,1.1f,1.1f };	//エビを食べたときに加算
//
//VECTOR AddScale_ika = { 1.3f,1.3f,1.3f };	//イカを食べたときに加算
//
//MV1_COLL_RESULT_POLY HitPoly;		//当たり判定？

//########## プロトタイプ宣言 ##########

VOID MY_FPS_UPDATE(VOID);		//FPS値を計測、更新する
VOID MY_FPS_DRAW(VOID);			//FPS値を描画する
VOID MY_FPS_WAIT(VOID);			//FPS値を計測し、待つ

VOID MY_ALL_KEYDOWN_UPDATE(VOID);	//キーの入力状態を更新する
BOOL MY_KEY_DOWN(int);				//キーを押しているか、キーコードで判断する
BOOL MY_KEY_UP(int);				//キーを押し上げたか、キーコードで判断する
BOOL MY_KEYDOWN_KEEP(int, int);		//キーを押し続けているか、キーコードで判断する

BOOL MY_GAME_INIT(VOID);	//ゲーム初期化画面

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
	ModelHandle = MV1LoadModel(".\\mqoモデル\\マグロ\\マグロ.mqo");


	//画面に映る位置に３Ｄモデルを移動
	ModelPos = VGet(500.0f, 300.0f, 0.0f);
	//右,上,前後？
	//DXライブラリは左下が原点

	ModelScale = VGet(0.3f, 0.3f, 0.3f);



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

		//マグロを常に奥へ
		ModelPos.z += 10.0f;

		//画面に映る位置に３Ｄモデルを移動
		MV1SetPosition(ModelHandle, ModelPos);

		MV1SetScale(ModelHandle, ModelScale);

		MV1SetRotationXYZ(ModelHandle, VGet(0.0f, 180.0f * DX_PI_F / 180.0f, 0.0f));

		//モデルの当たり判定
		MV1SetupCollInfo(ModelHandle, 0, 1, 1, 1);

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