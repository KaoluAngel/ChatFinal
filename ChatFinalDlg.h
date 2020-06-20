
// ChatFinalDlg.h: 头文件
//

#pragma once


// CChatFinalDlg 对话框
class CChatFinalDlg : public CDialogEx
{
// 构造
public:
	CChatFinalDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATFINAL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSend();

	static unsigned __stdcall RecvServerThread(void* pArguments);

	void RecvMsg(SOCKET& sock, SOCKADDR_IN& addr);
	void ShowMsg(CString& strMsg);

	CString m_userName;

private:

	CRichEditCtrl m_richEditCtrl;
	CRichEditCtrl m_sendRichEditCtrl;
	CIPAddressCtrl m_ipAddr;

};
