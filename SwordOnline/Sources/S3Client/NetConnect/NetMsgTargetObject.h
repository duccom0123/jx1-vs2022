/*******************Editer	: duccom0123 EditTime:	2024/06/12 11:48:44*********************
//	网络消息的目地对象
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-10-8
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

struct iKNetMsgTargetObject
{
	virtual void	AcceptNetMsg(void* pMsgData) = 0;
};