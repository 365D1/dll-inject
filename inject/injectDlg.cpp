
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
	DDX_Control(pDX, ID_CMB_THREAD, cmbThread);
}

BEGIN_MESSAGE_MAP(CinjectDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_BTN_INJECT, &CinjectDlg::OnBnClickedBtnInject)
	ON_BN_CLICKED(ID_BTN_UNJECT, &CinjectDlg::OnBnClickedBtnUnject)
	ON_BN_CLICKED(ID_BTN_SELECT, &CinjectDlg::OnBnClickedBtnSelect)
	ON_BN_CLICKED(ID_BTN_REFRESH, &CinjectDlg::OnBnClickedBtnRefresh)
END_MESSAGE_MAP()


// CinjectDlg 消息处理程序
BOOL CinjectDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();
	// 设置大图标
	SetIcon(m_hIcon, TRUE);
	// 设置小图标
	SetIcon(m_hIcon, FALSE);

	// 初始化
	this->refreshProcess();

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

void CinjectDlg::refreshProcess() {
	cmbThread.SetEditSel(0, -1);
	cmbThread.Clear();
	cmbThread.ResetContent();
	// 获取进程快照
	this->hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if(this->hSnapshot == INVALID_HANDLE_VALUE) {
		MessageBox("遍历进程失败", "提示");
		return;
	}
	PROCESSENTRY32 processInfo = { 0 };
	processInfo.dwSize = sizeof(PROCESSENTRY32);
	do {
		CString processName = processInfo.szExeFile;
		DWORD processPid = processInfo.th32ProcessID;
		if(processPid != 0) {
			CString pid;
			pid.Format("%-6d", processPid);
			CString item;
			item.Format("%s | %s", pid, processName);
			cmbThread.AddString(item);
		}
	} while(Process32Next(this->hSnapshot, &processInfo));
	CloseHandle(this->hSnapshot);
}

// 注入按钮
void CinjectDlg::OnBnClickedBtnInject() {
	int current = cmbThread.GetCurSel();
	if(-1 == current) {
		MessageBox("请选择进程", "提示");
		return;
	}
	if(this->dllPath.IsEmpty()) {
		MessageBox("请选择 DLL 文件", "提示");
		return;
	}
	CString select;
	int curPos = 0;
	cmbThread.GetLBText(current, select);
	select = select.Tokenize("|", curPos).Trim();
	// 打开进程句柄
	DWORD pid = (DWORD)_ttol(select);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(NULL == hProcess) {
		MessageBox("打开进程失败", "提示");
		return;
	}

	// 写入内存数据 dll 路径地址	
	int size = this->dllPath.GetLength() + 1;
	char* buff = new char[size];
	memcpy(buff, this->dllPath.GetBuffer(), size);

	// 申请内存空间
	LPVOID dllAddr = VirtualAllocEx(hProcess, NULL, strlen(buff), MEM_COMMIT, PAGE_READWRITE);
	if(NULL == dllAddr) {
		MessageBox("申请内存空间失败", "提示");
		// 释放进程句柄
		CloseHandle(hProcess);
		return;
	}
	CString test;
	test.Format("%p", dllAddr);
	OutputDebugString(test);

	BOOL writeFlag = WriteProcessMemory(hProcess, dllAddr, buff, strlen(buff), NULL);
	if(!writeFlag) {
		MessageBox("写入DLL地址失败", "提示");
		// 释放进程句柄
		CloseHandle(hProcess);
		return;
	}

	// 获取 LoadLibrary 函数地址
	HMODULE hModule = GetModuleHandle("Kernel32.dll");
	if(NULL == hModule) {
		MessageBox("Kernel32 获取失败", "提示");
		// 释放进程句柄
		CloseHandle(hProcess);
		return;
	}
	LPVOID loadAddr = GetProcAddress(hModule, "LoadLibraryA");
	if(NULL == loadAddr) {
		loadAddr = GetProcAddress(hModule, "LoadLibraryW");
		if(NULL == loadAddr) {
			MessageBox("LoadLibrary 函数地址获取失败", "提示");
			// 释放进程句柄
			CloseHandle(hProcess);
			return;
		}
	}

	// 创建远程线程 执行 LoadLibrary
	HANDLE injectFlag = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadAddr, dllAddr, 0, NULL);
	if(NULL == injectFlag) {
		MessageBox("注入DLL失败", "提示");
		// 释放进程句柄
		CloseHandle(hProcess);
		return;
	}
	MessageBox("注入DLL成功", "提示");
	// 释放进程句柄
	CloseHandle(hProcess);
}

// 卸载按钮
void CinjectDlg::OnBnClickedBtnUnject() {
	// TODO: 在此添加控件通知处理程序代码
}

// 选择按钮
void CinjectDlg::OnBnClickedBtnSelect() {
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, "*.dll||", NULL);
	if(fileDlg.DoModal()) {
		this->dllPath = fileDlg.GetPathName();
		GetDlgItem(ID_EDI_PATH)->SetWindowText(this->dllPath);
	}
}

// 刷新按钮
void CinjectDlg::OnBnClickedBtnRefresh() {
	this->refreshProcess();
}