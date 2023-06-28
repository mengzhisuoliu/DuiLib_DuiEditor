#pragma once

#ifdef DUILIB_WIN32
#include "atlimage.h"
namespace DuiLib
{
/** Count of blind piece */
#define BLIND_PIECE	8

class UILIB_API CImageBoxExUI : public CControlUI
{
	DECLARE_DUICONTROL(CImageBoxExUI)
public:
	/** Animation type */
	enum AnimationType
	{
		kNone,
		kAnimationFade,
		kAnimationSlideLeft,
		kAnimationSlideRight,
		kAnimationSlideTop,
		kAnimationSlideBottom,
		kAnimationBlindLeft,
		kAnimationBlindRight,
		kAnimationBlindTop,
		kAnimationBlindBottom,
		kAnimationCoverLeft,
		kAnimationCoverRight,
		kAnimationCoverTop,
		kAnimationCoverBottom
	};

protected:
	enum 
	{ 
		DEFAULT_TIMERID = 20,
	};
public:
	CImageBoxExUI(void);
	~CImageBoxExUI(void);

	virtual LPCTSTR GetClass() const override;
	virtual LPVOID	GetInterface(LPCTSTR pstrName) override;

	virtual void DoInit() override;
	virtual bool DoPaint(UIRender *pRender, const RECT& rcPaint, CControlUI* pStopControl) override;
	virtual void DoEvent(TEventUI& event) override;	
	virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
	virtual void Move(SIZE szOffset, bool bNeedInvalidate = true) override;
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
	
	void LoadImageList();
	
	/** 
	 * Get image.
	 */
	CImage* GetImageAt(int iIndex) const;

	/** 
	 * Get the number of images.
	 */
	int		GetSize() const;

	/**
	 * Set animation duration.
	 * @param iTime - Duration in milliseconds.
	 */
	void	SetDuration(int iTime);

	/**
	 * Set frame retention duration.
	 * @param iTime - Duration in milliseconds.
	 */
	void	SetRetention(int iTime);

	/** 
	 * Set animation type
	 * @param iType - Animation type.
	 */
	void	SetAnimationType(AnimationType iType);

	void Play();
	void Stop();

	/** 
	 * Get linear interpolation 
	 */
	static int GetLinearInterpolation(int x1, int x2, double delta);
protected:
	/** 
	 * Get the next frame image index.
	 */
	int		GetNextFrameIndex() const;

	/**
	 * Perform alpha animation. 
	 * @param dwTick - Current system time tick.
	 */
	void	AnimationRender_Alpha(DWORD dwTick);

	/** 
	 * Perform slide animation.
	 * @param dwTick - Current system time tick.
	 */
	void    AnimationRender_Slide(DWORD dwTick);

	/** 
	 * Perform blind animation.
	 * @param dwTick - Current system time tick.
	 */
	void    AnimationRender_Blind(DWORD dwTick);

	/** 
	 * Perform cover animation.
	 * @param dwTick - Current system time tick.
	 */
	void    AnimationRender_Cover(DWORD dwTick);

public:
	void OnTimer(UINT_PTR idEvent);
protected:
	CDuiString m_strImagePath;
	int m_nTimerEscape;
	UINT_PTR m_idEventTimer;
private:
private:
	/** Image container */
	std::vector<CImage *> m_arrImages;

	/** Current frame image index */
	int				m_iIndex;

	/** Animation duration in milliseconds */
	int				m_iDuration;

	/** Frame retention in milliseconds */
	int				m_iRetention;

	/** Current animation type */
	AnimationType	m_iCurAnimType;

	/** Off screen bitmap */
	CImage*			m_pOffScreenImage;

	/** Starting tick of current animation */
	DWORD			m_dwAnimStartTick;

	/** Delay tick for next animation */
	DWORD			m_dwDelayStartTick;

	/** Is delayed */
	BOOL			m_bIsDelayed;

	/** Is animating */
	BOOL			m_bIsAnimating;

	/** Play toggle */
	BOOL			m_bPlay;
};


}
#endif //#ifdef DUILIB_WIN32