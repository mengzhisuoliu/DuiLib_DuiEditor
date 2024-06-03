#pragma once

namespace DuiLib
{

class regCBufferUI
{
protected:
	static void CBufferUI_Construct(CBufferUI *obj)
	{
		new (obj) CBufferUI();
	}

	static void CBufferUI_Destruct(CBufferUI *thisPointer)
	{
		thisPointer->~CBufferUI();
	}

	static void CBufferUI_AddStringA(const CDuiString &w, CBufferUI &dest)
	{
		CDuiStringA a = w;
		dest.AddStringA(a);
	}

	static void CBufferUI_AddStringW(const CDuiString &w, CBufferUI &dest)
	{
		dest.AddStringW(w);
	}
public:
	static bool Register(asIScriptEngine *engine)
	{
		int r =0;

		//ע����
		r = engine->RegisterObjectType("CBuffer", sizeof(CBufferUI), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("CBuffer", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(CBufferUI_Construct), asCALL_CDECL_OBJLAST);  assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("CBuffer", asBEHAVE_DESTRUCT,   "void f()", asFUNCTION(CBufferUI_Destruct),  asCALL_CDECL_OBJLAST); assert( r >= 0 );
		
		//r = engine->RegisterObjectMethod("CBufferUI", "LPBYTE GetBuffer()", asMETHOD(CBufferUI, GetBuffer), asCALL_THISCALL);  assert( r >= 0 );
		REG_METHOD_FUNPR2("CBuffer", CBufferUI, LPBYTE, GetBuffer, ());
		REG_METHOD_FUNPR2("CBuffer", CBufferUI, int, GetLength, ());
		REG_METHOD_FUNPR2("CBuffer", CBufferUI, void, SetMaxBufferSize, (int size));
		REG_METHOD_FUNPR2("CBuffer", CBufferUI, void, SetBufferSize, (int size));
		REG_METHOD_FUNPR2("CBuffer", CBufferUI, void, InitBuffer, (int newBufferSize));
		REG_METHOD_FUNPR2("CBuffer", CBufferUI, void, AddInt, (int n));	
		REG_METHOD_FUNPR2("CBuffer", CBufferUI, void, AddByte, (BYTE buffer));

		r = engine->RegisterObjectMethod("CBuffer", "void AddStringA(string &buf)", asFUNCTIONPR(CBufferUI_AddStringA, (const CDuiString &, CBufferUI &), void), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = engine->RegisterObjectMethod("CBuffer", "void AddStringW(string &buf)", asFUNCTIONPR(CBufferUI_AddStringW, (const CDuiString &, CBufferUI &), void), asCALL_CDECL_OBJLAST); assert( r >= 0 );

		REG_METHOD_FUNPR2("CBuffer", CBufferUI, void, AddBuffer, (LPCVOID buffer, int len));
		REG_METHOD_FUNPR2("CBuffer", CBufferUI, void, DeleteBuffer, (int len));
		r = engine->RegisterObjectMethod("CBuffer", "void CopyFrom(CBuffer &buf)", asMETHOD(CBufferUI, CopyFrom), asCALL_THISCALL);  assert( r >= 0 );
		REG_METHOD_FUNPR2("CBuffer", CBufferUI, int, GetMemSize, ());
		REG_METHOD_FUNPR2("CBuffer", CBufferUI, BYTE, GetAt, (int n));
		return r >= 0;	
	}
};

} //namespace DuiLib

