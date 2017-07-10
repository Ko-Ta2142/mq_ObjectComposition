//#pragma once
#ifndef _ObjectComposition_H_
#define _ObjectComposition_H_

#include "MQBasePlugin.h"
#include "MQWidget.h"
#include <string>

// data class
class ObjectData {
protected:
public:
	bool CompositFlag;
	ObjectData();
	~ObjectData();
};
class MaterialData {
protected:
public:
	bool ExistsOP;
	std::wstring OPName;
	std::wstring ObjName;

	//setting param
	int KeepAspect;		//比率を保つ
	int ZeroCenter;		//0,0,0点を中心点とする
	float Scale;		//スケール

	int ObjNo;			//ObjNo
	MQObject Obj;		//FreezObject（対象となるオブジェクトをフリーズしたもの）
	MQPoint Box[8];		//内包領域

	MaterialData();
	~MaterialData();
};

class ObjectComposition {
protected:
	std::vector<ObjectData*> fObjectList;		//
	std::vector<MaterialData*> fMaterialList;
	bool fEnable;
	bool fShowCurrent;
	bool fShowOther;
	bool fWireframe;
	float fAlphaValue;
	bool fRealtime;
	bool fFreezSubLayer;
	bool fAlphaSync;
	float fAlpha;

	// log
	MQListBox* fLog;

	// Build
	bool fBuildRefreshFlag;
	int fBuildCount;
	std::vector<std::string> fCacheObjectNames;		//最適化用。これらは名前変更の比較しかしないのでSDK基準のansiで取り扱う。
	std::vector<std::string> fCacheMaterialNames;

	// DrawObject
	bool fDrawObjectRefreshFlag;
	MQObject fCacheDrawObject;

	// log
	void AddLog(std::wstring s);
	
	bool ApplyComposition(MQDocument doc, MQObject obj, MQObject dest, bool mirror, bool canvisible);
	void BuildComposition(MQDocument doc, bool compact);
	bool FreezeComposition(MQDocument doc, MQObject obj);

	std::wstring ExtractOP(const std::wstring &s);
	std::wstring ExtractName(const std::wstring &s);

	MQObject FreezObject(MQDocument doc, MQObject obj);
	void CalcObjectBox(MQDocument doc, MaterialData* m);
	MQObject MakeTransformObject(MQDocument doc, MQObject obj, int startindex, int materialno);
	MQObject MakeTransformObject2(MQDocument doc, MQObject obj, int startindex, int materialno);
	void ClearObjectCompMaterialOnly(MQObject obj);
public:
	ObjectComposition(MQListBox* log=nullptr);
	~ObjectComposition();
	void Init(MQStationPlugin* plg=NULL, MQDocument doc=NULL);

	void Draw(MQStationPlugin* plg, MQDocument doc);

	bool IsSrcCompObject(int a);
	bool IsDestCompObject(MQObject o);
	int SearchObject(MQDocument doc, const std::wstring name);
	bool CheckObjectNameChange(MQDocument doc);
	bool CheckMaterialNameChange(MQDocument doc);
	void BuildData();
	void BuildDraw();

	bool Freeze(MQDocument doc, MQObject obj, bool sublayer);
	bool FreezeAll(MQDocument doc);
	void MaterialAlphaSync(MQDocument doc, float alpha);

	// property
	bool pEnable() { return fEnable; }
	void pEnable(bool b) { fEnable = b; }
	bool pShowCurrent() { return fShowCurrent; }
	void pShowCurrent(bool b) { fShowCurrent = b; }
	bool pShowOther() { return fShowOther; }
	void pShowOther(bool b) { fShowOther = b; }
	bool pWireframe() { return fWireframe; }
	void pWireframe(bool b) { fWireframe = b; }
	float pAlphaValue() { return fAlphaValue; }
	void pAlphaValue(float f) { fAlphaValue = f; }
	float pRealtime() { return fRealtime; }
	void pRealtime(bool b) { fRealtime = b; }
	float pFreezSubLayer() { return fFreezSubLayer; }
	void pFreezSubLayer(bool b) { fFreezSubLayer = b; }

	bool pAlphaSync() { return fAlphaSync; }
	void pAlphaSync(bool b) { fAlphaSync = b; }
	float pAlpha() { return fAlpha; }
	void pAlpha(float f) { fAlpha = f; }
};

#endif
