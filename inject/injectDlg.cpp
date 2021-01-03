
// injectDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "inject.h"
#include "injectDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CinjectDlg::CinjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INJECT_DIALOG, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CinjectDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CinjectDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CinjectDlg 消息处理程序
BOOL CinjectDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();
	// 设置大图标
	SetIcon(m_hIcon, TRUE);
	// 设置小图标
	SetIcon(m_hIcon, FALSE);

	// TODO: 在此添加额外的初始化代码

	return TRUE;
}

// 最小化按钮等
void CinjectDlg::OnPaint() {
	if(IsIconic()) {
		// 用于绘制的设备上下文
		CPaintDC dc(this);
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
	} else {
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示
HCURSOR CinjectDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

