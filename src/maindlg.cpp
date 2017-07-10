// maindlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "aboutdlg.h"
#include "maindlg.h"
#include "StationPlugin.h"
#include "MQSetting.h"
#include <string>
#include <sstream>
#include "ObjectComposition.h"


CMainDlg::CMainDlg(StationPlugin *plugin, MQWindowBase& parent) : MQWindow(parent)
{
	setlocale(LC_ALL, ""); 

	m_pPlugin = plugin;
	/*
	// Set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);
	*/

	SetTitle(plugin_namew);
	SetOutSpace(0.4);

	//page1
	m_Tab = CreateTab(this);
	m_Tab->AddChangedEvent(this, &CMainDlg::OnTabChange);
	m_Tab->SetVertLayout(MQWidgetBase::LAYOUT_FILL);
	m_Tab->SetTabTitle(0, L"main");

	MQFrame *mainFrame, *sideFrame;
	mainFrame = CreateVerticalFrame(m_Tab);
	mainFrame->SetHorzLayout(MQWidgetBase::LAYOUT_FILL);
	mainFrame->SetVertLayout(MQWidgetBase::LAYOUT_FILL);
	sideFrame = CreateVerticalFrame(mainFrame);
	sideFrame->SetHorzLayout(MQWidgetBase::LAYOUT_FILL);
	sideFrame->SetVertLayout(MQWidgetBase::LAYOUT_FILL);
	//sideFrame->SetHintSizeRateX(20);

	m_CheckEnable = CreateCheckBox(sideFrame, L"active");
	m_CheckEnable->AddChangedEvent(this, &CMainDlg::OnCheckEvent);
	m_CheckShowCurrent= CreateCheckBox(sideFrame, L"show current object layer");
	m_CheckShowCurrent->AddChangedEvent(this, &CMainDlg::OnCheckEvent);
	m_CheckShowOther = CreateCheckBox(sideFrame, L"show other object layer");
	m_CheckShowOther->AddChangedEvent(this, &CMainDlg::OnCheckEvent);
	m_CheckWireframe = CreateCheckBox(sideFrame, L"show wire frame");
	m_CheckWireframe->AddChangedEvent(this, &CMainDlg::OnCheckEvent);
	m_CheckRealtime = CreateCheckBox(sideFrame, L"show realtime");
	m_CheckRealtime->AddChangedEvent(this, &CMainDlg::OnCheckEvent);
	m_CheckFreezSubLayer = CreateCheckBox(sideFrame, L"freeze sub-layers");
	m_CheckFreezSubLayer->AddChangedEvent(this, &CMainDlg::OnCheckEvent);

	m_CheckAlphaSync = CreateCheckBox(sideFrame, L"alpha sync");
	m_CheckAlphaSync->AddChangedEvent(this, &CMainDlg::OnCheckEvent);
	m_CheckAlphaSync->SetVertLayout(MQWidgetBase::LAYOUT_HINTSIZE);
	m_CheckAlphaSync->SetHintSizeRateY(2.0);
	m_SliderAlpha = CreateSlider(sideFrame);
	m_SliderAlpha->AddChangingEvent(this, &CMainDlg::OnSliderEvent);
	m_SliderAlpha->SetMin(0.0);
	m_SliderAlpha->SetMax(1.0);

	sideFrame = CreateHorizontalFrame(mainFrame);
	sideFrame->SetHorzLayout(MQWidgetBase::LAYOUT_FILL);
	sideFrame->SetVertLayout(MQWidgetBase::LAYOUT_HINTSIZE);
	sideFrame->SetHintSizeRateY(1.5); // 1.0よりも縦幅が伸びる

	MQButton *btn;
	btn = CreateButton(sideFrame, L"refresh");
	btn->AddClickEvent(this, &CMainDlg::OnButtonRefresh);
	btn = CreateButton(sideFrame, L"freeze current");
	btn->AddClickEvent(this, &CMainDlg::OnButtonCurrentFreez);
	btn = CreateButton(sideFrame, L"freeze all");
	btn->AddClickEvent(this, &CMainDlg::OnButtonAllFreez);

	//page2
	mainFrame = CreateVerticalFrame(m_Tab);
	mainFrame->SetVertLayout(MQWidgetBase::LAYOUT_FILL);
	m_Tab->SetTabTitle(1, L"log");

	m_MessageList = CreateListBox(mainFrame);
	m_MessageList->SetHorzLayout(MQWidgetBase::LAYOUT_FILL);

	//page3
	mainFrame = CreateVerticalFrame(m_Tab);
	mainFrame->SetVertLayout(MQWidgetBase::LAYOUT_FILL);
	m_Tab->SetTabTitle(2, L"about");

	MQLabel *label = CreateLabel(mainFrame, plugin_copyrightw);
	label->SetHorzLayout(MQWidgetBase::LAYOUT_HINTSIZE);
	label->SetHintSizeRateX(12);	// 1.0 = 1word でもない。謎の数値。
	label->SetAlignment(MQLabel::ALIGN_CENTER);
	label->SetWordWrap(true);

	label = CreateLabel(mainFrame, plugin_buildw);
	label->SetHorzLayout(MQWidgetBase::LAYOUT_HINTSIZE);
	label->SetHintSizeRateX(12);	// 1.0 = 1word でもない。謎の数値。
	label->SetAlignment(MQLabel::ALIGN_CENTER);
	label->SetWordWrap(true);
	
	this->AddHideEvent(this, &CMainDlg::OnHide);

	//*
	OC = new ObjectComposition(m_MessageList);
}

CMainDlg::~CMainDlg() {
	delete OC;
}

void CMainDlg::UpdateGUI() {
	if (fEventLock) return;

	OC->pEnable(m_CheckEnable->GetChecked());
	OC->pShowCurrent(m_CheckShowCurrent->GetChecked());
	OC->pShowOther(m_CheckShowOther->GetChecked());
	OC->pWireframe(m_CheckWireframe->GetChecked());
	OC->pRealtime(m_CheckRealtime->GetChecked());
	OC->pFreezSubLayer(m_CheckFreezSubLayer->GetChecked());

	OC->pAlphaSync(m_CheckAlphaSync->GetChecked());
	OC->pAlpha(m_SliderAlpha->GetPosition());
}

void CMainDlg::RestoryLayout(MQBasePlugin *plg) {

	fEventLock = true;	//lock checkbox onchange evrnt
	MQSetting* conf = plg->OpenSetting();
	if (conf == nullptr) return;

	bool b;
	float f;
	int a;

	conf->Load("ObjectComposition.posx", a, -1);
	if (a != -1) this->SetPosX(a);
	conf->Load("ObjectComposition.posy", a, -1);
	if (a != -1) this->SetPosY(a);

	conf->Load("ObjectComposition.enable", b, false);
	m_CheckEnable->SetChecked(b);
	conf->Load("ObjectComposition.showcurrent", b, true);
	m_CheckShowCurrent->SetChecked(b);
	conf->Load("ObjectComposition.showother", b, true);
	m_CheckShowOther->SetChecked(b);
	conf->Load("ObjectComposition.wireframe", b, false);
	m_CheckWireframe->SetChecked(b);
	conf->Load("ObjectComposition.realtime", b, false);
	m_CheckRealtime->SetChecked(b);
	conf->Load("ObjectComposition.freezsublayer", b, false);
	m_CheckFreezSubLayer->SetChecked(b);

	conf->Load("ObjectComposition.alphasync", b, false);
	m_CheckAlphaSync->SetChecked(b);
	conf->Load("ObjectComposition.alpha", f, 0.0);
	m_SliderAlpha->SetPosition(f);

	plg->CloseSetting(conf);
	fEventLock = false;

	AddMessage(L"load layout settings");
	
	UpdateGUI();
}

void CMainDlg::MemoryLayout(MQBasePlugin *plg) {
	
	MQSetting* conf = plg->OpenSetting();

	conf->Save("ObjectComposition.posx", this->GetPosX());
	conf->Save("ObjectComposition.posy", this->GetPosY());

	conf->Save("ObjectComposition.enable", m_CheckEnable->GetChecked());
	conf->Save("ObjectComposition.showcurrent", m_CheckShowCurrent->GetChecked());
	conf->Save("ObjectComposition.showother", m_CheckShowOther->GetChecked());
	conf->Save("ObjectComposition.wireframe", m_CheckWireframe->GetChecked());
	conf->Save("ObjectComposition.realtime", m_CheckRealtime->GetChecked());
	conf->Save("ObjectComposition.freezsublayer", m_CheckFreezSubLayer->GetChecked());

	conf->Save("ObjectComposition.alphasync", m_CheckAlphaSync->GetChecked());
	conf->Save("ObjectComposition.alpha", m_SliderAlpha->GetPosition());

	plg->CloseSetting(conf);

	AddMessage(L"save layout settings");
}

// Add a message log
void CMainDlg::AddMessage(const wchar_t *message)
{
	int index = m_MessageList->AddItem(message);
	m_MessageList->MakeItemVisible(index);

	if (index > 2048) {
		m_MessageList->ClearItems();
	}
}

BOOL CMainDlg::OnHide(MQWidgetBase *sender, MQDocument doc)
{
	if(m_pPlugin->fActive){
		m_pPlugin->fActive = false;
		m_pPlugin->WindowClose();
	}
	return FALSE;
}

BOOL CMainDlg::OnTabChange(MQWidgetBase *sender, MQDocument doc)
{
	return FALSE;
}

BOOL CMainDlg::OnAppAbout(MQWidgetBase *sender, MQDocument doc)
{
	CAboutDlg dlg(*this);
	dlg.Execute();
	return FALSE;
}

BOOL CMainDlg::OnButtonRefresh(MQWidgetBase *sender, MQDocument doc)
{
	doc->Compact();
	OC->BuildData();
	MQ_RefreshView(nullptr);

	return FALSE;
}

BOOL CMainDlg::OnButtonCurrentFreez(MQWidgetBase *sender, MQDocument doc)
{
	bool b;

	int n = doc->GetCurrentObjectIndex();
	MQObject o = doc->GetObject(n);

	if (o == nullptr) {
		MQDialog::MessageInformationBox(MQWindow::GetMainWindow(),L"please select object layer.",L"freeze current");
		return false;
	}

	if (OC->pFreezSubLayer()) {
		//freez sublayer
		if (MQDialog::MessageOkCancelBox(MQWindow::GetMainWindow(),L"freeze current & sub-layers?",L"freeze current") != MQDialog::DIALOG_OK){
			return false;
		}
		b = OC->Freeze(doc, o, true);
	}
	else {
		//freez current layer only
		if (MQDialog::MessageOkCancelBox(MQWindow::GetMainWindow(), L"freeze current object layer?", L"freeze current") != MQDialog::DIALOG_OK) {
			return false;
		}
		b = OC->Freeze(doc, o, false);
	}

	if (!b) {
		MQDialog::MessageInformationBox(MQWindow::GetMainWindow(), L"not exists composition layer. process cancel.", L"freeze current");
		return false;
	}

	MQ_RefreshView(nullptr);
	// true ... add undo & refresh
	return true;
}

BOOL CMainDlg::OnButtonAllFreez(MQWidgetBase *sender, MQDocument doc)
{
	if (MQDialog::MessageOkCancelBox(MQWindow::GetMainWindow(), L"freeze all layers?", L"freeze all") != MQDialog::DIALOG_OK) {
		return false;
	}

	bool b = OC->FreezeAll(doc);
	if (!b) {
		MQDialog::MessageInformationBox(MQWindow::GetMainWindow(), L"not exists composition layer. process cancel.", L"freeze all");
		return false;
	}

	return true;
}

BOOL CMainDlg::OnCheckEvent(MQWidgetBase *sender, MQDocument doc)
{
	UpdateGUI();		// get gui state
	OC->BuildData();	// rebuild composition & draw object
	OC->Draw(m_pPlugin, doc);
	MQ_RefreshView(nullptr);	// refresh Metasequoia App

	return false;
}

BOOL CMainDlg::OnSliderEvent(MQWidgetBase *sender, MQDocument doc) {
	UpdateGUI();		// get gui state
	if (!OC->pAlphaSync()) return false;

	OC->MaterialAlphaSync(doc, OC->pAlpha());	// <mat> material alpha change
	MQ_RefreshView(nullptr);	// refresh Metasequoia App

	return false;
	//return true; // 大量の変更情報がundoに登録されてしまうので止めとく？
}

/*
void CMainDlg::UpdateVertexList(MQDocument doc, bool select_only)
{
	if(!select_only){
		m_VertexList->ClearItems();
	}

	int curobj = doc->GetCurrentObjectIndex();
	if(curobj == -1) return;

	MQObject obj = doc->GetObject(curobj);
	if(obj == NULL) return;

	if(!select_only){
		int num = obj->GetVertexCount();
		for(int i=0; i<num; i++){
			if(obj->GetVertexRefCount(i) == 0) continue;

			MQPoint p = obj->GetVertex(i);
			UINT uid = obj->GetVertexUniqueID(i);
			wchar_t buf[256];
			swprintf_s(buf, L"%d [id:%u] (%f %f %f)", i, uid, p.x, p.y, p.z);
			m_VertexList->AddItem(buf, i);

			if(doc->IsSelectVertex(curobj, i)){
				m_VertexList->SetItemSelected(i, true);
			}
		}
	}else{
		int numVertex = obj->GetVertexCount();
		int num = m_VertexList->GetItemCount();
		for(int i=0; i<num; i++){
			int vert = (int)m_VertexList->GetItemTag(i);
			if(vert >= 0 && vert < numVertex){
				m_VertexList->SetItemSelected(i, doc->IsSelectVertex(curobj, vert) != FALSE);
			}
		}
	}
}
*/

/*
void CMainDlg::UpdateFaceList(MQDocument doc, bool select_only)
{
	if(!select_only){
		m_FaceList->ClearItems();
	}

	int curobj = doc->GetCurrentObjectIndex();
	if(curobj == -1) return;

	MQObject obj = doc->GetObject(curobj);
	if(obj == NULL) return;

	if(!select_only){
		int num = obj->GetFaceCount();
		for(int i=0; i<num; i++){
			int ptnum = obj->GetFacePointCount(i);
			if(ptnum == 0) continue;

			std::vector<int> ptarray(ptnum);
			obj->GetFacePointArray(i, &(*ptarray.begin()));

			UINT uid = obj->GetFaceUniqueID(i);

			std::wostringstream st;
			st << i << L" [id:" << uid << L"] (";
			for(int j=0; j<ptnum; j++){
				if(j > 0) st << L" ";
				st << ptarray[j];
			}
			st << L")";
			m_FaceList->AddItem(st.str(), i);

			if(doc->IsSelectFace(curobj, i)){
				m_FaceList->SetItemSelected(i, true);
			}
		}
	}else{
		int numFace = obj->GetFaceCount();
		int num = m_FaceList->GetItemCount();
		for(int i=0; i<num; i++){
			int face = (int)m_FaceList->GetItemTag(i);
			if(face >= 0 && face < numFace){
				m_FaceList->SetItemSelected(i, doc->IsSelectFace(curobj, face) != FALSE);
			}
		}
	}
}
*/




