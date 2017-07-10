﻿#ifndef _STATION_SPY_PLUGIN_H_
#define _STATION_SPY_PLUGIN_H_

#include "MQBasePlugin.h"

extern const char* const plugin_name;
extern const wchar_t* const plugin_namew;
extern const char* const plugin_copyright;
extern const wchar_t* const plugin_copyrightw;
extern const wchar_t* const plugin_buildw;

extern const int plugin_id;
extern const int plugin_product;


class StationPlugin : public MQStationPlugin
{
	friend class CMainDlg;

protected:
	CMainDlg *dlgMain;
	bool fActive;	//window
public:
	StationPlugin();
	virtual ~StationPlugin();

	// プラグインIDを返す。
	void GetPlugInID(DWORD *Product, DWORD *ID) override;
	// プラグイン名を返す。
	const char *GetPlugInName(void) override;
	// ポップアップメニューに表示される文字列を返す。
	const char *EnumString(void) override;
	// サブコマンド前を列挙
	//const char *EnumSubCommand(int index) override;
	// サブコマンドの文字列を列挙
	//const wchar_t *GetSubCommandString(int index) override;

	// アプリケーションの初期化
	BOOL Initialize() override;
	// アプリケーションの終了
	void Exit() override;

	// 表示・非表示切り替え要求
	BOOL Activate(MQDocument doc, BOOL flag) override;
	// 表示・非表示状態の返答
	BOOL IsActivated(MQDocument doc) override;
	// ウインドウの最小化への返答
	void OnMinimize(MQDocument doc, BOOL flag) override;
	// プラグイン独自のメッセージの受け取り
	//int OnReceiveUserMessage(MQDocument doc, DWORD src_product, DWORD src_id, const char *description, void *message) override;
	// サブコマンドの呼び出し
	//BOOL OnSubCommand(MQDocument doc, int index) override;
	// 描画時の処理
	void OnDraw(MQDocument doc, MQScene scene, int width, int height) override;

	// ドキュメント初期化時
	void OnNewDocument(MQDocument doc, const char *filename, NEW_DOCUMENT_PARAM& param) override;
	// ドキュメント終了時
	void OnEndDocument(MQDocument doc) override;
	// ドキュメント保存時
	void OnSaveDocument(MQDocument doc, const char *filename, SAVE_DOCUMENT_PARAM& param) override;
	// アンドゥ実行時
	BOOL OnUndo(MQDocument doc, int undo_state) override;
	// リドゥ実行時
	BOOL OnRedo(MQDocument doc, int redo_state) override;
	// アンドゥ状態更新時
	//void OnUpdateUndo(MQDocument doc, int undo_state, int undo_size) override;
	// オブジェクトの編集時
	void OnObjectModified(MQDocument doc) override;
	// オブジェクトの選択状態の変更時
	void OnObjectSelected(MQDocument doc) override;
	// カレントオブジェクトの変更時
	void OnUpdateObjectList(MQDocument doc) override;
	// マテリアルのパラメータ変更時
	void OnMaterialModified(MQDocument doc) override;
	// カレントマテリアルの変更時
	void OnUpdateMaterialList(MQDocument doc) override;
	// シーン情報の変更時
	void OnUpdateScene(MQDocument doc, MQScene scene) override;


	typedef bool (StationPlugin::*ExecuteCallbackProc)(MQDocument doc);

	void Execute(ExecuteCallbackProc proc);

	// 面の削除
	bool DeleteFace(MQDocument doc);

protected:
	struct CallbackInfo {
		ExecuteCallbackProc proc;
	};

	// コールバックに対する実装部
	virtual bool ExecuteCallback(MQDocument doc, void *option);
};


#endif _STATION_SPY_PLUGIN_H_