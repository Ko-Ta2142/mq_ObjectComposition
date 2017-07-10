// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABOUTDLG_H__07D51ED4_FBAB_4CD1_8FF7_9274DAF5CF64__INCLUDED_)
#define AFX_ABOUTDLG_H__07D51ED4_FBAB_4CD1_8FF7_9274DAF5CF64__INCLUDED_

#include "MQWidget.h"

class CAboutDlg : public MQDialog
{
public:
	CAboutDlg(MQWindowBase& parent);

	BOOL OnCloseCmd(MQWidgetBase *sender, MQDocument doc);
};

#endif // !defined(AFX_ABOUTDLG_H__07D51ED4_FBAB_4CD1_8FF7_9274DAF5CF64__INCLUDED_)
