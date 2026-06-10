#ifndef __UIGLOBAL_H__
#define __UIGLOBAL_H__

#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API UIGlobal
	{
	public:
		static UIGlobal* GetInstance();
		static void Release();

		UIRenderFactory *GetRenderFactory();
		static UIRender *CreateRenderTarget();
		static UIFont *CreateFont();
		static UIPen *CreatePen();
		static UIBitmap *CreateBitmap();
		static UIBrush *CreateBrush();
		static UIImage *CreateImage();

		//////////////////////////////////////////////////////////////////////////
		//HSL颜色转换
		static void RGBtoHSL(DWORD ARGB, float* H, float* S, float* L);

		//HSL颜色转换
		static void HSLtoRGB(DWORD* ARGB, float H, float S, float L);

		//HSL颜色转换
		static CDuiColor AdjustHslColor(CDuiColor dwColor, short H, short S, short L);

		//HSL颜色转换
		static void  AdjustHslImage(bool bUseHSL, UIImage *imageInfo, short H, short S, short L);

		//alpha utilities
		static void CheckAlphaColor(CDuiColor& dwColor);
	private:
		UIGlobal();
		UIGlobal(const UIGlobal &src);
		const UIGlobal& operator=(const UIGlobal& src);
		~UIGlobal();

	private:
		CStdRefPtr<UIRenderFactory> m_renderEngineFactory;
	};

} // namespace DuiLib

#endif // __UIRENDER_H__
