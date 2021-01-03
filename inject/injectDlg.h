
// injectDlg.h: 头文件
//

#pragma once

class CinjectDlg : public CDialogEx {	
public:
	// 构造函数
	CinjectDlg(CWnd* pParent = nullptr);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INJECT_DIALOG };
#endif

protected:
	// DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
