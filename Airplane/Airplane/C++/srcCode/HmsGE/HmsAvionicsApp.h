#pragma once
#include "HmsAviContext.h"

class CHmsAvionicsApp
{
public:
	CHmsAvionicsApp(CHmsAviContext * pContext = nullptr);
	~CHmsAvionicsApp();

	/**
	*初始化程序
	*param   
	*returns 
	*by [3/23/2017 song.zhang079] 
	*/
	virtual bool Init();

	/**
	*执行程序
	*param   
	*returns 
	*by [3/23/2017 song.zhang079] 
	*/
	int Exec();

private:
	CHmsAviContext *	m_pContext;
};

