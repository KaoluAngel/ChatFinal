
// ChatFinalDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ChatFinal.h"
#include "ChatFinalDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int PORT = 6000;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChatFinalDlg 对话框



CChatFinalDlg::CChatFinalDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATFINAL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatFinalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//绑定---------------------------------------------------------
	DDX_Control(pDX, IDC_EDIT_RECV, m_richEditCtrl);
	DDX_Control(pDX, IDC_EDIT_SEND, m_sendRichEditCtrl);
	DDX_Control(pDX, IDC_IPADDRESS, m_ipAddr);

}

BEGIN_MESSAGE_MAP(CChatFinalDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SEND, &CChatFinalDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


// CChatFinalDlg 消息处理程序

BOOL CChatFinalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码

	m_ipAddr.SetWindowText(L"127.0.0.1");	//设置默认IP

	WSADATA wsaData;
	int nErr(0);
	CString strErr;

	nErr = WSAStartup(MAKEWORD(2, 2), &wsaData);


	if (0 != nErr)
	{
		strErr.Format(_T("初始化WSA环境失败。\r\n工作线程即将关闭，服务器将停止工作。\r\n(错误代码：%d)"),
			WSAGetLastError());
		MessageBox(strErr, _T("错误"), MB_ICONERROR);
	}
	//起TCP服务器线程
	HANDLE hThread1;
	unsigned threadID1;
	hThread1 = (HANDLE)_beginthreadex(NULL, 0, &RecvServerThread, (LPVOID)this, 0, &threadID1);
	CloseHandle(hThread1);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CChatFinalDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChatFinalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChatFinalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CChatFinalDlg::OnBnClickedBtnSend()
{

	//获取用户名
	GetDlgItemText(IDC_EDIT_USER, m_userName);
	if (m_userName=="")
	{
		AfxMessageBox(L"请取一个昵称 =￣□￣=!");
		return;
	}
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(FALSE);



	CString strMsg;
	CString strSta=L"";
	DWORD dwIp;
	m_ipAddr.GetAddress(dwIp);

	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == sockClient)
	{

		AfxMessageBox(L"创建套接字失败(客户端)!");
		return ;
	}

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(dwIp);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(PORT);

	if (SOCKET_ERROR == connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
	{
		AfxMessageBox(L"连接失败，对方可能不在线 T_T");
		closesocket(sockClient);
		return ;
	}

	GetDlgItemText(IDC_EDIT_SEND, strMsg);
	//判断输入是否为空
	if (strMsg=="")
	{
		AfxMessageBox(L"请输入内容  ￢_￢!");
		return;
	}

	strSta += L"→_→  \r\n已发送:"+strMsg;
	strMsg= L"From:"+m_userName+L"  "+strMsg;


	USES_CONVERSION;
	char sendBuf[100];
	WideCharToMultiByte(CP_OEMCP, 0, (LPCTSTR)strMsg, -1, sendBuf, 260, 0, false);

	if (SOCKET_ERROR == send(sockClient, sendBuf, 100, 0))
	{
		AfxMessageBox(L"发送消息失败 哦豁~");
		closesocket(sockClient);
		return ;
	}

	SetDlgItemText(IDC_EDIT_STA, strSta);
	closesocket(sockClient);
	SetDlgItemText(IDC_EDIT_SEND, L"");
	
	return ;
}


unsigned __stdcall CChatFinalDlg::RecvServerThread(void* pArguments)
{
	CChatFinalDlg* pDlg = (CChatFinalDlg*)pArguments;

	SOCKET m_socketServ;

	m_socketServ = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == m_socketServ)
	{

		AfxMessageBox(L"创建套接字失败！");
	

		return FALSE;
	}

	SOCKADDR_IN addrFrom;
	addrFrom.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrFrom.sin_family = AF_INET;
	addrFrom.sin_port = htons(PORT);

	if (SOCKET_ERROR == bind(m_socketServ, (SOCKADDR*)&addrFrom, sizeof(SOCKADDR)))
	{
		AfxMessageBox(L"bind error!");
		return -1;
	}

	if (SOCKET_ERROR == listen(m_socketServ, 5))
	{
		CString strError;
		strError.Format(L"%d", WSAGetLastError());
		AfxMessageBox(strError);
		return -1;
	}

	SOCKADDR_IN addrClient;

	while(1)
	{
		int len = sizeof(SOCKADDR);
		SOCKET conn = accept(m_socketServ, (SOCKADDR*)&addrClient, &len);
		if (conn == INVALID_SOCKET)
		{
			AfxMessageBox(L"conn error!");
			return -1;
		}
	
		pDlg->RecvMsg(conn, addrClient);
		closesocket(conn);
	}
	closesocket(m_socketServ);
	return 0;
}


void CChatFinalDlg::RecvMsg(SOCKET& sock, SOCKADDR_IN& addr)
{
	char recvMsgBuf[100] = { 0 };
	CString strMsg;
	CString strNewMsg;
	CString strOldMsg;
	CString strIp;

	while(1)
	{
		int recvCount = recv(sock, recvMsgBuf, 100, 0);
		if (0 == recvCount)
		{
			break;
		}
		strNewMsg += recvMsgBuf;
	}
	USES_CONVERSION;
	strNewMsg += "\r\n";
	m_richEditCtrl.GetWindowText(strOldMsg);
	strMsg += strNewMsg;
	ShowMsg(strMsg);
	return;
}

//显示对面发来的消息。

void CChatFinalDlg::ShowMsg(CString& strMsg)
{
	static int nLineCount = 0;
	if (nLineCount != 0)
		nLineCount = m_richEditCtrl.GetLineCount();
	int nOldEnd = m_richEditCtrl.GetWindowTextLength();
	m_richEditCtrl.SetSel(nOldEnd, nOldEnd);
	m_richEditCtrl.ReplaceSel(strMsg);

	int nEnd = m_richEditCtrl.GetWindowTextLength();
	m_richEditCtrl.SetSel(nOldEnd - nLineCount, nEnd);
	m_richEditCtrl.HideSelection(TRUE, FALSE);
	++nLineCount;
	m_richEditCtrl.LineScroll(1);
	//Invalidate();
}
