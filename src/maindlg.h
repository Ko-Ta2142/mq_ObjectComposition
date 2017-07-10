// maindlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINDLG_H__35069CDB_1248_4479_8AE4_CC99D68849FF__INCLUDED_)
#define AFX_MAINDLG_H__35069CDB_1248_4479_8AE4_CC99D68849FF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MQWidget.h"
#include "ObjectComposition.h"

class StationPlugin;

class CMainDlg : public MQWindow
{
private:
	bool fEventLock = false;
public:
	ObjectComposition* OC;

	MQTab *m_Tab;
	MQListBox *m_MessageList;
	MQCheckBox *m_CheckEnable;
	MQCheckBox *m_CheckShowCurrent;
	MQCheckBox *m_CheckShowOther;
	MQCheckBox *m_CheckWireframe;
	MQCheckBox *m_CheckRealtime;
	MQCheckBox *m_CheckFreezSubLayer;
	MQCheckBox *m_CheckAlphaSync;
	MQSlider *m_SliderAlpha;

	StationPlugin *m_pPlugin;

	CMainDlg(StationPlugin *plugin, MQWindowBase& parent);
	~CMainDlg();

	BOOL OnTabChange(MQWidgetBase *sender, MQDocument doc);
	BOOL OnHide(MQWidgetBase *sender, MQDocument doc);
	BOOL OnAppAbout(MQWidgetBase *sender, MQDocument doc);
	BOOL OnButtonRefresh(MQWidgetBase *sender, MQDocument doc);
	BOOL OnButtonCurrentFreez(MQWidgetBase *sender, MQDocument doc);
	BOOL OnButtonAllFreez(MQWidgetBase *sender, MQDocument doc);
	BOOL OnCheckEvent(MQWidgetBase *sender, MQDocument doc);
	BOOL OnSliderEvent(MQWidgetBase *sender, MQDocument doc);

	void AddMessage(const wchar_t *message);
	void UpdateGUI();
	void RestoryLayout(MQBasePlugin *plg);
	void MemoryLayout(MQBasePlugin *plg);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_MAINDLG_H__35069CDB_1248_4479_8AE4_CC99D68849FF__INCLUDED_)
