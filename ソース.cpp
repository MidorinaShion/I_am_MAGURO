#include "DxLib.h"
#include <math.h>

//########## マクロ定義 ##########

#define DEBUG_MODE			TRUE	//デバッグモード

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
#define IMAGE_TITLE_BK_PATH		TEXT(".\\画像\\タイトル背景.png")
#define IMAGE_TITLE_LOGO_PATH	TEXT(".\\画像\\タイトルロゴ.png")
#define IMAGE_TITLE_START_PATH	TEXT(".\\画像\\はじめる.png")
#define IMAGE_TITLE_END_PATH	TEXT(".\\画像\\おわる1.png")
#define IMAGE_TITLE_HOWTO_PATH	TEXT(".\\画像\\あそびかた.png")
#define IMAGE_HOWTO_BK_PATH		TEXT(".\\画像\\あそびかた背景.png")

#define IMAGE_PLAY_BK_PATH	TEXT(".\\画像\\プレイ背景.png")

//カメラの設定
#define CAMERA_NEAR			500.0f		//どこまで近くを写すか
#define CAMERA_FAR			60000.0f	//どこまで遠くを写すか
#define CAMERA_HEIGHT		0.0f		//カメラの注視点の高さ
#define CAMERA_DISTANCE		1000.0f		//カメラと注視点の距離
#define CAMERA_INIT_V_ANGLE	0.0f		//カメラの向きデフォルト(垂直) / -90が上 90が下
#define CAMERA_INIT_H_ANGLE	180.0f		//カメラの向きデフォルト(水平) / -90が左 90が右
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
MODEL Ebi;		//エビ
MODEL Ika;		//イカ
MODEL Pura;		//プランクトン

//画像
IMAGE ImageTitleBK;		//仮置き
IMAGE ImageTitleLOGO;
IMAGE ImageTitleStart;
IMAGE ImageTitleEnd;
IMAGE ImageTitleHowTo;
IMAGE ImageHowToBK;

IMAGE ImagePlayBK;

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
	SetLightDirection(VGet(0.0f, 1.0f, 0.0f));

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

	DrawString(0, 0, "スタート画面", GetColor(255, 255, 255));
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

	DrawString(0, 0, "あそびかた画面", GetColor(255, 255, 255));
	return;
}

//プレイ画面
VOID MY_PLAY(VOID)
{
	//背景描画
	DrawGraph(ImagePlayBK.x, ImagePlayBK.y, ImagePlayBK.handle, TRUE);

	MY_PLAY_PROC();	//プレイ画面の処理
	MY_PLAY_DRAW();	//プレイ画面の描画
	DrawString(0, 0, "プレイ画面", GetColor(255, 255, 255));

	return;
}

//プレイ画面の処理
VOID MY_PLAY_PROC(VOID)
{
	//マップの外にいたら動けない
	if (Maguro.pos.x <= -20000.0f) { Maguro.IsMove = FALSE; }
	if (Maguro.pos.x >= 20000.0f) { Maguro.IsMove = FALSE; }

	//描画の判断
	for (int tate = 0; tate < MAP_TATE_MAX; tate++)
	{
		for (int yoko = 0; yoko < MAP_YOKO_MAX; yoko++)
		{
			//最初は全て描画する
			/*ModelAste_ue[tate][yoko].IsDraw = TRUE;	//エサ類置換予定地
			ModelAste[tate][yoko].IsDraw = TRUE;
			ModelAste_sita[tate][yoko].IsDraw = TRUE;*/

			//マグロの前すぎるモデルは描画しない
			if (MaguroToMapZ + 20 < tate)
			{
				/*ModelAste_ue[tate][yoko].IsDraw = FALSE;
				ModelAste[tate][yoko].IsDraw = FALSE;
				ModelAste_sita[tate][yoko].IsDraw = FALSE;*/
			}

			//マグロの後ろのモデルは描画しない
			if (MaguroToMapZ - 1 > tate)
			{
				/*ModelAste_ue[tate][yoko].IsDraw = FALSE;
				ModelAste[tate][yoko].IsDraw = FALSE;
				ModelAste_sita[tate][yoko].IsDraw = FALSE;*/
			}
		}
	}

	//ひとまずはスペースキーでエンド画面へ
	if (MY_KEY_DOWN(KEY_INPUT_SPACE) == TRUE)
	{
		//エンドシーンへ移動する
		GameScene = GAME_SCENE_END;

	}

	//マグロの移動
	MY_PROC_MAGURO();

	//マグロの位置をマップと対応付ける
	MaguroToMapX = int(Maguro.pos.x / MAP_YOKO_SIZE);
	MaguroToMapZ = int(Maguro.pos.z / MAP_TATE_SIZE);

}

//プレイ画面の描画
VOID MY_PLAY_DRAW(VOID)
{
	// カメラのクリッピング距離を設定
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	SetLightDirection(VGet(-1.0f, -1.0f, 1.0f));		// 標準ライトの方向を設定する
	MV1DrawModel(Maguro.handle);						//マグロを描画

	//デバッグ用表示
	if (DEBUG_MODE == TRUE)
	{
		DrawFormatString(0, 20, GetColor(255, 255, 255), "HAngle:%.2lf / VAngle:%.2lf", camera.HAngle, camera.VAngle);
		DrawFormatString(0, 40, GetColor(255, 255, 255), "MaguroToMapX:%2d / MaguroToMapZ:%2d", MaguroToMapX, MaguroToMapZ);
		DrawFormatString(0, 60, GetColor(255, 255, 255), "MaguroX:%.2lf / MaguroZ:%.2lf", Maguro.pos.x, Maguro.pos.z);

		//デバッグ用のカプセルの表示
		//int Bunkatsu = 8;
		//DrawCapsule3D(MaguroCollVecStart, MaguroCollVecEnd, MaguroCollRadius, Bunkatsu, GetColor(255, 255, 255), GetColor(255, 255, 255), FALSE);
	
		//デバッグ用エビ
		MV1DrawModel(Ebi.handle);
	
	}

	return;
}

//カメラの処理
VOID MY_PROC_MAGURO(VOID)
{
	//マグロの移動量を初期化
	Maguro.move = VGet(0.0f, 0.0f, 0.0f);

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
	Maguro.VecSin = sin(double(camera.VAngle / 180.0f) * DX_PI_F);
	Maguro.VecCos = cos(double(camera.VAngle / 180.0f) * DX_PI_F);
	TempPosCalc1.x = 0.0f;
	TempPosCalc1.y = Maguro.VecSin * CAMERA_DISTANCE;
	TempPosCalc1.z = -Maguro.VecCos * CAMERA_DISTANCE;

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
	MY_PLAY_DRAW();	//プレイ画面の描画
	DrawString(0, 0, "エンド画面(エンターキーを押してください)", GetColor(255, 255, 255));

	return;
}

//ゲーム初期化画面
BOOL MY_GAME_INIT(VOID)
{

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
	strcpyDx(Ebi.path, MODEL_EBI_PATH);		//パスのコピー
	Ebi.handle = MV1LoadModel(Ebi.path);		//モデル読み込み
	//読み込みエラー
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

//3Dモデルをまとめて削除する関数
VOID MY_DELETE_MODEL(VOID)
{
	MV1DeleteModel(Maguro.handle);
	MV1DeleteModel(Ebi.handle);

	return;
}

