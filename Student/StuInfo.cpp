/******************************************************************* 
FileName: StuInfo.cpp
Desc	: 专门处理学生信息相关内容的类
*******************************************************************/ 
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
Parameter	: socket--连接教师机的SOCKET， stuInfo--学生信息结构体
Return		: 
Desc		: 
******************************************************************/
void CStuInfo::SendStuInfo(SOCKET socket, STUINFODATA& stuInfo)
{
	CSocketCenter socketCenter;
	// 通知教师端准备接收学生信息
	socketCenter.SendReadyInfo(socket, STUDENTINFO);
	// 发送登录学生的信息到教师端
	socketCenter.SendDataTCP(socket, (char*)&stuInfo, sizeof(STUINFODATA));
}
