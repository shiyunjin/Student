#include "stdafx.h"
#include "StuInfo.h"
#include "SocketCenter.h"


CStuInfo::CStuInfo()
{
}


CStuInfo::~CStuInfo()
{
}


/******************************************************************
Function	: SendStuInfo
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:15
Parameter	: socket--连接教师机的SOCKET
Parameter	: stuInfo--学生信息结构体
Return		: void
Desc		: 发送学生信息
******************************************************************/
void CStuInfo::SendStuInfo(SOCKET socket, STUINFODATA& stuInfo)
{
	CSocketCenter socketCenter;
	// 通知教师端准备接收学生信息
	socketCenter.SendReadyInfo(socket, STUDENTINFO);
	// 发送登录学生的信息到教师端
	socketCenter.SendDataTCP(socket, (char*)&stuInfo, sizeof(STUINFODATA));
}
