

#include "stdafx.h"
#include "maindlg.h"
#include "StationPlugin.h"
#include <string>
#include <sstream>

const char* const plugin_name = "ObjectComposition ver2.1 MQwidget";
const wchar_t* const plugin_namew = L"ObjectComposition ver2.1 MQwidget";
const char* const plugin_copyright = "ObjectComposition ver2.1 MQwidget / Copyright(C) 2006-2016, Ko-Ta.";
const wchar_t* const plugin_copyrightw = L"ObjectComposition ver2.1 MQwidget / Copyright(C) 2006-2016, Ko-Ta.";
const wchar_t* const plugin_buildw = L"build Visual Studio C++ 2015.";

const int plugin_id = 0x23486A;
const int plugin_product = 0x8435AF;


static StationPlugin plugin;

// Constructor
// コンストラクタ
StationPlugin::StationPlugin()
{
	dlgMain = nullptr;
	fActive = false;
}

// Destructor
// デストラクタ
StationPlugin::~StationPlugin()
{
	if (dlgMain != nullptr){
		// Also deletes the window here just to make sure though it has been 
		// deleted in Exit().
		// Exit()で破棄されるはずだが一応念のためここでも
		delete dlgMain;
		dlgMain = nullptr;
	}
}

//---------------------------------------------------------------------------
//  GetPlugInID
//    プラグインIDを返す。
//    この関数は起動時に呼び出される。
//---------------------------------------------------------------------------
void StationPlugin::GetPlugInID(DWORD *Product, DWORD *ID)
{
	// プロダクト名(制作者名)とIDを、全部で64bitの値として返す
	// 値は他と重複しないようなランダムなもので良い
	*Product = plugin_product;
	*ID      = plugin_id;
}

//---------------------------------------------------------------------------
//  GetPlugInName
//    プラグイン名を返す。
//    この関数は[プラグインについて]表示時に呼び出される。
//---------------------------------------------------------------------------
const char *StationPlugin::GetPlugInName(void)
{
	return plugin_copyright;
}

//---------------------------------------------------------------------------
//  EnumString
//    ポップアップメニューに表示される文字列を返す。
//    この関数は起動時に呼び出される。
//---------------------------------------------------------------------------
const char *StationPlugin::EnumString(void)
{
	return plugin_name;
}


//---------------------------------------------------------------------------
//  EnumSubCommand
//    サブコマンド前を列挙
//---------------------------------------------------------------------------
/*
const char *StationPlugin::EnumSubCommand(int index)
{
	switch(index){
	case 0: return "command";
	case 1: return "log";
	}
	return NULL;
}
*/

//---------------------------------------------------------------------------
//  GetSubCommandString
//    サブコマンドの文字列を列挙
//---------------------------------------------------------------------------
/*
const wchar_t *StationPlugin::GetSubCommandString(int index)
{
	switch(index){
	case 0: return L"command";
	case 1: return L"log";
	}
	return NULL;
}
*/

//---------------------------------------------------------------------------
//  Initialize
//    アプリケーションの初期化
//---------------------------------------------------------------------------
BOOL StationPlugin::Initialize()
{
	if (dlgMain == nullptr){
		// Create a new window that's parent is the main window.
		// メインウインドウを親にした新しいウィンドウの作成
		MQWindowBase mainwnd = MQWindow::GetMainWindow();
		dlgMain = new CMainDlg(this, mainwnd);

		dlgMain->AddMessage(L"Initialize");
	}

	//load layour & param
	if (dlgMain != nullptr) dlgMain->RestoryLayout(this);

	return TRUE;
}

//---------------------------------------------------------------------------
//  Exit
//    アプリケーションの終了
//---------------------------------------------------------------------------
void StationPlugin::Exit()
{
	if (dlgMain != nullptr){
		dlgMain->AddMessage(L"Exit");
		delete dlgMain;
		dlgMain = nullptr;
	}
}

//---------------------------------------------------------------------------
//  Activate
//    表示・非表示切り替え要求
//---------------------------------------------------------------------------
BOOL StationPlugin::Activate(MQDocument doc, BOOL flag)
{
	if (dlgMain == nullptr)
		return FALSE;

	// Switch showing and hiding the window.
	// ウインドウの表示・非表示切り替え
	fActive = flag ? true : false;
	dlgMain->SetVisible(fActive);

	if (fActive){

	}
	else {

	}

	std::wstring s;
	flag ? s = L"Active:true" : s = L"Active:false";
	dlgMain->AddMessage(s.c_str());

	return fActive;
}

//---------------------------------------------------------------------------
//  IsActivated
//    表示・非表示状態の返答
//---------------------------------------------------------------------------
BOOL StationPlugin::IsActivated(MQDocument doc)
{
	// Return TRUE if a window has been not created yet.
	// ウインドウがまだ生成されていないならTRUE
	if (dlgMain == nullptr)
		return FALSE;

	return fActive;
}

//---------------------------------------------------------------------------
//  OnMinimize
//    ウインドウの最小化への返答
//---------------------------------------------------------------------------
void StationPlugin::OnMinimize(MQDocument doc, BOOL flag)
{
	std::wstring s;
	flag ? s = L"minimize:true" : s = L"minimize:false";
	dlgMain->AddMessage(s.c_str());
}

//---------------------------------------------------------------------------
//  OnReceiveUserMessage
//    プラグイン独自のメッセージの受け取り
//---------------------------------------------------------------------------
/*
int StationPlugin::OnReceiveUserMessage(MQDocument doc, DWORD src_product, DWORD src_id, const char *description, void *message)
{
	size_t len = strlen(description);
	wchar_t *buf = new wchar_t[len + 200];
	swprintf_s(buf, len + 200, L"OnReceiveUserMessage : src_plugin=[%08x %08x] description=%S", src_product, src_id, description);
	dlgMain->AddMessage(buf);

	// Operate a special action only when the message was sent from SingleMove plug-in.
	// SingleMoveプラグインからの通知のみ、特別な動作を行う
	if (src_product == 0x56A31D20 && src_id == 0x3901EA9B){
		if (strcmp(description, "text") == 0){
			swprintf_s(buf, len + 200, L"  from SingleMove : %S", (const char*)message);
			dlgMain->AddMessage(buf);
		}
	}

	delete[] buf;

	// Return 59 without a deep idea.
	// 適当に59を返している
	return 59;
}
*/

//---------------------------------------------------------------------------
//  OnSubCommand
//    A message for calling a sub comand
//    サブコマンドの呼び出し
//---------------------------------------------------------------------------
/*
BOOL StationPlugin::OnSubCommand(MQDocument doc, int index)
{
	switch(index){
	case 0:
		dlgMain->m_Tab->SetCurrentPage(0);
		return FALSE;
	case 1:
		dlgMain->m_Tab->SetCurrentPage(1);
		return FALSE;
	}
	return FALSE;
}
*/

//---------------------------------------------------------------------------
//  OnDraw
//    描画時の処理
//---------------------------------------------------------------------------
void StationPlugin::OnDraw(MQDocument doc, MQScene scene, int width, int height)
{
	//if (!m_bOnDraw) return;

	//std::wstring s;
	//s = L"OnDarw:" + std::to_wstring(width) + L"x" + std::to_wstring(height);
	//dlgMain->AddMessage(s.c_str());

	if (dlgMain->OC->pEnable()) {
		dlgMain->OC->Draw(this, doc);
	}
}


//---------------------------------------------------------------------------
//  OnNewDocument
//    ドキュメント初期化時
//---------------------------------------------------------------------------
void StationPlugin::OnNewDocument(MQDocument doc, const char *filename, NEW_DOCUMENT_PARAM& param)
{
	std::wostringstream ss;

	filename != nullptr ? ss << L"OnNewDocument:" << filename : ss << L"OnNewDocument:none";
	dlgMain->AddMessage(ss.str().c_str());

	dlgMain->OC->BuildData();	//*
}

//---------------------------------------------------------------------------
//  OnEndDocument
//    ドキュメント終了時
//---------------------------------------------------------------------------
void StationPlugin::OnEndDocument(MQDocument doc)
{
	dlgMain->AddMessage(L"OnEndDocument");

	dlgMain->OC->Init(this, doc);	//* Free Draw Object

	//save layour & param
	if (dlgMain != nullptr) dlgMain->MemoryLayout(this);
}

//---------------------------------------------------------------------------
//  OnSaveDocument
//    ドキュメント保存時
//---------------------------------------------------------------------------
void StationPlugin::OnSaveDocument(MQDocument doc, const char *filename, SAVE_DOCUMENT_PARAM& param)
{
	std::wostringstream ss;
	
	filename != nullptr ? ss << L"OnSaveDocument:" << filename : ss << L"OnSaveDocument:none";
	dlgMain->AddMessage(ss.str().c_str());
}

//---------------------------------------------------------------------------
//  OnUndo
//    アンドゥ実行時
//---------------------------------------------------------------------------
BOOL StationPlugin::OnUndo(MQDocument doc, int undo_state)
{
	//std::wstring s;
	//s = L"OnUndo:" + std::to_wstring(undo_state);
	//dlgMain->AddMessage(s.c_str());

	dlgMain->OC->BuildData();	//*
	
	return FALSE;
}

//---------------------------------------------------------------------------
//  OnRedo
//    リドゥ実行時
//---------------------------------------------------------------------------
BOOL StationPlugin::OnRedo(MQDocument doc, int redo_state)
{
	//std::wstring s;
	//s = L"OnRedo:" + std::to_wstring(redo_state);
	//dlgMain->AddMessage(s.c_str());

	dlgMain->OC->BuildData();	//*

	return FALSE;
}

//---------------------------------------------------------------------------
//  OnUpdateUndo
//    アンドゥ状態更新時
//---------------------------------------------------------------------------
/*
void StationPlugin::OnUpdateUndo(MQDocument doc, int undo_state, int undo_size)
{
	if (!m_bOnUpdateUndo) return;

	wchar_t buf[256];
	swprintf_s(buf, L"OnUpdateUndo : undo_state=%d undo_size=%d", undo_state, undo_size);
	dlgMain->AddMessage(buf);
}
*/

//---------------------------------------------------------------------------
//  OnObjectModified
//    オブジェクトの編集時
//---------------------------------------------------------------------------
void StationPlugin::OnObjectModified(MQDocument doc)
{
	dlgMain->AddMessage(L"OnObjectModified");

	// GUI情報の反映。たぶんいらないと思うけど
	dlgMain->UpdateGUI();

	// DeleteObjectの場合は呼ばれません

	// Object構造の変化があったか？
	if (dlgMain->OC->CheckObjectNameChange(doc)) {
		dlgMain->OC->BuildData();	//flagのみ
	}

	// コンポジット元オブジェクトを編集したか？
	// ※GetCurrentは編集中のオブジェクトを返すとは限りません。
	//   選択外のobjectを編集できる仕組みがあるため、その場合はgetcurrentと編集中の物とで異なります。
	//   取得には全オブジェクトの全face/vertex/lineを調べる必要があるので...現実的ではないでしょう。
	int n;
	n = doc->GetCurrentObjectIndex();
	if (dlgMain->OC->IsSrcCompObject(n)) {
		dlgMain->OC->BuildData();	//flagのみ
	}

	// currentではないobjectのvisibleを変更することができるため、currentのみでの判断が不可能。
	// 結局、変更があれば描画内容は強制的に更新をかけないと、表示ずれが起こります。
	dlgMain->OC->BuildDraw();		// flagのみ
	dlgMain->OC->Draw(this, doc);	// DrawObject,CompositionObjectの再構成処理
	MQ_RefreshView(nullptr);
}

//---------------------------------------------------------------------------
//  OnObjectSelected
//    オブジェクトの選択状態の変更時
//---------------------------------------------------------------------------
void StationPlugin::OnObjectSelected(MQDocument doc)
{
	//dlgMain->AddMessage(L"OnObjectSelected");
}

//---------------------------------------------------------------------------
//  OnUpdateObjectList
//    カレントオブジェクトの変更時
//---------------------------------------------------------------------------
void StationPlugin::OnUpdateObjectList(MQDocument doc)
{
	dlgMain->AddMessage(L"OnUpdateObjectList");

	dlgMain->OC->BuildData();	//*
}

//---------------------------------------------------------------------------
//  OnMaterialModified
//    マテリアルのパラメータ変更時
//---------------------------------------------------------------------------
void StationPlugin::OnMaterialModified(MQDocument doc)
{
	dlgMain->AddMessage(L"OnMaterialModified");

	dlgMain->OC->BuildData();	//*
}

//---------------------------------------------------------------------------
//  OnUpdateMaterialList
//    カレントマテリアルの変更時
//---------------------------------------------------------------------------
void StationPlugin::OnUpdateMaterialList(MQDocument doc)
{
	dlgMain->AddMessage(L"OnUpdateMaterialList");

	dlgMain->OC->BuildData();	//*
}

//---------------------------------------------------------------------------
//  OnUpdateScene
//    シーン情報の変更時
//---------------------------------------------------------------------------
void StationPlugin::OnUpdateScene(MQDocument doc, MQScene scene)
{
	//dlgMain->AddMessage(L"OnUpdateScene");
}

//---------------------------------------------------------------------------
//  ExecuteCallback
//    コールバックに対する実装部
//---------------------------------------------------------------------------
bool StationPlugin::ExecuteCallback(MQDocument doc, void *option)
{
	CallbackInfo *info = (CallbackInfo*)option;
	return ((*this).*info->proc)(doc);
}

// コールバックの呼び出し
void StationPlugin::Execute(ExecuteCallbackProc proc)
{
	CallbackInfo info;
	info.proc = proc;
	BeginCallback(&info);
}

// プラグインのベースクラスを返す
MQBasePlugin *GetPluginClass()
{
	return &plugin;
}

