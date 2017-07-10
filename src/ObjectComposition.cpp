
#include "stdafx.h"

#include "MQBasePlugin.h"
#include "MQPlugin.h"
#include <string>
#include <algorithm>
#include "ObjectComposition.h"

//misc
int StrToIntDef(std::wstring &s , int def) {
	int a;
	try {
		a = std::stoi(s);
	}
	catch(...){
		a = def;
	}
	return a;
}

float StrToFloatDef(std::wstring &s, float def) {
	float a;
	try {
		a = std::stof(s);
	}
	catch (...) {
		a = def;
	}
	return a;
}


std::wstring LowerCase(const std::wstring &s) {
	std::wstring ss;

	ss = s;
	std::transform(s.begin(), s.end(), ss.begin(), ::towlower);
	return ss;
}


std::wstring ConvertAnsiToUnicode(const std::string &s) {
	int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), s.length()+1, nullptr, 0);	//get length(wchr_t * len)
	std::wstring ss;
	wchar_t buf[1024];
	//ss.resize(len);
	//wchar_t *ptr = &ss[0];
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), s.length()+1, buf, 1024);
	ss = buf;

	return ss;
}

std::string ConvertUnicodeToAnsi(const std::wstring &s) {
	int len = WideCharToMultiByte(CP_THREAD_ACP, 0, s.c_str(), s.length()+1, NULL, 0, NULL, NULL);
	std::string ss;
	char buf[1024];
	//ss.resize(len);
	//char *ptr = &ss[0];
	len = WideCharToMultiByte(CP_THREAD_ACP, 0, s.c_str(), s.length()+1, buf, 1024, NULL, NULL);
	ss = buf;

	return ss;
}

std::vector<std::wstring>* DivideText(const wchar_t divchar, const std::wstring &s, int count) {
	std::vector<std::wstring>* sl;
	sl = new std::vector<std::wstring>();

	// divide
	{
		std::wstring ss;

		int n = s.length();
		ss = L"";
		for each (wchar_t c in s) {
			if (c == divchar) {
				sl->push_back(ss);
				ss = L"";
				continue;
			}
			ss = ss + c;
		}
		if (!ss.empty()) sl->push_back(ss);
	}

	// fixied length(line count)
	for (int i = sl->size(); i < count; i++) sl->push_back(L"");

	return sl;
}

MQPoint LineOnPoint(const MQPoint v0, const MQPoint v1, float a) {
	//２点間上の点を求める
	//a ... 0.0(v0)～1.0(v1)
	MQPoint r;
	float na;

	na = 1.0f - a;
	r.x = v0.x*na + v1.x*a;
	r.x = v0.y*na + v1.y*a;
	r.x = v0.z*na + v1.z*a;

	return r;
}

MQPoint FaceOnPoint(const MQPoint v0, const MQPoint v1, const MQPoint v2, const MQPoint v3, float ax, float ay) {
	/*
	[v0]---[v1]     [+y]
	 |       |       |
	 |       |       |
	[v3]---[v2]      *----[+x]
	*/

	MQPoint p1, p2, r;
	float a, na;

	//v0,v1上の点を求める
	a = ax;
	na = 1.0f - a;
	p1.x = v0.x*na + v1.x*a;
	p1.y = v0.y*na + v1.y*a;
	p1.z = v0.z*na + v1.z*a;
	//v3,v2上の点を求める
	a = ax;
	na = 1.0f - a;
	p2.x = v3.x*na + v2.x*a;
	p2.y = v3.y*na + v2.y*a;
	p2.z = v3.z*na + v2.z*a;
	//p1,p2上の点を求める
	a = ay;
	na = 1.0f - a;
	r.x = p2.x*na + p1.x*a;
	r.y = p2.y*na + p1.y*a;
	r.z = p2.z*na + p1.z*a;

	return r;
}

void ClearObjectLayer(MQObject o) {
	// DeleteFace
	{
		int n = o->GetFaceCount();
		for (int i = n-1; i >= 0; i--)o->DeleteFace(i, false);
	}
	// DeleteVertex
	{
		int n = o->GetVertexCount();
		for (int i = n-1; i >= 0; i--)o->DeleteVertex(i);
	}
	// compact
	o->Compact();
}

// data class
ObjectData::ObjectData() {
	CompositFlag = false;
}
ObjectData::~ObjectData() {

}
MaterialData::MaterialData() {
	ExistsOP = false;

	//setting param
	KeepAspect = 1;	
	ZeroCenter = 0;		
	Scale = 1.0;	

	ObjNo = 0;
	Obj = nullptr;
}
MaterialData::~MaterialData() {
	if (Obj != nullptr) {
		Obj->DeleteThis();
	}
}

// ObjectComposition calss
ObjectComposition::ObjectComposition(MQListBox* log)
{
	fEnable = false;
	fShowCurrent = true;
	fShowOther = true;
	fAlphaSync = false;
	fWireframe = false;
	fAlphaValue = 0.0;
	fRealtime = false;
	fFreezSubLayer = false;
	
	fBuildRefreshFlag = false;
	fBuildCount = 0;
	fDrawObjectRefreshFlag = false;
	fCacheDrawObject = nullptr;

	fLog = log;

	AddLog(L"class create");
	Init();
};

ObjectComposition::~ObjectComposition()
{
	Init();
	AddLog(L"class free");
};

void ObjectComposition::AddLog(std::wstring s) {
	if (fLog == nullptr) return;

	int index = fLog->AddItem(s);
	fLog->MakeItemVisible(index);
}

void ObjectComposition::Init(MQStationPlugin* plg, MQDocument doc) {
	AddLog(L"class DrawObject.init");
	for each(ObjectData* o in fObjectList) {
		delete o;
	}
	fObjectList.clear();

	for each(MaterialData* m in fMaterialList) {
		delete m;
	}
	fMaterialList.clear();

	fCacheObjectNames.clear();
	fCacheMaterialNames.clear();

	if ((doc != nullptr) && (plg != nullptr)) {
		if (fCacheDrawObject != nullptr) plg->DeleteDrawingObject(doc , fCacheDrawObject);
		fCacheDrawObject = nullptr;
		AddLog(L"class DrawObject.free");
	}
}

std::wstring ObjectComposition::ExtractOP(const std::wstring &s) {
	std::wstring ss;
	ss = LowerCase(s);

	int b = ss.find(L"<cmp,");	//,を含めてcmpaaaなどにヒットしないように
	int e = ss.find(L">", b);
	if (b == std::wstring::npos){
		b = ss.find(L"<cmp>");
		e = b + 4;
	}

	if (b == std::wstring::npos) return L"";
	if (e == std::wstring::npos) return L"";

	return ss.substr(b + 1, e - b - 1);
}

std::wstring ObjectComposition::ExtractName(const std::wstring &s) {
	std::wstring ss;
	ss = LowerCase(s);

	int b = ss.find(L"<cmp,");	//,を含めてcmpaaaなどにヒットしないように
	int e = ss.find(L">", b);
	if (b == std::wstring::npos) {
		b = ss.find(L"<cmp>");
		e = b + 4;
	}

	if (b == std::wstring::npos) return s;
	if (e == std::wstring::npos) return s;

	return ss.replace(b, b - e, L"");
}

int ObjectComposition::SearchObject(MQDocument doc, std::wstring name) {
	std::wstring s1, s2;

	s1 = LowerCase(name);
	{
		MQObject o;

		int n = doc->GetObjectCount();
		for (int i = 0; i < n; i++) {
			o = doc->GetObject(i);
			if (o == nullptr)continue;

			s2 = ConvertAnsiToUnicode(o->GetName());
			s2 = ExtractName(LowerCase(s2));	//aaa<cmp>も対象に出来るように名前だけ抜き出し
			if (s1 == s2) {
				return i;
			}
		}
	}
	return -1;
}

bool ObjectComposition::IsDestCompObject(MQObject o) {
	if (o == nullptr) return false;
	
	std::wstring s;
	s = ConvertAnsiToUnicode(o->GetName());
	s = LowerCase(s);
	if (s.find(L"<cmp>") != std::wstring::npos) return true;

	return false;
}

bool ObjectComposition::IsSrcCompObject(int a) {
	if (a < 0) return false;
	if (a >= fObjectList.size()) return false;

	return fObjectList[a]->CompositFlag;
}


bool ObjectComposition::CheckObjectNameChange(MQDocument doc) {
	std::string s;	//ansi 単なる比較だけなのでSDK基準で行う

	int n = doc->GetObjectCount();
	if (fCacheObjectNames.size() != n) return true;

	for (int i = 0; i < n; i++) {
		MQObject o = doc->GetObject(i);
		if (o != nullptr) {
			s = o->GetName();
			if (fCacheObjectNames[i] != s)return true;
		}
		else {
			if (fCacheObjectNames[i] != "")return true;
		}
	}

	return false;
}


bool ObjectComposition::CheckMaterialNameChange(MQDocument doc) {
	std::string s;	//ansi 単なる比較だけなのでSDK基準で行う

	int n = doc->GetMaterialCount();
	if (fCacheMaterialNames.size() != n) return true;

	for (int i = 0; i < n; i++) {
		MQMaterial m = doc->GetMaterial(i);
		if (m != nullptr) {
			s = m->GetName();
			if (fCacheMaterialNames[i] != s)return true;
		}
		else {
			if (fCacheMaterialNames[i] != "")return true;
		}
	}

	return false;
}


void ObjectComposition::BuildData() {
	fBuildRefreshFlag = true;
	fDrawObjectRefreshFlag = true;
}

void ObjectComposition::BuildDraw() {
	fDrawObjectRefreshFlag = true;
}

void ObjectComposition::BuildComposition(MQDocument doc, bool compact) {
	if (compact || (fBuildCount > 20)) {
		doc->Compact();
		fBuildCount = 0;
	}

	Init();

	AddLog(L"class BuildComposition");

	// SetLength
	{
		int n = doc->GetObjectCount();
		for (int i = 0; i < n; i++) {
			ObjectData* o = new ObjectData();
			fObjectList.push_back(o);
		}

		n = doc->GetMaterialCount();
		for (int i = 0; i < n; i++) {
			MaterialData* m = new MaterialData();
			fMaterialList.push_back(m);
		}
	}

	// NameCache
	{
		int n = doc->GetObjectCount();
		for (int i = 0; i < n; i++) {
			MQObject o = doc->GetObject(i);
			if (o != nullptr) {
				fCacheObjectNames.push_back( o->GetName() );//ansi string
			}
			else {
				fCacheObjectNames.push_back("");			//ansi string
			}
		}

		n = doc->GetMaterialCount();
		for (int i = 0; i < n; i++) {
			MQMaterial m = doc->GetMaterial(i);
			if (m != nullptr) {
				fCacheMaterialNames.push_back(m->GetName());	//ansi string
			}
			else {
				fCacheMaterialNames.push_back("");				//ansi string
			}

		}
	}

	// Material OP
	// example : hogehoge<oc,[ObjectName],[KeepAspect],[ZeroCenter]>
	{
		std::wstring s1;
		std::vector<std::wstring>* sl;
		MQMaterial mat;
		MQObject o;
		MaterialData* m;
	
		int n = doc->GetMaterialCount();
		for (int i = 0; i < n; i++) {
			m = fMaterialList[i];	
			mat = doc->GetMaterial(i);
			if (mat == nullptr)continue;
			if (m == nullptr)continue;

			s1 = ConvertAnsiToUnicode( mat->GetName());
			s1 = ExtractOP(s1);
			if (s1.empty())continue;
			
			sl = DivideText(L',', s1, 5);
			if (sl->at(0) == L"cmp") {
				//Param default
				m->ExistsOP = true;
				m->OPName = sl->at(0);
				m->ObjName = sl->at(1);
				m->Scale = 1.0f;
				m->ZeroCenter = false;
				m->KeepAspect = true;
				
				for (int j = 2; j < sl->size(); j++) {
					if (sl->at(j) == L"") continue;

					std::vector<std::wstring>* sl2;
					sl2 = DivideText(L':', sl->at(j), 2);

					std::wstring op, val;
					op = LowerCase(sl2->at(0));
					val = LowerCase(sl2->at(1));

					if (op == L"scale") {
						m->Scale = StrToFloatDef(val, 1.0f);
					}
					if (op == L"center") {
						if (val == L"zero") { m->ZeroCenter = true; }
						if (val == L"auto") { m->ZeroCenter = false; }
						if (val == L"1") { m->ZeroCenter = true; }
						if (val == L"0") { m->ZeroCenter = false; }
					}
					if (op == L"aspect") {
						if (val == L"keep") { m->KeepAspect = true; }
						if (val == L"stretch") { m->KeepAspect = false; }
						if (val == L"1") { m->KeepAspect = true; }
						if (val == L"0") { m->KeepAspect = false; }
					}
					delete sl2;
				}
				//FreezObject
				m->Obj = nullptr;
				m->ObjNo = SearchObject(doc, m->ObjName);
				o = doc->GetObject(m->ObjNo);
				if (o != nullptr) {
					//make freez object
					m->Obj = FreezObject(doc, o);
					CalcObjectBox(doc, m);
				}
			}
			delete sl;
		}
	}

	// Cacheフラグ
	fBuildRefreshFlag = false;
}

// object layers(child layers) freez!
MQObject ObjectComposition::FreezObject(MQDocument doc, MQObject obj) {
	MQObject o, c;

	o = obj->Clone();
	if (o == nullptr) return nullptr;

	// freez
	o->Freeze(MQOBJECT_FREEZE_ALL);

	// child layers
	{
		int n = doc->GetChildObjectCount(obj);
		for (int i = 0; i < n; i++) {
			c = doc->GetChildObject(obj, i);
			c = FreezObject(doc, c);
			if (c != nullptr) {
				o->Merge(c);
				c->DeleteThis();
			}
		}
	}

	// no polygon
	if (o->GetVertexCount() < 3) {
		o->DeleteThis();
		return nullptr;
	}

	// CompositFlag
	{
		int n = doc->GetObjectCount();
		for (int i = 0; i < n; i++) {
			if (obj == doc->GetObject(i)) {
				ObjectData* od = fObjectList[i];
				od->CompositFlag = true;
				break;
			}
		}
	}

	return o;
}

// calc composite area
// Composit対象Objectの最内包ボックスを作成
void ObjectComposition::CalcObjectBox(MQDocument doc, MaterialData* m) {
	if (m == nullptr) return;
	if (m->Obj == nullptr) return;

	MQObject o;
	float minx, miny, minz;
	float maxx, maxy, maxz;

	o = m->Obj;

	// area min max
	// 最内包ボックスの算出
	{
		minx = 0xFFFFFF;
		miny = 0xFFFFFF;
		minz = 0xFFFFFF;
		maxx = -0xFFFFFF;
		maxy = -0xFFFFFF;
		maxz = -0xFFFFFF;

		MQPoint v;
		int n = o->GetVertexCount();
		for (int i = 0; i<n; i++) {
			v = o->GetVertex(i);
			//x
			if (minx > v.x) minx = v.x;
			if (maxx < v.x) maxx = v.x;
			//y
			if (miny > v.y) miny = v.y;
			if (maxy < v.y) maxy = v.y;
			//z
			if (minz > v.z) minz = v.z;
			if (maxz < v.z) maxz = v.z;
		}
	}

	// zero center
	// 中心位置を原点(x,y,z=0,0,0)に持ってくるようにする
	// min,maxの0からの距離(abs)で大きい方を使用する
	if (m->ZeroCenter != 0) {
		float ll;

		//x
		if (abs(minx)<abs(maxx))
			ll = abs(maxx);
		else
			ll = abs(minx);
		minx = -ll;
		maxx = +ll;
		//y
		if (abs(miny)<abs(maxy))
			ll = abs(maxy);
		else
			ll = abs(miny);
		miny = -ll;
		maxy = +ll;
		//z
		if (abs(minz)<abs(maxz))
			ll = abs(maxz);
		else
			ll = abs(minz);
		minz = -ll;
		maxz = +ll;
	}

	// KeepAspect
	// 正立法体時にxyzアスペクト比が1:1:1になるように、xyzでmax-min間の距離の長いものに揃える
	if (m->KeepAspect != 0) {
		float xx, yy, zz, ll, cc;

		xx = abs(maxx - minx);
		yy = abs(maxy - miny);
		zz = abs(maxz - minz);
		ll = 0;
		if (ll < xx)ll = xx;
		if (ll < yy)ll = yy;
		if (ll < zz)ll = zz;
		ll = ll*0.5f;

		//x
		cc = (maxx + minx) * 0.5f;
		minx = cc - ll;
		maxx = cc + ll;
		//y
		cc = (maxy + miny) * 0.5f;
		miny = cc - ll;
		maxy = cc + ll;
		//z
		cc = (maxz + minz) * 0.5f;
		minz = cc - ll;
		maxz = cc + ll;
	}

	// FreezObjectの段階で２頂点以上あることが約束されているので例外はしない
	// 内包ボックス座標配列をセットする
	// ただし、頂点の並びは、MetasequoiaのCubeを生成した際のVertex順に合わせること
	// 尚、実際の計算には8頂点全ての情報を使うわけではありません。
	m->Box[0].x = minx;   //-,+,+
	m->Box[0].y = maxy;
	m->Box[0].z = maxz;
	m->Box[1].x = minx;   //-,-,+
	m->Box[1].y = miny;
	m->Box[1].z = maxz;
	m->Box[2].x = maxx;   //+,+,+
	m->Box[2].y = maxy;
	m->Box[2].z = maxz;
	m->Box[3].x = maxx;   //+,-,+
	m->Box[3].y = miny;
	m->Box[3].z = maxz;
	m->Box[4].x = maxx;   //+,+,-
	m->Box[4].y = maxy;
	m->Box[4].z = minz;
	m->Box[5].x = maxx;   //+,-,-
	m->Box[5].y = miny;
	m->Box[5].z = minz;
	m->Box[6].x = minx;   //-,+,-
	m->Box[6].y = maxy;
	m->Box[6].z = minz;
	m->Box[7].x = minx;   //-,-,-
	m->Box[7].y = miny;
	m->Box[7].z = minz;
}

void ObjectComposition::Draw(MQStationPlugin* plg, MQDocument doc) {
	if (!fEnable) {
		//delete draw object
		if (fCacheDrawObject != nullptr)plg->DeleteDrawingObject(doc, fCacheDrawObject);
		fCacheDrawObject = nullptr;
		return;
	}

	// CompositDataBuild
	if (fBuildRefreshFlag) BuildComposition(doc, false);

	// DrawingObjectの更新が必要かどうか
	if (!fDrawObjectRefreshFlag)return;

	if (fCacheDrawObject != nullptr)plg->DeleteDrawingObject(doc, fCacheDrawObject);
	// DrawingObject生成
	if (fWireframe) {
		fCacheDrawObject = plg->CreateDrawingObject(doc, MQStationPlugin::DRAW_OBJECT_LINE, false);
	}
	else {
		fCacheDrawObject = plg->CreateDrawingObject(doc, MQStationPlugin::DRAW_OBJECT_FACE, false);
	}

	// CompositDraw
	{
		MQObject o;

		int n = doc->GetObjectCount();
		int c = doc->GetCurrentObjectIndex();
		for (int i = 0; i < n; i++) {
			o = doc->GetObject(i);
			if (o == nullptr)continue;
			// Visible
			if (o->GetVisible() == 0x00000000)continue;

			if (i == c) {
				// Current
				if (fShowCurrent)ApplyComposition(doc, o, fCacheDrawObject, true, true);
			}
			else {
				// Other
				if (fShowOther)ApplyComposition(doc, o, fCacheDrawObject, true, true);
			}
		}
	}

	// RealTimeEdit
	if (fRealtime) {
		fDrawObjectRefreshFlag = true;
	}
	else {
		fDrawObjectRefreshFlag = false;
	}
}

bool ObjectComposition::ApplyComposition(MQDocument doc, MQObject obj, MQObject dest, bool mirror, bool canvisible) {
	if (obj == NULL) return false;
	if (!IsDestCompObject(obj)) return false;
	if (obj->GetVertexCount() < 8) return false;
	if (canvisible) {
		if (obj->GetVisible() == 0x000000) return false;
	}
	// support mirror
	// 対象objectがmirrorを使用している場合はフリーズさせる。
	bool MirrorFreez;

	MirrorFreez = false;
	if (mirror)
		if (obj->GetMirrorType() != MQOBJECT_MIRROR_NONE) {
			MirrorFreez = true;
			obj = obj->Clone();
			obj->Freeze(MQOBJECT_FREEZE_ALL);
		}

	// face,vertex count
	int fn, vn, msize;
	vn = obj->GetVertexCount();
	fn = obj->GetFaceCount();
	msize = fMaterialList.size();

	// optimize. check include composition material.
	// 事前にコンポジション対象のmaterialが存在するかをチェック。
	// vectorで領域を確保するまえに行う。
	{
		bool b = false;
		for (int i = 0; i < fn; i++) {
			int m = obj->GetFaceMaterial(i);
			if (m < 0) continue;
			if (m >= msize)continue;

			if (fMaterialList[m]->ExistsOP) {
				b = true;
				break;
			}
		}
		if (!b) return false;
	}

	// calc temp array
	// Cube検出用の計算用配列
	std::vector<int> group(vn , -1);	//size vn , default -1
	std::vector<int> material(vn, -1);

	// Face情報からVertexの繋がり（グループ情報）を構築する
	// ただし、Cubeのみ分かればいいので適当に端折ります。
	
	// 基本図形で生成を前提にしているので、faceとvertexの並びはいつも同じであることが約束されます。
	// 次にfaceを順に追っていった場合、並びが前の面とくっついている法則があるのでそれを利用します。
	// アルゴリズムは、順々にfaceの番号をvertexに書き込んでいきますが、vertexのなかにfaceの番号があれば、現在のfaceの番号と比べて最小値を求めて書き込みます。

	// 1枚目のface：この時点では独立たfaceなので、番号(0)がそのままvertexに書き込まれます。
	// 2枚目のface：並びが必ず前の面とくっついている特性上、vertexのどれかに最初のface番号(0)が存在します。最小をとって0が書き込まれます。
	// 以降のface：2枚目と同じです。vertexのどれかに最小値の0が存在するので0が書き込まれます。

	// ここまで来れば立方体の検出は簡単です。
	// vertexには[0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,4,4,4,4...]といった番号が並びます。
	// 立方体は8頂点なので、同じ番号が8個並んだ箇所が該当箇所です。

	// 変な図形のデータで8個並ぶ可能性については、事前にMaterialで弾いているので作り手のミスがない限り大丈夫でしょう。
	// 変則的な並びに対応させるにはかなり困難な代物です。特に速度も求められます。作り方を間違えればすぐバグります。
	// 絶対に正式には乗せられないプラグインですので、ご了承ください。

	{
		int vv[4];
		int a1, a2;

		for (int i = 0; i < fn; i++) {
			int mm = obj->GetFaceMaterial(i);	//face material no
			int nn = obj->GetFacePointCount(i);	//face point count

			// cubeだけの情報でいいので、4頂点以外はパス。
			if (nn != 4) continue;
			// materialが対象外ならそれもパス。
			if (mm < 0) continue;
			if (mm < msize) {
				if (!fMaterialList[mm]->ExistsOP) continue;
			}

			obj->GetFacePointArray(i, vv);	//face vertex index no array

			// セットされた値で最小の番号を求める
			// 全て未定義(-1)の場合は-1を返す
			a1 = 0xFFFFFF;
			for (int j = 0; j < nn; j++) {
				if (vv[j] >= vn) continue;		//例外処理
				a2 = group[ vv[j] ];
				if (a2 != -1)
					if (a2 < a1)a1 = a2;
			}
			if (a1 == 0xFFFFFF)a1 = -1;
			// 最小値をセットする
			// 未定義(-1)の場合はFaceNoで構わない
			if (a1 == -1) a1 = i;
			for (int j = 0; j < nn; j++) {
				a2 = vv[j];
				if (a2 >= vn) continue;		//例外処理
				group[a2] = a1;
				material[a2] = mm;
			}
		}
	}

	// グループ情報から、８頂点で構成されているものに対してモデル表示を適用する
	{
		MQObject o = nullptr;

		int StartIndex = 0;
		int gg = group[0];
		int cc = 1;
		for (int i = 1; i<vn; i++) {
			// グループ番号が同じならカウンタをインクリメントして次へ
			if (gg == group[i]) {
				cc++;
				continue;
			}
			// グループ番号に変化があった
			// 8,26頂点なら変形
			if (cc == 8) o = MakeTransformObject(doc, obj, StartIndex, material[StartIndex]);	//create clone & transform
			if (cc == 26) o = MakeTransformObject2(doc, obj, StartIndex, material[StartIndex]);	//create clone & transform
			if (o != nullptr) {
				ApplyComposition(doc, o, dest, mirror, false);
				dest->Merge(o);	//merge object
				o->DeleteThis();
				o = nullptr;
			}
			StartIndex = i;
			cc = 1;
			gg = group[i];
		}
		if (cc == 8) o = MakeTransformObject(doc, obj, StartIndex, material[StartIndex]);	//create clone & transform
		if (cc == 26) o = MakeTransformObject2(doc, obj, StartIndex, material[StartIndex]);	//create clone & transform
		if (o != nullptr) {
			ApplyComposition(doc, o, dest, mirror, false);
			dest->Merge(o);	//merge object
			o->DeleteThis();
			o = nullptr;
		}
	}

	group.clear();
	material.clear();

	// ミラーフリーズしている場合はCloneを作成したので解放
	if (MirrorFreez) {
		obj->DeleteThis();
	}

	return true;
}


MQObject ObjectComposition::MakeTransformObject(MQDocument doc, MQObject obj, int startindex, int materialno) {
	if (materialno < 0) return nullptr;
	if (materialno >= doc->GetMaterialCount()) return nullptr;
	if (materialno >= fMaterialList.size()) return nullptr;		//重要
	if (startindex < 0) return nullptr;
	if (startindex + 8 > obj->GetVertexCount()) return nullptr;
	// material check
	if (doc->GetMaterial(materialno) == nullptr) return nullptr;

	// Cube・Boxの頂点の並び
	/*
	  [6]-----[4]         [y+]
	  /|     / |           |
	[0]----[2] |           |
	|  |    |  |           *----[x+]
	| [7]---|-[5]         /
	| /     | /          /
	[1]----[3]        [z+]
	*/

	MaterialData* m;
	m = fMaterialList[materialno];
	if (m->Obj == nullptr) return nullptr;

	MQPoint vv[8];
	{
		for (int i = 0; i<8; i++)vv[i] = obj->GetVertex(startindex + i);
	}

	// Objectの生成・複製
	MQObject o;
	o = m->Obj->Clone();    //Clone
	o->SetVisible(0xFFFFFFFF);

	// 頂点情報を操作する
	{
		float nx, ny, nz; //x,y,z 1/(max-min)
		float mx, my, mz; //x,y,z min
		float ax, ay, az;
		float cx, cy, cz;

		// box中心座標
		cx = (m->Box[4].x + m->Box[7].x) * 0.5f;
		cy = (m->Box[4].y + m->Box[7].y) * 0.5f;
		cz = (m->Box[1].z + m->Box[7].z) * 0.5f;

		// size
		MQPoint box[8];
		{
			float isize = 1.0f;
			if (m->Scale != 1.0f)
				if (m->Scale > 0.0001f) isize = 1 / m->Scale;

			for (int i = 0; i < 8; i++) {
				box[i].x = (m->Box[i].x - cx) * isize + cx;
				box[i].y = (m->Box[i].y - cy) * isize + cy;
				box[i].z = (m->Box[i].z - cz) * isize + cz;
			}
		}

		// 計算高速化用逆数
		nx = 0.0;
		ny = 0.0;
		nz = 0.0;
		ax = box[4].x - box[7].x;
		ay = box[4].y - box[7].y;
		az = box[1].z - box[7].z;
		if (ax > 0.0001f) nx = 1 / ax;
		if (ay > 0.0001f) ny = 1 / ay;
		if (az > 0.0001f) nz = 1 / az;

		// 基準点
		mx = box[7].x;
		my = box[7].y;
		mz = box[7].z;

		int vn = o->GetVertexCount();
		for (int i = 0; i<vn; i++) {
			MQPoint p, p1, p2, p3;
			p = o->GetVertex(i);
			// 比率を求める
			ax = (p.x - mx) * nx;
			ay = (p.y - my) * ny;
			az = (p.z - mz) * nz;
			// Cubeの形状に沿って変形させる
			// 0-2-3-1 平面上の点P1を求める
			p1 = FaceOnPoint(vv[0], vv[2], vv[3], vv[1], ax, ay);
			// 6-4-5-7 平面上の点p2を求める
			p2 = FaceOnPoint(vv[6], vv[4], vv[5], vv[7], ax, ay);
			// p1,p2上の点を求める
			float a, na;
			a = az;
			na = 1.0f - a;
			p3.x = p2.x*na + p1.x*a;
			p3.y = p2.y*na + p1.y*a;
			p3.z = p2.z*na + p1.z*a;
			o->SetVertex(i, p3);
		}
	}
	return(o);
}

MQObject ObjectComposition::MakeTransformObject2(MQDocument doc, MQObject obj, int startindex, int materialno) {
	if (materialno < 0) return nullptr;
	if (materialno >= doc->GetMaterialCount()) return nullptr;
	if (materialno >= fMaterialList.size()) return nullptr;		//重要
	if (startindex < 0) return nullptr;
	if (startindex + 26 > obj->GetVertexCount()) return nullptr;
	// material check
	if (doc->GetMaterial(materialno) == nullptr) return nullptr;

	// 正面(f2)
	// front(z+)         middle              back(z-)
	// [0]  [1]  [6]     [19]  [24]  [7]     [18]  [13]  [12]       y+  z-
	// [2]  [3]  [8]     [21]  [*26] [9]     [20]  [15]  [14]   x-  +   x+
	// [4]  [5]  [10]    [23]  [25]  [11]    [22]  [17]  [16]   z+  y-  

	MaterialData* m;
	m = fMaterialList[materialno];
	if (m->Obj == nullptr) return nullptr;

	MQPoint vv[27];		//26vertex + center vertex
	{
		for (int i = 0; i<26; i++)vv[i] = obj->GetVertex(startindex + i);
		vv[26].x = (vv[21].x + vv[9].x) *0.5f;
		vv[26].y = (vv[24].y + vv[25].y) *0.5f;
		vv[26].z = (vv[3].z + vv[15].z) *0.5f;
		//vv[26].x = (vv[21].x + vv[9].x + vv[24].x + vv[25].x + vv[3].x + vv[15].x) * (1.0f / 6.0f);
		//vv[26].y = (vv[21].y + vv[9].y + vv[24].y + vv[25].y + vv[3].y + vv[15].y) * (1.0f / 6.0f);
		//vv[26].z = (vv[21].z + vv[9].z + vv[24].z + vv[25].z + vv[3].z + vv[15].z) * (1.0f / 6.0f);
	}

	// Objectの生成・複製
	MQObject o;
	o = m->Obj->Clone();    //Clone
	o->SetVisible(0xFFFFFFFF);

	// 頂点情報を操作する
	{
		float nx, ny, nz; // x,y,z 1/(max-min)
		float mx, my, mz; // x,y,z min
		float ax, ay, az;
		float cx, cy, cz; // src object center position

		// box中心座標
		cx = (m->Box[4].x + m->Box[7].x) * 0.5f;
		cy = (m->Box[4].y + m->Box[7].y) * 0.5f;
		cz = (m->Box[1].z + m->Box[7].z) * 0.5f;
		
		// size
		MQPoint box[8];
		{
			float isize = 1.0f;
			if (m->Scale != 1.0f)
				if (m->Scale > 0.0001f) isize = 1 / m->Scale;

			for (int i = 0; i < 8; i++) {
				box[i].x = (m->Box[i].x - cx) * isize + cx;
				box[i].y = (m->Box[i].y - cy) * isize + cy;
				box[i].z = (m->Box[i].z - cz) * isize + cz;
			}
		}

		// 計算高速化用逆数
		nx = 0.0;
		ny = 0.0;
		nz = 0.0;
		ax = box[4].x - box[7].x;
		ay = box[4].y - box[7].y;
		az = box[1].z - box[7].z;
		if (ax > 0.0001f) nx = 1 / (ax * 0.5f);		// 2x2x2に分けて計算するので半分で良い
		if (ay > 0.0001f) ny = 1 / (ay * 0.5f);
		if (az > 0.0001f) nz = 1 / (az * 0.5f);

		int vn = o->GetVertexCount();
		for (int i = 0; i<vn; i++) {
			MQPoint p, p1, p2, p3;
			p = o->GetVertex(i);

			// 該当立方体を導く
			int vv1[4], vv2[4];
			mx = cx; my = cy; mz = cz;
			if (p.x > cx) {
				if (p.y > cy) {
					if (p.z > cz){
						vv1[0] = 1; vv1[1] = 6; vv1[2] = 8; vv1[3] = 3;			//+,+,+
						vv2[0] = 24; vv2[1] = 7; vv2[2] = 9; vv2[3] = 26;
						mx = cx; my = cy; mz = cz;
					}
					else {
						vv1[0] = 24; vv1[1] = 7; vv1[2] = 9; vv1[3] = 26;		//+,+,-
						vv2[0] = 13; vv2[1] = 12; vv2[2] = 14; vv2[3] = 15;
						mx = cx; my = cy; mz = box[7].z;
					}
				}
				else {
					if (p.z > cz) {
						vv1[0] = 3; vv1[1] = 8; vv1[2] = 5; vv1[3] = 10;		//+,-,+
						vv2[0] = 26; vv2[1] = 9; vv2[2] = 11; vv2[3] = 25;
						mx = cx; my = box[1].y; mz = cz;
					}
					else {
						vv1[0] = 26; vv1[1] = 9; vv1[2] = 11; vv1[3] = 25;
						vv2[0] = 15; vv2[1] = 14; vv2[2] = 16; vv2[3] = 17;		//+,-,-
						mx = cx; my = box[1].y; mz = box[7].z;
					}
				}
			}
			else {
				if (p.y > cy) {
					if (p.z > cz) {
						vv1[0] = 0; vv1[1] = 1; vv1[2] = 3; vv1[3] = 2;			//-,+,+
						vv2[0] = 19; vv2[1] = 24; vv2[2] = 26; vv2[3] = 21;
						mx = box[0].x; my = cy; mz = cz;
					}
					else {
						vv1[0] = 19; vv1[1] = 24; vv1[2] = 26; vv1[3] = 21;		//-,+,-
						vv2[0] = 18; vv2[1] = 13; vv2[2] = 15; vv2[3] = 20;
						mx = box[0].x; my = cy; mz = box[7].z;
					}
				}
				else {
					if (p.z > cz) {
						vv1[0] = 2; vv1[1] = 3; vv1[2] = 5; vv1[3] = 4;			//-,-,+
						vv2[0] = 21; vv2[1] = 26; vv2[2] = 25; vv2[3] = 23;
						mx = box[0].x; my = box[1].y; mz = cz;
					}
					else {
						vv1[0] = 21; vv1[1] = 26; vv1[2] = 25; vv1[3] = 23;		//-,-,-
						vv2[0] = 20; vv2[1] = 15; vv2[2] = 17; vv2[3] = 22;
						mx = box[0].x; my = box[1].y; mz = box[7].z;
					}
				}
			}

			// 比率を求める
			ax = (p.x - mx) * nx;
			ay = (p.y - my) * ny;
			az = (p.z - mz) * nz;
			// Cubeの形状に沿って変形させる
			// 0-2-3-1 平面上の点P1を求める
			p1 = FaceOnPoint(vv[vv1[0]], vv[vv1[1]], vv[vv1[2]], vv[vv1[3]], ax, ay);
			// 6-4-5-7 平面上の点p2を求める
			p2 = FaceOnPoint(vv[vv2[0]], vv[vv2[1]], vv[vv2[2]], vv[vv2[3]], ax, ay);
			// p1,p2上の点を求める
			float a, na;
			a = az;
			na = 1.0f - a;
			p3.x = p2.x*na + p1.x*a;
			p3.y = p2.y*na + p1.y*a;
			p3.z = p2.z*na + p1.z*a;
			o->SetVertex(i, p3);
		}
	}
	return(o);
}

// freeze object layer
bool ObjectComposition::FreezeComposition(MQDocument doc, MQObject obj) {
	if (obj == nullptr) return false;

	// optimize&compaction layer data
	BuildComposition(doc, true);
	
	if (!IsDestCompObject(obj))return false;
	if (obj->GetVertexCount() < 8)return false;

	// apply composition
	MQObject o = obj->Clone();				//src...Clone作成
	//ClearObjectCompMaterialOnly(obj);		//該当materialのフェイスを削除
	ApplyComposition(doc, o, obj, false, false);	//Freezeの場合はMirror反映無し
	ClearObjectCompMaterialOnly(obj);		//入れ子の場合はもう一度消す必要がある
	obj->Compact();
	o->DeleteThis();

	// delete name op
	// どうにも名前を変更するとオブジェクトパネル上の表記が反映されない（一回クラッシュ）バグがあるのでそのままで。
	// フリーズしてさらに作っていくこともあるかもしれないので、そのままでいいかな

	/*
	std::wstring name;
	//名前から命令部分<???>を抜き去る
	{
		std::wstring s = ConvertAnsiToUnicode(obj->GetName());
		int b = s.find(L"<cmp>");
		if (b == std::string::npos) b = s.find(L"<CMP>");
		if (b != std::string::npos) {
			s = s.replace(b, 5, L"");	//5...length("<cmp>");
		}
		name = s;
	}

	//同名対処
	{
		std::wstring s1;
		s1 = LowerCase(name);

		int n = doc->GetObjectCount();
		for (int i = 0; i < n; i++) {
			MQObject o = doc->GetObject(i);
			if (o == nullptr)continue;

			std::wstring s2 = ConvertAnsiToUnicode(o->GetName());
			s2 = LowerCase(s2);

			if (s1 == s2) {
				name = name + L"_" + std::to_wstring(rand() & 0xFFFFFF);	//てきとー
				s1 = LowerCase(name);
			}
		}

	}

	{
		std::string s = ConvertUnicodeToAnsi(name);
		obj->SetName(s.c_str());
	}
	*/

	//再構築フラグを立てておこう
	BuildData();

	return true;
}

bool ObjectComposition::Freeze(MQDocument doc, MQObject obj, bool sublayer) {
	if (obj == nullptr) return false;
	
	if (sublayer) {
		// sublayer freeze
		// 一つでも成功するとtrueを返す
		bool b = false;
		int n = doc->GetChildObjectCount(obj);
		for (int i = 0; i < n; i++) {
			MQObject o = doc->GetChildObject(obj, i);
			if (o == nullptr)continue;
			b = b | FreezeComposition(doc, o);
		}
		b = b | FreezeComposition(doc, obj);
		return b;
	}
	else {
		// current layer only
		return FreezeComposition(doc, obj);
	}
}

bool ObjectComposition::FreezeAll(MQDocument doc) {
	bool b = false;
	
	int n = doc->GetObjectCount();
	for (int i = 0; i < n; i++) {
		MQObject o = doc->GetObject(i);
		if (o == nullptr)continue;

		b = b | Freeze(doc, o, false);
	}

	return b;
}

void ObjectComposition::ClearObjectCompMaterialOnly(MQObject obj) {
	// DeleteFace only composition use material
	{
		int n = obj->GetFaceCount();
		for (int i = n - 1; i >= 0; i--) {
			int m = obj->GetFaceMaterial(i);
			if (m < 0) continue;
			if (m >= fMaterialList.size()) continue;

			if (fMaterialList[m]->ExistsOP) {
				obj->DeleteFace(i, true);	//delete vertex on
			}
		}
	}
	// compact
	obj->Compact();
}

void ObjectComposition::MaterialAlphaSync(MQDocument doc, float alpha) {
	// fAlphaSyncによる分岐は上層で行ってください
	// if (!fAlphaSync) return;

	{
		int n = doc->GetMaterialCount();
		for (int i = 0; i < n; i++) {
			MQMaterial m = doc->GetMaterial(i);
			std::wstring s = ConvertAnsiToUnicode(m->GetName());
			s = LowerCase(s);
			if (s.find(L"<cmp") != std::wstring::npos) {
				m->SetAlpha(alpha);
			}
		}
	}
}