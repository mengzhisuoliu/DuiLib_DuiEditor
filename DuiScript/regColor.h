#pragma once

namespace DuiLib
{

class regColor
{
	//////////////////////////////////////////////////////////////////////////
	static void CDuiColor_Construct(CDuiColor *obj)
	{
		new (obj) CDuiColor();
	}

	static void CDuiColor_Copy(const CDuiColor &other, CDuiColor *thisPointer)
	{
		new (thisPointer) CDuiColor(other);
	}

	static void CDuiColor_Destruct(CDuiColor *thisPointer)
	{
		thisPointer->~CDuiColor();
	}

	static void CDuiColor_Construct2(int other, CDuiColor *obj)
	{
		new (obj) CDuiColor(other);
	}

	static void CDuiColor_Construct3(UINT other, CDuiColor *obj)
	{
		new (obj) CDuiColor(other);
	}

	static void CDuiColor_Construct4(const CDuiString &other, CDuiColor *obj)
	{
		new (obj) CDuiColor(other.GetData());
	}

	static void CDuiColor_Construct5(BYTE r, BYTE g, BYTE b, CDuiColor *obj)
	{
		new (obj) CDuiColor(r,g,b);
	}

	static void CDuiColor_Construct6(BYTE a, BYTE r, BYTE g, BYTE b, CDuiColor *obj)
	{
		new (obj) CDuiColor(a,r,g,b);
	}

	static CDuiColor CDuiColor_opAssign(const CDuiColor &other, CDuiColor &dest)
	{
		dest = other;
		return dest;
	}
public:
	static bool Register(asIScriptEngine *engine)
	{
		int r = 0;

		//×¢²áCDuiColorÀà
		r = engine->RegisterObjectType("CColor", sizeof(CDuiColor), asOBJ_VALUE| asOBJ_APP_CLASS_CDAK   ); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("CColor", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(CDuiColor_Construct), asCALL_CDECL_OBJLAST);  assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("CColor", asBEHAVE_CONSTRUCT,  "void f(const CColor &in)", asFUNCTION(CDuiColor_Copy), asCALL_CDECL_OBJLAST);  assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("CColor", asBEHAVE_DESTRUCT,   "void f()", asFUNCTION(CDuiColor_Destruct),  asCALL_CDECL_OBJLAST); assert( r >= 0 );

		r = engine->RegisterObjectBehaviour("CColor", asBEHAVE_CONSTRUCT,  "void f(int)", asFUNCTION(CDuiColor_Construct2), asCALL_CDECL_OBJLAST);  assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("CColor", asBEHAVE_CONSTRUCT,  "void f(uint)", asFUNCTION(CDuiColor_Construct3), asCALL_CDECL_OBJLAST);  assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("CColor", asBEHAVE_CONSTRUCT,  "void f(const string &in)", asFUNCTION(CDuiColor_Construct4), asCALL_CDECL_OBJLAST);  assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("CColor", asBEHAVE_CONSTRUCT,  "void f(uint r, uint g, uint b)", asFUNCTION(CDuiColor_Construct5), asCALL_CDECL_OBJLAST);  assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("CColor", asBEHAVE_CONSTRUCT,  "void f(uint a, uint r, uint g, uint bn)", asFUNCTION(CDuiColor_Construct6), asCALL_CDECL_OBJLAST);  assert( r >= 0 );

		r = engine->RegisterObjectMethod("CColor", "CColor &opAssign(const CColor &in)", asMETHODPR(CDuiColor, operator =, (const CDuiColor&), CDuiColor&), asCALL_THISCALL); assert( r >= 0 );

		r = engine->RegisterObjectMethod("CColor", "bool FromString(const string &in)", asMETHOD(CDuiColor, FromString), asCALL_THISCALL);  assert( r >= 0 );
		r = engine->RegisterObjectMethod("CColor", "string ToString()", asMETHOD(CDuiColor, ToString), asCALL_THISCALL);  assert( r >= 0 );

		return r >= 0;
	}
};

} //namespace DuiLib

