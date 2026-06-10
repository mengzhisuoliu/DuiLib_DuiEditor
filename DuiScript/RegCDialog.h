#pragma once

namespace DuiLib
{

class regCDialog
{
protected:
	static CUIDialog *CUIDialog_Ref_Factory() { return new CUIDialog(); };
	static CUIDialog *CUIDialog_Ref_Factory2(CDuiString sSkinFile) { return new CUIDialog(sSkinFile); };
	
public:
	static bool Register(asIScriptEngine *engine)
	{
		int r = 0;
		r = engine->RegisterObjectType("CDialog", 0, asOBJ_REF);  assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("CDialog", asBEHAVE_FACTORY, "CDialog@ f()", asFunctionPtr(CUIDialog_Ref_Factory), asCALL_CDECL); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("CDialog", asBEHAVE_FACTORY, "CDialog@ f(string sSkinFile)", asFunctionPtr(CUIDialog_Ref_Factory2), asCALL_CDECL); assert( r >= 0 );
		
		r = engine->RegisterObjectBehaviour("CDialog", asBEHAVE_ADDREF, "void f()", asMETHOD(CUIDialog,__AddRef), asCALL_THISCALL); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("CDialog", asBEHAVE_RELEASE, "void f()", asMETHOD(CUIDialog,__ReleaseRef), asCALL_THISCALL); assert( r >= 0 );
		
		r = engine->RegisterObjectMethod("CDialog", "void RegisterNotify(string,string)", asMETHODPR(CUIDialog, RegScriptNotify, (LPCTSTR, LPCTSTR), void), asCALL_THISCALL); assert( r >= 0 );
		
		REG_METHOD_FUNPR2("CDialog", CUIDialog, UINT, DoModal, (UIWND)); assert( r >= 0 );

		//暂不支持 弹出非模态窗口
		//REG_METHOD_FUNPR2("CDialog", CUIDialog, void, ShowDialog, (HWND hWndParent)); assert( r >= 0 );
		
		REG_METHOD_FUNPR2("CDialog", CUIDialog, void, Close, (UINT)); assert( r >= 0 );
		
		return r >= 0;
	}
	
};

} //namespace DuiLib

