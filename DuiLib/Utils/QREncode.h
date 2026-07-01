// QR_Encode.h : CQR_Encode 类声明和接口定义
// 日期 2006/05/17	版本 1.22	Psytec Inc.

#if !defined(AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD38__INCLUDED_)
#define AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace DuiLib
{
	namespace uinner{
/////////////////////////////////////////////////////////////////////////////
// 常量

// 纠错等级
#define QR_LEVEL_L	0
#define QR_LEVEL_M	1
#define QR_LEVEL_Q	2
#define QR_LEVEL_H	3

// 数据模式
#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3

// 版本(型号)分组
#define QR_VRESION_S	0 // 1 ～ 9
#define QR_VRESION_M	1 // 10 ～ 26
#define QR_VRESION_L	2 // 27 ～ 40

#define MAX_ALLCODEWORD	 3706 // 总码字数最大值
#define MAX_DATACODEWORD 2956 // 数据码字最大值(版本40-L)
#define MAX_CODEBLOCK	  153 // 块数据码字数最大值(含RS码字)
#define MAX_MODULESIZE	  177 // 单边模块数最大值

// 位图绘制时边距
#define QR_MARGIN	4


/////////////////////////////////////////////////////////////////////////////
typedef struct tagRS_BLOCKINFO
{
	int ncRSBlock;		// RS块数
	int ncAllCodeWord;	// 块内码字数
	int ncDataCodeWord;	// 数据码字数(码字数 - RS码字数)

} RS_BLOCKINFO, *LPRS_BLOCKINFO;


/////////////////////////////////////////////////////////////////////////////
// QR码版本(型号)相关信息

typedef struct tagQR_VERSIONINFO
{
	int nVersionNo;	   // 版本(型号)编号(1～40)
	int ncAllCodeWord; // 总码字数

	// 以下数组下标为纠错率(0 = L, 1 = M, 2 = Q, 3 = H) 
	int ncDataCodeWord[4];	// 数据码字数(总码字数 - RS码字数)

	int ncAlignPoint;	// 对齐图案坐标数
	int nAlignPoint[6];	// 对齐图案中心坐标

	RS_BLOCKINFO RS_BlockInfo1[4]; // RS块信息(1)
	RS_BLOCKINFO RS_BlockInfo2[4]; // RS块信息(2)

} QR_VERSIONINFO, *LPQR_VERSIONINFO;


/////////////////////////////////////////////////////////////////////////////
// CQR_Encode 类

class CQR_Encode
{
// 构造/析构
public:
	CQR_Encode();
	~CQR_Encode();

public:
	int m_nLevel;		// 纠错等级
	int m_nVersion;		// 版本(型号)
	uiBool m_bAutoExtent;	// 版本(型号)自动扩展指定标志
	int m_nMaskingNo;	// 掩码图案编号

public:
	int m_nSymbleSize;
	BYTE m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE]; // [x][y]
	// bit5:功能模块(掩码对象外)标志
	// bit4:功能模块绘制数据
	// bit1:编码数据
	// bit0:掩码后编码绘制数据
	// 通过和0x20进行逻辑或判断功能模块，通过和0x11进行逻辑或绘制(最终转换为BOOL值)

private:
	int m_ncDataCodeWordBit; // 数据码字位长
	BYTE m_byDataCodeWord[MAX_DATACODEWORD]; // 输入数据编码区域

	int m_ncDataBlock;
	BYTE m_byBlockMode[MAX_DATACODEWORD];
	int m_nBlockLength[MAX_DATACODEWORD];

	int m_ncAllCodeWord; // 总码字数(含RS纠错数据)
	BYTE m_byAllCodeWord[MAX_ALLCODEWORD]; // 总码字数计算区域
	BYTE m_byRSWork[MAX_CODEBLOCK]; // RS码字计算工作区

// 数据编码相关函数
public:
	uiBool EncodeData(int nLevel, int nVersion, uiBool bAutoExtent, int nMaskingNo, LPBYTE lpsSource, int ncSource = 0);

private:
	int GetEncodeVersion(int nVersion, LPBYTE lpsSource, int ncLength);
	uiBool EncodeSourceData(LPBYTE lpsSource, int ncLength, int nVerGroup);

	int GetBitLength(BYTE nMode, int ncData, int nVerGroup);

	int SetBitStream(int nIndex, WORD wData, int ncData);

	uiBool IsNumeralData(unsigned char c);
	uiBool IsAlphabetData(unsigned char c);
	uiBool IsKanjiData(unsigned char c1, unsigned char c2);

	BYTE AlphabetToBinaly(unsigned char c);
	WORD KanjiToBinaly(WORD wc);

	void GetRSCodeWord(LPBYTE lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord);

// 模块配置相关函数
private:
	void FormatModule();

	void SetFunctionModule();
	void SetFinderPattern(int x, int y);
	void SetAlignmentPattern(int x, int y);
	void SetVersionPattern();
	void SetCodeWordPattern();
	void SetMaskingPattern(int nPatternNo);
	void SetFormatInfoPattern(int nPatternNo);
	int CountPenalty();
};
}}
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_)