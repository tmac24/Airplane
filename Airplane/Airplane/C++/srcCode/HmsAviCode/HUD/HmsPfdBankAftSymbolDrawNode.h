#pragma once
#include "display/HmsDrawNode.h"
#include "HmsMultLineDrawNode.h"

struct HmsDrawIndex
{
	int nIndex;
	int nIndexCnt;

	inline HmsDrawIndex(int index)
	{
		nIndex = index;
		nIndexCnt = 1;
	}

	inline HmsDrawIndex(int index, int nCnt)
	{
		nIndex = index - (nCnt-1);
		nIndexCnt = nCnt;
	}

	inline const HmsDrawIndex& operator=(int index)
	{
		nIndex = index;
		nIndexCnt = 1;
		return *this;
	}
};

class CHmsPfdBankAftSymbolDrawNode : public HmsAviPf::CHmsMultLineDrawNode
{
public:
	CHmsPfdBankAftSymbolDrawNode();
	~CHmsPfdBankAftSymbolDrawNode();

	CREATE_FUNC(CHmsPfdBankAftSymbolDrawNode);

	virtual bool Init();

	virtual void onDrawShape();

	void InitAftSymbol(float fAftWidth , float fLineWidth);
	void InitBankScale(float fRadius, float fMaxLineLength, float fLineWidth);

	void DrawBgIndex(const HmsDrawIndex & nIndex);

	void SetBank(float fBank);

private:
	HmsDrawIndex		m_nIndexAftSymbol;
	HmsDrawIndex		m_nIndexBankScale0;
	HmsDrawIndex		m_nIndexBankScale10to20;
	HmsDrawIndex		m_nIndexBankScale30;
	HmsDrawIndex		m_nIndexBankScale45to60;
	HmsDrawIndex		m_nIndexBankScale90To180;

	float				m_fBank;
};

