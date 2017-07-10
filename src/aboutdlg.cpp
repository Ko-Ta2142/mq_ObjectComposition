// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StationPlugin.h"

#include "aboutdlg.h"

CAboutDlg::CAboutDlg(MQWindowBase& parent) : MQDialog(parent)
{
	SetTitle(plugin_namew);

	MQLabel *label = CreateLabel(this, plugin_copyrightw);
	label->SetAlignment(MQLabel::ALIGN_CENTER);
	label->SetWordWrap(true);

	MQButton *btn = CreateButton(this, L"OK");
	btn->AddClickEvent(this, &CAboutDlg::OnCloseCmd);
}

BOOL CAboutDlg::OnCloseCmd(MQWidgetBase *sender, MQDocument doc)
{
	Close();
	return FALSE;
}
