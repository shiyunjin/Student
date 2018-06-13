#include "stdafx.h"
#include "ControlCenter.h"
#include "StuInfo.h"


/******************************************************************
Function	: CControlCenter
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:27
Parameter	: stuInfo--自定义结构体，学生的信息
Return		: 
Desc		: 构造方法
******************************************************************/
CControlCenter::CControlCenter(STUINFODATA stuInfo)
: m_port(0)
, m_socketMsg(INVALID_SOCKET)
, m_pScreenMonitor(NULL)
, m_stuInfo(stuInfo)
, m_pMulticastDlg(NULL)
, now_cast(false)
{
}

CControlCenter::~CControlCenter()
{
	DeleteSocketMsg();

	DeletepScreenMonitor();

	DeleteSocketMulticast();

	DeletepMulticastDlg();
}


/******************************************************************
Function	: DeletepMulticastDlg
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:27
Return		: void
Desc		: 释放new 分配的内存
******************************************************************/
void CControlCenter::DeletepMulticastDlg()
{
	if (m_pMulticastDlg != NULL)
	{
		delete m_pMulticastDlg;
		m_pMulticastDlg = NULL;
	}
}


/******************************************************************
Function	: DeleteSocketMulticast
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:27
Return		: void
Desc		: 关闭SOCKET 连接
******************************************************************/
void CControlCenter::DeleteSocketMulticast()
{
	if (m_socketMulticast != INVALID_SOCKET)
	{
		closesocket(m_socketMulticast);
		m_socketMulticast = INVALID_SOCKET;
	}
}


/******************************************************************
Function	: DeletepScreenMonitor
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:27
Return		: void
Desc		: 释放new 分配的内存
******************************************************************/
void CControlCenter::DeletepScreenMonitor()
{
	if (m_pScreenMonitor != NULL)
	{
		delete m_pScreenMonitor;
		m_pScreenMonitor = NULL;
	}
}


/******************************************************************
Function	: DeleteSocketMsg
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:27
Return		: void
Desc		: 关闭SOCKET 连接
******************************************************************/
void CControlCenter::DeleteSocketMsg()
{
	if (m_socketMsg != INVALID_SOCKET)
	{
		closesocket(m_socketMsg);
		m_socketMsg = INVALID_SOCKET;
	}
}


/******************************************************************
Function	: SetConnectInfo
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:28
Parameter	: IP--IP地址
Parameter	: port--端口号
Return		: void
Desc		: 设置IP 地址和端口的信息
******************************************************************/
void CControlCenter::SetConnectInfo(char* IP, int port)
{
	strcpy_s(m_IP, IP);
	m_port = port;
}


/******************************************************************
Function	: ConnectClient
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:28
Return		: void
Desc		: 连接教师机，得到SOCKET
******************************************************************/
void CControlCenter::ConnectClient()
{

	char ip[255];
	//AfxMessageBox(L"1");
	Sleep(2000);
	strcpy(ip, m_socketCenter.GetIP());
	while (strcmp(ip,"")==0) {
		Sleep(1000);
		strcpy(ip, m_socketCenter.GetIP());
		//AfxMessageBox(CA2CT(ip));
	}

	//AfxMessageBox(CA2CT(ip));
	while (true)
	{
		SetConnectInfo(ip, CONNECT_PORT);
		m_socketMsg = m_socketCenter.InitSocket(m_IP, m_port);
		if (m_socketMsg == SOCKET_ERROR)
		{
			Sleep(3000);
			continue;
		}
		else
		{
			break;
		}
	}
	RecvRequest();
}


/******************************************************************
Function	: RecvRequest
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:28
Return		: void
Desc		: 不断监听来处教师机的请求
******************************************************************/
void CControlCenter::RecvRequest()
{
	// 监听教师机广播的SOCKET（IP 组播技术）
	m_socketMulticast = m_socketCenter.InitMulticastSocket(MULTICAST_TRANS_PORT,
		MULTICAST_IP);

	MSGTYPE msgType;
	while (true)
	{
		
		memset(&msgType, 0, sizeof(MSGTYPE));
		// 不断监听来自客户端的请求
		int result = m_socketCenter.RecvDataTCP(m_socketMsg,
			(char*)&msgType, sizeof(MSGTYPE));
		if (result == SOCKET_ERROR) {
			if(now_cast)
				EndMulticast();
			ConnectClient();
			break;
		}
		if (result == 0)
		{
			//AfxMessageBox(_T("result = 0"));
			continue;
		}
		this->ExecuteRequest(msgType);
	}
}


/******************************************************************
Function	: ExecuteRequest
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:29
Parameter	: msgType--自定义结构体，消息的信息
Return		: void
Desc		: 执行接收到的请求
******************************************************************/
void CControlCenter::ExecuteRequest(MSGTYPE msgType)
{
	switch (msgType.msgID)
	{
	case STUDENTINFO:
		SendStuInfo();
		break;
	case BEGINSCREENMONITOR:
		BeginScreenMonitor();
		break;
	case ENDSCREENMONITOR:
		EndScreenMonitor();
		break;
	case BEGINMULTICAST:
		BeginMulticast();
		break;
	case ENDMULTICAST:
		EndMulticast();
		break;
	default:
		AfxMessageBox(_T("Unkonw message"));
		return;
	}
}


/******************************************************************
Function	: BeginMulticast
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:29
Return		: void
Desc		: 接收来自教师机的屏幕图像数据并调用显示方法
******************************************************************/
void CControlCenter::BeginMulticast()
{
	now_cast = true;
	DeletepMulticastDlg();
	m_pMulticastDlg = new CMulticastDlg();
	m_pMulticastDlg->SetSocketMulticast(m_socketMulticast);
	m_pMulticastDlg->SetIsMulticastStop(false);
	CloseHandle(::CreateThread(0, 0, OnShowMulticastDlg, (LPVOID)m_pMulticastDlg, 0, NULL));
	m_hMulticast = ::CreateThread(0, 0, OnBeginMulticast,
		(LPVOID)m_pMulticastDlg, 0, NULL);
}


/******************************************************************
Function	: OnBeginMulticast
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:29
Parameter	: self--CMulticast 类对象
Return		: DWORD WINAPI
Desc		: 教师机广播线程运行方法
******************************************************************/
DWORD WINAPI CControlCenter::OnBeginMulticast(LPVOID self)
{
	CMulticastDlg* multicastDlg = (CMulticastDlg*)self;
	multicastDlg->SetScreenData();
	//	multicastDlg->DoModal();
	return 0;
}


/******************************************************************
Function	: OnShowMulticastDlg
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:29
Parameter	: self--CMulticastDlg 类对象
Return		: DWORD WINAPI
Desc		: 显示教师机广播模态对话框的线程运行方法
******************************************************************/
DWORD WINAPI CControlCenter::OnShowMulticastDlg(LPVOID self)
{
	CMulticastDlg* multicastDlg = (CMulticastDlg*)self;
	multicastDlg->DoModal();
	// 	multicastDlg->Create(IDD_MULTICAST);
	// 	multicastDlg->ShowWindow(SW_NORMAL);
	return 0;
}


/******************************************************************
Function	: EndMulticast
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:29
Return		: void
Desc		: 停止接收来自教师机的屏幕图像数据
to-do       : 线程会上面SOCKET 的RecvFrom 上而导致线程无法结束
******************************************************************/
void CControlCenter::EndMulticast()
{
	now_cast = false;
	m_pMulticastDlg->SetIsMulticastStop(true);
	DWORD exitCode = 0;
	// 关闭模态对话框
	m_pMulticastDlg->CloseModalDlg();

	GetExitCodeThread(m_hMulticast, &exitCode);
	while (STILL_ACTIVE == exitCode)
	{
		Sleep(100);
		GetExitCodeThread(m_hMulticast, &exitCode);
		//TerminateThread(m_hMulticast, 0);
	}
	//	TerminateThread(m_hMulticast, exitCode);

	CloseHandle(m_hMulticast);
	m_hMulticast = NULL;
	DeletepMulticastDlg();
	//	delete m_pMulticastDlg;
	//	m_pMulticastDlg = NULL;
}


/******************************************************************
Function	: BeginScreenMonitor
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:31
Return		: void
Desc		: 发送当前机子的屏幕图像数据到教师机
******************************************************************/
void CControlCenter::BeginScreenMonitor()
{
	DeletepScreenMonitor();
	m_pScreenMonitor = new CScreenMonitor();
	m_pScreenMonitor->SetSocketMsg(m_socketMsg);
	m_pScreenMonitor->SetIsScreenMonitorEnd(true);
	m_hScreenDlg = (::CreateThread(0, 0, OnBeginScreenMonitor,
		(LPVOID)m_pScreenMonitor, 0, NULL));
}


/******************************************************************
Function	: OnBeginScreenMonitor
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:31
Parameter	: self--CScreenMonitor 类对象
Return		: DWORD WINAPI
Desc		: 学生机屏幕监控线程运行方法
******************************************************************/
DWORD WINAPI CControlCenter::OnBeginScreenMonitor(LPVOID self)
{
	CScreenMonitor* screenThread = (CScreenMonitor*)self;
	screenThread->SendScreenData();
	return 0;
}


/******************************************************************
Function	: EndScreenMonitor
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:31
Return		: void
Desc		: 停止发送本机的屏幕图像数据到教师机
******************************************************************/
void CControlCenter::EndScreenMonitor()
{
	m_pScreenMonitor->SetIsScreenMonitorEnd(false);
	DWORD exitCode = 0;
	::GetExitCodeThread(m_hScreenDlg, &exitCode);
	while (exitCode == STILL_ACTIVE)
	{
		Sleep(10);
		::GetExitCodeThread(m_hScreenDlg, &exitCode);
	}
	CloseHandle(m_hScreenDlg);
	DeletepScreenMonitor();
// 	delete m_pScreenMonitor;
// 	m_pScreenMonitor = NULL;
}


/******************************************************************
Function	: SendStuInfo
Author		: shiyunjin(luoyibin_001@163.com)
Date		: 2018-6-13 14:31
Return		: void
Desc		: 发送登录的学生的信息到教师端
******************************************************************/
void CControlCenter::SendStuInfo()
{
	CStuInfo StuInfo;
	StuInfo.SendStuInfo(m_socketMsg, m_stuInfo);
}