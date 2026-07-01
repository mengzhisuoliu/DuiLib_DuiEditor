#ifndef __DPI_Base_H__
#define __DPI_Base_H__
#pragma once

namespace DuiLib
{
	class UILIB_API CDpiBase
	{
	public:
		CDpiBase();

		virtual int GetMainMonitorDPI();
	public:
		virtual UINT GetDPI();
		virtual UINT GetScale();
		void SetScale(UINT uDPI);

		virtual int ScaleInt(int iValue);
		virtual int  ScaleIntBack(int iValue);

		CDuiRect ScaleRect(const CDuiRect& rcRect);
		void ScaleRect(RECT *pRect);
		void ScaleRectBack(RECT *pRect);

		CDuiPoint ScalePoint(CDuiPoint ptPoint);
		void ScalePoint(CDuiPoint *pPoint);
		void ScalePointBack(CDuiPoint *pPoint);

		CDuiSize ScaleSize(const CDuiSize& szSize);
		void ScaleSize(SIZE *pSize);
		void ScaleSizeBack(SIZE *pSize);


	protected:
		int m_nScaleFactor;
		int m_nScaleFactorSDA;
	};
}
#endif //__DPI_Base_H__
