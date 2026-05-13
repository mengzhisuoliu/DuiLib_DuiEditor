#pragma once

#ifdef DUILIB_SDL
namespace DuiLib {



class UILIB_API CUIApplicationSDL : public CUIApplicationBase
{
public:
	CUIApplicationSDL(void);
	virtual ~CUIApplicationSDL(void);

	virtual bool InitInstance(int argc, char* argv[])  override;
	virtual void Run() override;

protected:
	virtual int ExitInstance() override;
};
}  //namespace DuiLib {
#endif //#ifdef DUILIB_SDL