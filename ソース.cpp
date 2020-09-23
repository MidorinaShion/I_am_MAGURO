#include "DxLib.h"
#include <math.h>

//########## マクロ定義 ##########

#define DEBUG_MODE			FALSE	//デバッグモード

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
//#define MODEL_MAGURO_PATH	TEXT(".\\mqoモデル\\マグロ\\マグロ.mqo")
//#define MODEL_EBI_PATH		TEXT(".\\mqoモデル\\エビ\\エビ.mqo")
//#define MODEL_IKA_PATH		TEXT(".\\mqoモデル\\イカ\\イカ.mqo")
//#define MODEL_PURA_PATH		TEXT(".\\mqoモデル\\プランクトン\\プランクトン.mqo")

#define MODEL_MAGURO_PATH	TEXT(".\\models\\Maguro.mqo")
#define MODEL_EBI_PATH		TEXT(".\\models\\Ebi.mqo")
#define MODEL_IKA_PATH		TEXT(".\\models\\Ika.mqo")
#define MODEL_PURA_PATH		TEXT(".\\models\\Pura.mqo")

//画像のパス(仮置き)
#define IMAGE_TITLE_BK_PATH		TEXT(".\\画像\\タイトル背景.png")
#define IMAGE_TITLE_LOGO_PATH	TEXT(".\\画像\\タイトルロゴ.png")
#define IMAGE_TITLE_START_PATH	TEXT(".\\画像\\はじめる.png")
#define IMAGE_TITLE_END_PATH	TEXT(".\\画像\\おわる1.png")
#define IMAGE_TITLE_HOWTO_PATH	TEXT(".\\画像\\あそびかた.png")
#define IMAGE_HOWTO_BK_PATH		TEXT(".\\画像\\あそびかた背景.png")

#define IMAGE_PLAY_BK_PATH		TEXT(".\\画像\\プレイ背景.png")

#define IMAGE_END_LOGO_PATH		TEXT(".\\画像\\残念3.png")
#define IMAGE_END_CONTENUE_PATH	TEXT(".\\画像\\つづける.png")
#define IMAGE_END_END_PATH		TEXT(".\\画像\\おわる2.png")

//カメラの設定
#define CAMERA_NEAR			50.0f		//どこまで近くを写すか
#define CAMERA_FAR			60000.0f	//どこまで遠くを写すか
#define CAMERA_HEIGHT		0.0f		//カメラの注視点の高さ
#define CAMERA_DISTANCE		700.0f		//カメラと注視点の距離
#define CAMERA_INIT_V_ANGLE	0.0f		//カメラの向きデフォルト(垂直) / -90が上 90が下
#define CAMERA_INIT_H_ANGLE	180.0f		//カメラの向きデフォルト(水平) / -90が左 90が右
#define CAMERA_ANGLE_X_PLUS	1.0f		//カメラの角度(X)の加算値
#define CAMERA_ANGLE_MAX_X	90.0f		//カメラのMAX角度(X)
#define CAMERA_ANGLE_MAX_Y	90.0f		//カメラのMAX角度(Y)

//マップ
#define MAP_TATE_MAX	15000
#define MAP_YOKO_MAX	15000

#define MAP_TATE_SIZE	(100.0f * 2.0f) //間隔を2.0倍開ける
#define MAP_YOKO_SIZE	(100.0f * 2.0f)

//マグロ関係
#define MAGURO_MOVE			10.0f	//スピード
//#define MAGURO_CAP_RADIUS	30.0f	//当たり判定のカプセルの半径

//エサの数
#define EBI_MAX		50
#define PURA_MAX	100
#define IKA_MAX		30

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
	GAME_SCENE_HOWTO,
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
MODEL Ebi[EBI_MAX];		//エビ
MODEL Ika[IKA_MAX];		//イカ
MODEL Pura[PURA_MAX];		//プランクトン

//画像
IMAGE ImageTitleBK;		
IMAGE ImageTitleLOGO;
IMAGE ImageTitleStart;
IMAGE ImageTitleEnd;
IMAGE ImageTitleHowTo;
IMAGE ImageHowToBK;

IMAGE ImagePlayBK;

IMAGE ImageEndLOGO;
IMAGE ImageEndContenue;
IMAGE ImageEndEnd;

//マグロの位置をマップと対応付ける
int MaguroToMapX;
int MaguroToMapZ;

//マグロが動けたときの座標を記憶
VECTOR MaguroMovePos;

//マグロのパラメータ
//float Maguro_O2 = 100.0;	//酸素
//float Maguro_Hp = 1000.0;	//空腹度
float Maguro_Size = 0.5f;		//初期の大きさ

//エサの大きさ
float Pura_Size = 0.2f;
float Ebi_Size = 0.8f;
float Ika_Size = 2.0f;

//マグロの当たり判定(カプセル)
VECTOR MaguroCollVecStart;						//カプセルの始点
VECTOR MaguroCollVecEnd;						//カプセルの終点

float MaguroCollRadius = Maguro_Size * 70;		//カプセルの半径

VECTOR MaguroScale = { Maguro_Size,Maguro_Size,Maguro_Size };//最初のマグロの大きさ

//エサのサイズ指定
VECTOR PuraScale = { Pura_Size,Pura_Size,Pura_Size };	//プランクトン
VECTOR EbiScale = { Ebi_Size,Ebi_Size,Ebi_Size };	//エビ
VECTOR IkaScale = { Ika_Size,Ika_Size,Ika_Size };	//イカ

//カメラの距離
float Cam_Dist = CAMERA_DISTANCE;

//VECTOR AddScale_pla = { 1.05f,1.05f,1.05f };	//プランクトンを食べたときに加算

//VECTOR AddScale_ebi = { 1.1f,1.1f,1.1f };	//エビを食べたときに加算

//VECTOR AddScale_ika = { 1.3f,1.3f,1.3f };	//イカを食べたときに加算

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

VOID MY_START(VOID);		//スタート画面
VOID MY_START_PROC(VOID);	//スタート画面の処理
VOID MY_START_DRAW(VOID);	//スタート画面の描画

VOID MY_HOWTO(VOID);		//あそびかた画面
VOID MY_HOWTO_PROC(VOID);	
VOID MY_HOWTO_DRAW(VOID);	

VOID MY_PLAY(VOID);			//プレイ画面
VOID MY_PLAY_PROC(VOID);	//プレイ画面の処理
VOID MY_PLAY_DRAW(VOID);	//プレイ画面の描画

VOID MY_END(VOID);			//エンド画面
VOID MY_END_PROC(VOID);		//エンド画面の処理
VOID MY_END_DRAW(VOID);		//エンド画面の描画

BOOL MY_LOAD_IMAGE(VOID);		//画像をまとめて読み込む関数
VOID MY_DELETE_IMAGE(VOID);		//画像をまとめて削除する関数

BOOL MY_LOAD_MODEL(VOID);		//3Dモデルをまとめて読み込む関数
VOID MY_DELETE_MODEL(VOID);		//3Dモデルをまとめて削除する関数

VOID MY_PROC_MAGURO(VOID);		//マグロの処理

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE);								//ウィンドウモードに設定
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//指定の数値でウィンドウを表示する
	SetWindowStyleMode(GAME_WINDOW_BAR);				//タイトルバーはデフォルトにする
	SetMainWindowText(TEXT(GAME_WINDOW_NAME));			//ウィンドウのタイトルの文字
	SetAlwaysRunFlag(TRUE);								//非アクティブでも実行する

	//ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1) { return -1; }

	SetMouseDispFlag(TRUE);					//マウスカーソルを表示	
	SetDrawScreen(DX_SCREEN_BACK);			//Draw系関数は裏画面に描画

	//画像を読み込む
	if (MY_LOAD_IMAGE() == FALSE) { return -1; }

	//3Dモデルを読み込む
	if (MY_LOAD_MODEL() == FALSE) { return -1; }

	//ゲームの初期化
	if (MY_GAME_INIT() == FALSE) { return -1; }

	//標準ライトの方向を設定する
	//SetLightDirection(VGet(0.0f, 1.0f, 0.0f));

	//無限ループ
	while (TRUE)
	{
		//メッセージ処理の結果がエラーのとき、強制終了
		if (ProcessMessage() != 0) { break; }

		//画面を消去できなかったとき、強制終了
		if (ClearDrawScreen() != 0) { break; }

		//おわるを選ぶとゲーム終了ダイアログを表示(仮置きでESCキー)
		if (AllKeyState[KEY_INPUT_ESCAPE] != 0)
		{
			int Ret = MessageBox(GetMainWindowHandle(), "終了しますか？", "確認", MB_YESNO);
		
			if (Ret == IDYES) { break; }
		
		}

		//押しているキー状態を取得
		MY_ALL_KEYDOWN_UPDATE();

		//FPSの処理(更新)
		MY_FPS_UPDATE();

		//シーンごとに処理を行う
		switch (GameScene)
		{

		case GAME_SCENE_START:
			MY_START();	//スタート画面
			break;

		case GAME_SCENE_HOWTO:
			MY_HOWTO();	//あそびかた画面
			break;

		case GAME_SCENE_PLAY:
			MY_PLAY();	//プレイ画面
			break;

		case GAME_SCENE_END:
			MY_END();	//エンド画面
			break;

		}

		MY_FPS_DRAW();	//FPSの処理(描画)

		ScreenFlip();	//モニタのリフレッシュレートの速さで画面を再描画

		MY_FPS_WAIT();	//FPSの処理(待つ)

	}

	// ＤＸライブラリの後始末
	DxLib_End();

	// ソフトの終了
	return 0;
}

//FPSを計測、更新する関数 ################################3
VOID MY_FPS_UPDATE(VOID)
{

	if (CountFps == 0) //1フレーム目なら時刻を記憶
	{
		StartTimeFps = GetNowCount();
	}

	if (CountFps == SampleNumFps)
	{
		int now = GetNowCount();
		//現在の時間から、0フレーム目の時間を引き、FPSの数値で割る＝1FPS辺りの平均時間が計算される
		CalcFps = 1000.f / ((now - StartTimeFps) / (float)SampleNumFps);
		CountFps = 0;
		StartTimeFps = now;
	}

	CountFps++;
	return;

}

//FPS値を描画する関数 ##############################
VOID MY_FPS_DRAW(VOID)
{
	//文字列を描画
	DrawFormatString(
		0,
		GAME_HEIGHT - 20,
		GetColor(255, 255, 255),
		"FPS : %.1f",
		CalcFps
	);
	return;
}

//FPS値を計測し、待つ関数 ##########################
VOID MY_FPS_WAIT(VOID)
{
	int resultTime = GetNowCount() - StartTimeFps;			//かかった時間
	int waitTime = CountFps * 1000 / GAME_FPS - resultTime;	//待つべき時間

	if (waitTime > 0)
	{
		WaitTimer(waitTime);	//待つ
	}

	return;

}

//########## キーの入力状態を更新する関数 ##########
VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{
	//参考：https://dxlib.xsrv.jp/function/dxfunc_input.html

	char TempKey[256];	//一時的に、現在のキーの入力状態を格納する

	//直前のキー入力をとっておく
	for (int i = 0; i < 256; i++)
	{
		OldAllKeyState[i] = AllKeyState[i];
	}

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

//キーを押しているか、キーコードで判断する
//引数：int : キーコード：KEY_INPUT_???
BOOL MY_KEY_DOWN(int KEY_INPUT_)
{
	//キーコードのキーを押しているとき
	if (AllKeyState[KEY_INPUT_] != 0)
	{
		return TRUE;	//キーを押している
	}
	else
	{
		return FALSE;	//キーを押していない
	}
}

//キーを押し上げたか、キーコードで判断する
//引数：int : キーコード：KEY_INPUT_???
BOOL MY_KEY_UP(int KEY_INPUT_)
{
	if (OldAllKeyState[KEY_INPUT_] >= 1		//直前は押していて
		&& AllKeyState[KEY_INPUT_] == 0)	//今は押していないとき
	{
		return TRUE;	//キーを押し上げている
	}
	else
	{
		return FALSE;	//キーを押し上げていない
	}
}

//キーを押し続けているか、キーコードで判断する
//引　数：int：キーコード：KEY_INPUT_???
//引　数：int：キーを押し続ける時間
BOOL MY_KEYDOWN_KEEP(int KEY_INPUT_, int DownTime)
{
	//押し続ける時間 = 秒数 * FPS値
	//例）60FPSのゲームで、1秒間押し続けられるなら、1秒 * 60FPS
	int UpdateTime = DownTime * GAME_FPS;

	if (AllKeyState[KEY_INPUT_] > UpdateTime)
	{
		return TRUE;	//押し続けている
	}
	else
	{
		return FALSE;	//押し続けていない
	}

}

//スタート画面
VOID MY_START(VOID)
{
	MY_START_PROC();	//スタート画面の処理
	MY_START_DRAW();	//スタート画面の描画

	return;

}

//スタート画面の処理
VOID MY_START_PROC(VOID)
{
	//エンターキーでプレイシーンへ
	if (MY_KEY_UP(KEY_INPUT_RETURN) == TRUE)
	{
		//プレイシーンへ移動する
		GameScene = GAME_SCENE_PLAY;

		return;
	}
	else if (MY_KEY_DOWN(KEY_INPUT_SPACE) == TRUE)
	{
		//あそびかたへ移動する
		GameScene = GAME_SCENE_HOWTO;

		return;
	}

	return;
}

//スタート画面の描画
VOID MY_START_DRAW(VOID)
{
	//背景描画
	DrawGraph(ImageTitleBK.x, ImageTitleBK.y, ImageTitleBK.handle, TRUE);

	//タイトルロゴ描画
	DrawGraph(ImageTitleLOGO.x, ImageTitleLOGO.y, ImageTitleLOGO.handle, TRUE);

	//コマンド類
	DrawGraph(ImageTitleStart.x, ImageTitleStart.y, ImageTitleStart.handle, TRUE);
	DrawGraph(ImageTitleEnd.x, ImageTitleEnd.y, ImageTitleEnd.handle, TRUE);
	DrawGraph(ImageTitleHowTo.x, ImageTitleHowTo.y, ImageTitleHowTo.handle, TRUE);

	//DrawString(0, 0, "スタート画面", GetColor(255, 255, 255));
	return;
}

//あそびかた画面
VOID MY_HOWTO(VOID)
{
	MY_HOWTO_PROC();	//スタート画面の処理
	MY_HOWTO_DRAW();	//スタート画面の描画

	return;

}

//あそびかた画面の処理
VOID MY_HOWTO_PROC(VOID)
{
	//エンターキーでタイトルへ
	if (MY_KEY_UP(KEY_INPUT_RETURN) == TRUE)
	{
		//タイトルへ移動する
		GameScene = GAME_SCENE_START;

		return;
	}

	return;
}

//あそびかた画面の描画
VOID MY_HOWTO_DRAW(VOID)
{
	//背景描画
	DrawGraph(ImageHowToBK.x, ImageHowToBK.y, ImageHowToBK.handle, TRUE);

	//DrawString(0, 0, "あそびかた画面", GetColor(255, 255, 255));
	return;
}

//プレイ画面
VOID MY_PLAY(VOID)
{
	//背景描画
	DrawGraph(ImagePlayBK.x, ImagePlayBK.y, ImagePlayBK.handle, TRUE);

	MY_PLAY_PROC();	//プレイ画面の処理
	MY_PLAY_DRAW();	//プレイ画面の描画
	//DrawString(0, 0, "プレイ画面", GetColor(255, 255, 255));

	return;
}

//プレイ画面の処理
VOID MY_PLAY_PROC(VOID)
{
	//サイズの更新
	MaguroScale = VGet(Maguro_Size, Maguro_Size, Maguro_Size);
	MV1SetScale(Maguro.handle, MaguroScale);

	//マップの外にいたら動けない
	/*if (Maguro.pos.x <= 0.0f) { Maguro.IsMove = FALSE; }
	if (Maguro.pos.x >= 15000.0f) { Maguro.IsMove = FALSE; }
	if (Maguro.pos.z <= 0.0f) { Maguro.IsMove = FALSE; }
	if (Maguro.pos.z>= 15000.0f) { Maguro.IsMove = FALSE; }*/

	//描画の判断
	/*for (int tate = 0; tate < MAP_TATE_MAX; tate++)
	{
		for (int yoko = 0; yoko < MAP_YOKO_MAX; yoko++)
		{*/
			////最初は全て描画する
			//for (int i = 0; i < EBI_MAX; i++)
			//{
			//	MV1SetScale(Ebi[i].handle, EbiScale);
			//	Ebi[i].pos = VGet(GetRand(15000), 0, GetRand(15000));
			//	MV1SetPosition(Ebi[i].handle, Ebi[i].pos);
			//	Ebi[i].IsDraw = TRUE;
			//}

			//for (int i = 0; i < PURA_MAX; i++)
			//{
			//	MV1SetScale(Pura[i].handle, PuraScale);
			//	Pura[i].pos = VGet(GetRand(15000), 0, GetRand(15000));
			//	MV1SetPosition(Pura[i].handle, Pura[i].pos);
			//	Pura[i].IsDraw = TRUE;
			//}

			//for (int i = 0; i < IKA_MAX; i++)
			//{
			//	MV1SetScale(Ika[i].handle, IkaScale);
			//	Ika[i].pos = VGet(GetRand(15000), 0, GetRand(15000));
			//	MV1SetPosition(Ika[i].handle, Ika[i].pos);
			//	Ika[i].IsDraw = TRUE;
			//}

			////マグロの前すぎるモデルは描画しない
			//if (MaguroToMapZ + 20 < tate)
			//{
			//	Ebi.IsDraw = FALSE;
			//	Pura.IsDraw = FALSE;
			//	Ika.IsDraw = FALSE;
			//}

			//マグロの後ろのモデルは描画しない
			/*if (MaguroToMapZ - 1 > tate)
			{
				for (int i = 0; i < EBI_MAX; i++)
				{
					Ebi[i].IsDraw = FALSE;
				}

				for (int i = 0; i < PURA_MAX; i++)
				{
					Pura[i].IsDraw = FALSE;
				}

				for (int i = 0; i < IKA_MAX; i++)
				{
					Ika[i].IsDraw = FALSE;
				}
			}
		}
	}*/

	//ひとまずはスペースキーでエンド画面へ
	if (MY_KEY_DOWN(KEY_INPUT_SPACE) == TRUE)
	{
		//エンドシーンへ移動する
		GameScene = GAME_SCENE_END;

	}

	//マグロの移動
	MY_PROC_MAGURO();

	//マグロの位置をマップと対応付ける
	/*MaguroToMapX = int(Maguro.pos.x / MAP_YOKO_SIZE);
	MaguroToMapZ = int(Maguro.pos.z / MAP_TATE_SIZE);*/
	/*MaguroToMapX = Maguro.pos.x;
	MaguroToMapZ = Maguro.pos.z;*/

	//マグロの線分を計算(位置の微調整)
	MaguroCollVecStart = VGet(Maguro.pos.x, Maguro.pos.y + 35.0f * (Maguro_Size + 1), Maguro.pos.z);

	//マグロの線分を計算(位置の微調整)
	MaguroCollVecEnd = VGet(Maguro.pos.x, Maguro.pos.y - 35.0f * (Maguro_Size + 1), Maguro.pos.z);

	//この時点で、まだ動ける
	Maguro.IsMove = TRUE;

	//当たり判定の処理
	MV1_COLL_RESULT_POLY_DIM Ebi_poly;	//衝突判断係？
	MV1_COLL_RESULT_POLY_DIM Pura_poly;
	MV1_COLL_RESULT_POLY_DIM Ika_poly;

	//エビ
	for (int i = 0; i < EBI_MAX; i++)
	{
		if (Ebi[i].IsDraw == TRUE)
		{
			/*if (Maguro.pos.z > Ebi[i].pos.z)
			{
				Ebi[i].IsDraw = FALSE;
			}*/

			Ebi_poly = MV1CollCheck_Capsule(Ebi[i].handle, -1, MaguroCollVecStart, MaguroCollVecEnd, MaguroCollRadius);

			//少しでもカプセルに触れたら動けない
			if (Ebi_poly.HitNum >= 1 && Maguro_Size < Ebi_Size)
			{
				Maguro.IsMove = FALSE;	//動けない
				//break;
			}
			else if (Ebi_poly.HitNum >= 1 && Maguro_Size > Ebi_Size)
			{
				Ebi[i].IsDraw = FALSE;
				Maguro_Size += 0.075f;
				MaguroCollRadius *= 1.075f;
				Cam_Dist *= 1.05f;
			}

		}
	}
			

	//プランクトン
	for (int i = 0; i < PURA_MAX; i++)
	{
		if (Pura[i].IsDraw == TRUE)
		{
			/*if (Maguro.pos.z > Pura[i].pos.z)
			{
				Pura[i].IsDraw = FALSE;
			}*/

			Pura_poly = MV1CollCheck_Capsule(Pura[i].handle, -1, MaguroCollVecStart, MaguroCollVecEnd, MaguroCollRadius);

			//少しでもカプセルに触れたら動けない
			if (Pura_poly.HitNum >= 1 && Maguro_Size < Pura_Size)
			{
				Maguro.IsMove = FALSE;	//動けない
				//break;
			}
			else if (Pura_poly.HitNum >= 1 && Maguro_Size > Pura_Size)
			{
				Pura[i].IsDraw = FALSE;
				Maguro_Size += 0.05f;
				MaguroCollRadius *= 1.05f;
				Cam_Dist *= 1.03f;
			}

		}
	}
			

	//イカ
	for (int i = 0; i < IKA_MAX; i++)
	{
		if (Ika[i].IsDraw == TRUE)
		{
			/*if (Maguro.pos.z > Ika[i].pos.z)
			{
				Ika[i].IsDraw = FALSE;
			}*/

			Ika_poly = MV1CollCheck_Capsule(Ika[i].handle, -1, MaguroCollVecStart, MaguroCollVecEnd, MaguroCollRadius);

			//少しでもカプセルに触れたら動けない
			if (Ika_poly.HitNum >= 1 && Maguro_Size < Ika_Size)
			{
				Maguro.IsMove = FALSE;	//動けない
				//break;
			}
			else if (Ika_poly.HitNum >= 1 && Maguro_Size > Ika_Size)
			{
				Ika[i].IsDraw = FALSE;
				Maguro_Size += 0.1f;
				MaguroCollRadius *= 1.1f;
				Cam_Dist *= 1.08f;
			}

		}
	}
	//if (Maguro.IsMove == FALSE) { break; }

	if (Maguro.IsMove == FALSE)
	{
		//エンドシーンへ移動する
		GameScene = GAME_SCENE_END;

		//Maguro_O2 -= 1.0;

	}
	/*else if (Maguro.IsMove == TRUE || Maguro_O2 <= 100.0)
	{
		Maguro_O2 += 1.0;
	}*/

	return;

}

//プレイ画面の描画
VOID MY_PLAY_DRAW(VOID)
{
	// カメラのクリッピング距離を設定
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	SetLightDirection(VGet(0.0f, -1.0f, 1.0f));		// 標準ライトの方向を設定する
	MV1DrawModel(Maguro.handle);						//マグロを描画

	//描画
	for (int i = 0; i < PURA_MAX; i++)	//プランクトン
	{
		if (Pura[i].IsDraw == TRUE)
		{
			MV1DrawModel(Pura[i].handle);
		}

	}

	for (int i = 0; i < EBI_MAX; i++)	//エビ
	{
		if (Ebi[i].IsDraw == TRUE)
		{
			MV1DrawModel(Ebi[i].handle);
		}

	}

	for (int i = 0; i < IKA_MAX; i++)	//イカ
	{
		if (Ika[i].IsDraw == TRUE)
		{
			MV1DrawModel(Ika[i].handle);
		}

	}

	//デバッグ用表示
	if (DEBUG_MODE == TRUE)
	{
		DrawFormatString(0, 20, GetColor(255, 255, 255), "HAngle:%.2lf / VAngle:%.2lf", camera.HAngle, camera.VAngle);
		DrawFormatString(0, 40, GetColor(255, 255, 255), "MaguroToMapX:%2d / MaguroToMapZ:%2d", MaguroToMapX, MaguroToMapZ);
		DrawFormatString(0, 60, GetColor(255, 255, 255), "MaguroX:%.2lf / MaguroZ:%.2lf", Maguro.pos.x, Maguro.pos.z);
		DrawFormatString(0, 80, GetColor(255, 255, 255), "Maguro_Size:%.2lf", Maguro_Size);

		//デバッグ用のカプセルの表示
		int Bunkatsu = 8;
		DrawCapsule3D(MaguroCollVecStart, MaguroCollVecEnd, MaguroCollRadius, Bunkatsu, GetColor(255, 255, 255), GetColor(255, 255, 255), TRUE);
	
		////デバッグ用の位置を設定
		//Ebi.pos = VGet(5000, 0, 7500);
		//MV1SetPosition(Ebi.handle, Ebi.pos);

		//Pura.pos = VGet(-2500, 0, -500);
		//MV1SetPosition(Pura.handle, Pura.pos);

		//Ika.pos = VGet(1000, 0, 2500);
		//MV1SetPosition(Ika.handle, Ika.pos);

		////デバッグ用モデルの描画
		//MV1DrawModel(Ebi.handle);
		//MV1DrawModel(Pura.handle);
		//MV1DrawModel(Ika.handle);
	
	}

	return;
}

//カメラの処理
VOID MY_PROC_MAGURO(VOID)
{
	//マグロの移動量を初期化
	Maguro.move = VGet(0.0f, 0.0f, 0.0f);

	camera.VAngle = 15;

	if (Maguro.IsMove == TRUE)
	{
			//マグロを自動スクロール
			Maguro.move = VGet(0.0f, 0.0f, MAGURO_MOVE);
	}

	//左へ回転
	if (MY_KEY_DOWN(KEY_INPUT_A) == TRUE)
	{
		//角度を変える
		camera.HAngle += CAMERA_ANGLE_X_PLUS;
	}

	//右へ回転
	if (MY_KEY_DOWN(KEY_INPUT_D) == TRUE)
	{
		//角度を変える
		camera.HAngle -= CAMERA_ANGLE_X_PLUS;
	}

	//カメラの向きに合わせて、マグロを移動させる
	double SinParam = sin(double(camera.HAngle / 180.0f) * DX_PI_F);
	double CosParam = cos(double(camera.HAngle / 180.0f) * DX_PI_F);
	VECTOR TempMoveVector;
	TempMoveVector.x = Maguro.move.x * CosParam - Maguro.move.z * SinParam;
	TempMoveVector.y = 0.0f;
	TempMoveVector.z = Maguro.move.x * SinParam + Maguro.move.z * CosParam;
	Maguro.pos = VAdd(Maguro.pos, TempMoveVector);

	// 新しい向きをセット
	MV1SetRotationXYZ(Maguro.handle,
		VGet(
			0.0f,
			-(camera.HAngle / 180.0f * DX_PI_F),
			0.0f
		)
	);

	//マグロの位置を設定
	MV1SetPosition(Maguro.handle, Maguro.pos);

	//カメラの設定
	VECTOR TempPosCalc1;
	VECTOR TempPosCalc2;
	VECTOR CameraLookAtPos;

	// 注視点はキャラクターモデルの座標から CAMERA_HEIGHT 分だけ高い位置
	CameraLookAtPos = Maguro.pos;
	CameraLookAtPos.y += CAMERA_HEIGHT;

	// 最初に垂直角度を反映した位置を算出
	Maguro.VecSin = sin(double(camera.VAngle / 150.0f) * DX_PI_F);
	Maguro.VecCos = cos(double(camera.VAngle / 180.0f) * DX_PI_F);
	TempPosCalc1.x = 0.0f;
	TempPosCalc1.y = Maguro.VecSin * Cam_Dist;
	TempPosCalc1.z = -Maguro.VecCos * Cam_Dist;

	// 次に水平角度を反映した位置を算出
	Maguro.VecSin = sin(double(camera.HAngle / 180.0f) * DX_PI_F);
	Maguro.VecCos = cos(double(camera.HAngle / 180.0f) * DX_PI_F);
	TempPosCalc2.x = Maguro.VecCos * TempPosCalc1.x - Maguro.VecSin * TempPosCalc1.z;
	TempPosCalc2.y = TempPosCalc1.y;
	TempPosCalc2.z = Maguro.VecSin * TempPosCalc1.x + Maguro.VecCos * TempPosCalc1.z;

	// 算出した座標に注視点の位置を加算したものがカメラの位置
	camera.pos = VAdd(TempPosCalc2, CameraLookAtPos);

	// カメラの設定に反映する
	SetCameraPositionAndTarget_UpVecY(camera.pos, CameraLookAtPos);

	return;
}

//エンド画面
VOID MY_END(VOID)
{
	MY_END_PROC();	//エンド画面の処理
	MY_END_DRAW();	//エンド画面の描画
	return;
}

//エンド画面の処理
VOID MY_END_PROC(VOID)
{
	//エンターキーを押したら、タイトルシーンへ移動する
	if (MY_KEY_UP(KEY_INPUT_RETURN) == TRUE)
	{
		//ゲーム初期化
		MY_GAME_INIT();

		//タイトルシーンへ移動する
		GameScene = GAME_SCENE_START;

		return;
	}

	return;
}

//エンド画面の描画
VOID MY_END_DRAW(VOID)
{
	//背景描画
	DrawGraph(ImagePlayBK.x, ImagePlayBK.y, ImagePlayBK.handle, TRUE);

	MY_PLAY_DRAW();	//プレイ画面の描画

	DrawGraph(ImageEndLOGO.x, ImageEndLOGO.y, ImageEndLOGO.handle, TRUE);
	DrawGraph(ImageEndContenue.x, ImageEndContenue.y, ImageEndContenue.handle, TRUE);
	DrawGraph(ImageEndEnd.x, ImageEndEnd.y, ImageEndEnd.handle, TRUE);

	//DrawString(0, 0, "エンド画面(エンターキーを押してください)", GetColor(255, 255, 255));

	return;
}

//ゲーム初期化画面
BOOL MY_GAME_INIT(VOID)
{
	Maguro_Size = 0.5;
	MV1SetScale(Maguro.handle, MaguroScale);
	Maguro.pos = VGet(GetRand((1000) + 1), 0, GetRand((1000) + 1));

	//最初は全て描画する
	for (int i = 0; i < EBI_MAX; i++)
	{
		MV1SetScale(Ebi[i].handle, EbiScale);
		Ebi[i].pos = VGet(GetRand(15000), 0, GetRand(15000));
		MV1SetPosition(Ebi[i].handle, Ebi[i].pos);
		Ebi[i].IsDraw = TRUE;
	}

	for (int i = 0; i < PURA_MAX; i++)
	{
		MV1SetScale(Pura[i].handle, PuraScale);
		Pura[i].pos = VGet(GetRand(15000), 0, GetRand(15000));
		MV1SetPosition(Pura[i].handle, Pura[i].pos);
		Pura[i].IsDraw = TRUE;
	}

	for (int i = 0; i < IKA_MAX; i++)
	{
		MV1SetScale(Ika[i].handle, IkaScale);
		Ika[i].pos = VGet(GetRand(15000), 0, GetRand(15000));
		MV1SetPosition(Ika[i].handle, Ika[i].pos);
		Ika[i].IsDraw = TRUE;
	}

	return TRUE;
}

//画像をまとめて読み込む関数
BOOL MY_LOAD_IMAGE(VOID)
{
	//タイトル背景
	wsprintf(ImageTitleBK.path, IMAGE_TITLE_BK_PATH);	//ファイルパスをコピー
	ImageTitleBK.handle = LoadGraph(ImageTitleBK.path);	//画像をメモリに読み込み、ハンドルを取得
	if (ImageTitleBK.handle == -1) { MessageBox(GetMainWindowHandle(), ImageTitleBK.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//画像サイズを取得
	GetGraphSize(ImageTitleBK.handle, &ImageTitleBK.width, &ImageTitleBK.height);
	ImageTitleBK.x = GAME_WIDTH / 2 - ImageTitleBK.width / 2;	//中央揃え
	ImageTitleBK.y = GAME_HEIGHT / 2 - ImageTitleBK.height / 2;	//中央揃え

	//タイトルロゴ
	wsprintf(ImageTitleLOGO.path, IMAGE_TITLE_LOGO_PATH);	//ファイルパスをコピー
	ImageTitleLOGO.handle = LoadGraph(ImageTitleLOGO.path);	//画像をメモリに読み込み、ハンドルを取得
	if (ImageTitleLOGO.handle == -1) { MessageBox(GetMainWindowHandle(), ImageTitleLOGO.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//画像サイズを取得
	GetGraphSize(ImageTitleLOGO.handle, &ImageTitleLOGO.width, &ImageTitleLOGO.height);
	ImageTitleLOGO.x = 151.0;	//X座標
	ImageTitleLOGO.y = 50.0;	//Y座標

	//はじめる
	wsprintf(ImageTitleStart.path, IMAGE_TITLE_START_PATH);	//ファイルパスをコピー
	ImageTitleStart.handle = LoadGraph(ImageTitleStart.path);	//画像をメモリに読み込み、ハンドルを取得
	if (ImageTitleStart.handle == -1) { MessageBox(GetMainWindowHandle(), ImageTitleStart.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//画像サイズを取得
	GetGraphSize(ImageTitleStart.handle, &ImageTitleStart.width, &ImageTitleStart.height);
	ImageTitleStart.x = 145.5;	//X座標
	ImageTitleStart.y = 445.0;	//Y座標

	//おわる
	wsprintf(ImageTitleEnd.path, IMAGE_TITLE_END_PATH);	//ファイルパスをコピー
	ImageTitleEnd.handle = LoadGraph(ImageTitleEnd.path);	//画像をメモリに読み込み、ハンドルを取得
	if (ImageTitleEnd.handle == -1) { MessageBox(GetMainWindowHandle(), ImageTitleEnd.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//画像サイズを取得
	GetGraphSize(ImageTitleEnd.handle, &ImageTitleEnd.width, &ImageTitleEnd.height);
	ImageTitleEnd.x = 422.0;	//X座標
	ImageTitleEnd.y = 445.0;	//Y座標

	//あそびかた
	wsprintf(ImageTitleHowTo.path, IMAGE_TITLE_HOWTO_PATH);	//ファイルパスをコピー
	ImageTitleHowTo.handle = LoadGraph(ImageTitleHowTo.path);	//画像をメモリに読み込み、ハンドルを取得
	if (ImageTitleHowTo.handle == -1) { MessageBox(GetMainWindowHandle(), ImageTitleHowTo.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//画像サイズを取得
	GetGraphSize(ImageTitleHowTo.handle, &ImageTitleHowTo.width, &ImageTitleHowTo.height);
	ImageTitleHowTo.x = 654.5;	//X座標
	ImageTitleHowTo.y = 445.0;	//Y座標

	//あそびかた背景
	wsprintf(ImageHowToBK.path, IMAGE_HOWTO_BK_PATH);	//ファイルパスをコピー
	ImageHowToBK.handle = LoadGraph(ImageHowToBK.path);	//画像をメモリに読み込み、ハンドルを取得
	if (ImageHowToBK.handle == -1) { MessageBox(GetMainWindowHandle(), ImageHowToBK.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//画像サイズを取得
	GetGraphSize(ImageHowToBK.handle, &ImageHowToBK.width, &ImageHowToBK.height);
	ImageHowToBK.x = GAME_WIDTH / 2 - ImageHowToBK.width / 2;	//中央揃え
	ImageHowToBK.y = GAME_HEIGHT / 2 - ImageHowToBK.height / 2;	//中央揃え

	//プレイ背景
	wsprintf(ImagePlayBK.path, IMAGE_PLAY_BK_PATH);	//ファイルパスをコピー
	ImagePlayBK.handle = LoadGraph(ImagePlayBK.path);	//画像をメモリに読み込み、ハンドルを取得
	if (ImagePlayBK.handle == -1) { MessageBox(GetMainWindowHandle(), ImagePlayBK.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//画像サイズを取得
	GetGraphSize(ImagePlayBK.handle, &ImagePlayBK.width, &ImagePlayBK.height);
	ImagePlayBK.x = GAME_WIDTH / 2 - ImagePlayBK.width / 2;	//中央揃え
	ImagePlayBK.y = GAME_HEIGHT / 2 - ImagePlayBK.height / 2;	//中央揃え

	//残念
	wsprintf(ImageEndLOGO.path, IMAGE_END_LOGO_PATH);	//ファイルパスをコピー
	ImageEndLOGO.handle = LoadGraph(ImageEndLOGO.path);	//画像をメモリに読み込み、ハンドルを取得
	if (ImageEndLOGO.handle == -1) { MessageBox(GetMainWindowHandle(), ImageEndLOGO.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//画像サイズを取得
	GetGraphSize(ImageEndLOGO.handle, &ImageEndLOGO.width, &ImageEndLOGO.height);
	ImageEndLOGO.x = 110.0;	//X座標
	ImageEndLOGO.y = 95.0;	//Y座標

	//つづける
	wsprintf(ImageEndContenue.path, IMAGE_END_CONTENUE_PATH);	//ファイルパスをコピー
	ImageEndContenue.handle = LoadGraph(ImageEndContenue.path);	//画像をメモリに読み込み、ハンドルを取得
	if (ImageEndContenue.handle == -1) { MessageBox(GetMainWindowHandle(), ImageEndContenue.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//画像サイズを取得
	GetGraphSize(ImageEndContenue.handle, &ImageEndContenue.width, &ImageEndContenue.height);
	ImageEndContenue.x = 221.0;	//X座標
	ImageEndContenue.y = 442.0;	//Y座標

	//おわる
	wsprintf(ImageEndEnd.path, IMAGE_END_END_PATH);	//ファイルパスをコピー
	ImageEndEnd.handle = LoadGraph(ImageEndEnd.path);	//画像をメモリに読み込み、ハンドルを取得
	if (ImageEndEnd.handle == -1) { MessageBox(GetMainWindowHandle(), ImageEndEnd.path, ERR_LOAD_TITLE_IMAGE, MB_OK); return FALSE; }

	//画像サイズを取得
	GetGraphSize(ImageEndEnd.handle, &ImageEndEnd.width, &ImageEndEnd.height);
	ImageEndEnd.x = 601.0;	//X座標
	ImageEndEnd.y = 442.0;	//Y座標

	return TRUE;
}

//画像をまとめて削除する関数
VOID MY_DELETE_IMAGE(VOID)
{
	DeleteGraph(ImageTitleBK.handle);
	DeleteGraph(ImageTitleLOGO.handle);
	DeleteGraph(ImageTitleStart.handle);
	DeleteGraph(ImageTitleEnd.handle);
	DeleteGraph(ImageTitleHowTo.handle);
	DeleteGraph(ImagePlayBK.handle);
	DeleteGraph(ImageEndLOGO.handle);
	DeleteGraph(ImageEndContenue.handle);
	DeleteGraph(ImageEndEnd.handle);

	return;
}

//3Dモデルをまとめて読み込む関数
BOOL MY_LOAD_MODEL(VOID)
{
	//マグロを読み込む
	strcpyDx(Maguro.path, MODEL_MAGURO_PATH);		//パスのコピー
	Maguro.handle = MV1LoadModel(Maguro.path);		//モデル読み込み
	//読み込みエラー
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

	//エビを読み込む
	for (int i = 0; i < EBI_MAX; i++)
	{
		strcpyDx(Ebi[i].path, MODEL_EBI_PATH);		//パスのコピー
		Ebi[i].handle = MV1LoadModel(Ebi[i].path);		//モデル読み込み
		//読み込みエラー
		if (Ebi[i].handle == -1)
		{
			MessageBox(
				GetMainWindowHandle(),
				Ebi[i].path,
				ERR_LOAD_TITLE_MODEL,
				MB_OK
			);

			return FALSE;
		}
	}
	

	//プランクトンを読み込む
	for (int i = 0; i < PURA_MAX; i++)
	{
		strcpyDx(Pura[i].path, MODEL_PURA_PATH);		//パスのコピー
		Pura[i].handle = MV1LoadModel(Pura[i].path);		//モデル読み込み
		//読み込みエラー
		if (Pura[i].handle == -1)
		{
			MessageBox(
				GetMainWindowHandle(),
				Pura[i].path,
				ERR_LOAD_TITLE_MODEL,
				MB_OK
			);

			return FALSE;
		}
	}
	

	//イカを読み込む
	for (int i = 0; i < IKA_MAX; i++)
	{
		strcpyDx(Ika[i].path, MODEL_IKA_PATH);		//パスのコピー
		Ika[i].handle = MV1LoadModel(Ika[i].path);		//モデル読み込み
		//読み込みエラー
		if (Ika[i].handle == -1)
		{
			MessageBox(
				GetMainWindowHandle(),
				Ika[i].path,
				ERR_LOAD_TITLE_MODEL,
				MB_OK
			);

			return FALSE;
		}
	}

	return TRUE;
}

//3Dモデルをまとめて削除する関数
VOID MY_DELETE_MODEL(VOID)
{
	MV1DeleteModel(Maguro.handle);
	for (int i = 0; i < EBI_MAX; i++)
	{
		MV1DeleteModel(Ebi[i].handle);
	}

	for (int i = 0; i < PURA_MAX; i++)
	{
		MV1DeleteModel(Pura[i].handle);
	}

	for (int i = 0; i < IKA_MAX; i++)
	{
		MV1DeleteModel(Ika[i].handle);
	}

	return;
}

