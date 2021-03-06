﻿
// MainFrm.cpp: CMainFrame 类的实现
//

#include "pch.h"
#include "framework.h"
#include "UltrasoundSimulation.h"
#include "ImageSynthesis.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "UltrasoundSimulationView.h"
#include"CvvImage.h"


CDialog* pk;//声明一个全局变量，在c++语言中
int sstart;
int send1;

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_FILE_MRU_FILE2, &CMainFrame::OnSynthesis)
	ON_COMMAND(ID_FILE_MRU_FILE3, &CMainFrame::OnReflection)
	ON_COMMAND(ID_FILE_MRU_FILE4, &CMainFrame::OnScattering)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame() noexcept
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 允许用户定义的工具栏操作: 
	InitUserToolbars(nullptr, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == nullptr)
	{
		// 加载用户定义的工具栏图像
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// 基类将执行真正的工作

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != nullptr)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类
	CRect rect;
	GetClientRect(&rect);  //获取客户区坐标(左上 右下)
	if (!m_wndSplitter.CreateStatic(this, 2, 2))
		return FALSE;      //创建静态分割器窗口 2行*2列
	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CUltrasoundSimulationView), CSize(rect.Width() / 2, rect.Height() / 2), pContext))
		return FALSE;      //创建位置(0,0)
	if (!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CUltrasoundSimulationView), CSize(rect.Width() / 2, rect.Height()/2), pContext))
		return FALSE;      //创建位置(0,1)
	if (!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CUltrasoundSimulationView), CSize(rect.Width() / 2, rect.Height()/2), pContext))
		return FALSE;      //创建位置(1,0)
	if (!m_wndSplitter.CreateView(1, 1, RUNTIME_CLASS(CUltrasoundSimulationView), CSize(rect.Width() / 2, rect.Height()/2), pContext))
		return FALSE;      //创建位置(1,1)
	m_pProjectView = (CUltrasoundSimulationView*)m_wndSplitter.GetPane(0, 0);
	SetActiveView(m_pProjectView);
	return TRUE;

	//return CFrameWndEx::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnFileOpen()
{
	//COpenFileDlg file(TRUE,NULL,NULL,OFN_ALLOWMULTISELECT,NULL,this); 
	//file.DoModal();
	CString StrFolder;
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(BROWSEINFO));
	bi.hwndOwner = GetSafeHwnd();
	bi.ulFlags = 80;
	bi.lpszTitle = "选择文件夹";
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (idl == NULL)
		return;
	SHGetPathFromIDList(idl, StrFolder.GetBuffer(MAX_PATH));
	StrFolder.ReleaseBuffer();
	strcpy(m_pProjectView->m_File_Index1, StrFolder);

	//CString dest_Root = StrFolder;
	int zSize = 0;
	CFileFind finder;
	CString file_Root;
	file_Root = StrFolder;
	BOOL	bWorking = finder.FindFile(file_Root + "\\*.*");
	while (bWorking)//第一次得到xSize,ySize,zSize的值
	{
		bWorking = finder.FindNextFile();
		zSize++;
	}
	zSize = zSize - 2;
	sstart = 1;
	send1 = zSize;
	m_pProjectView->OpenCTFile();
	/*pk = new CDialog;
	int nResult = pk->Create(IDD_FILE_PROGRESS);
	pk->ShowWindow(SW_SHOW);*/
	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_SEPARATOR), "正在载入CT数据......", true);
	
	//CChooseSliceNum chooseslicedlg;
	//chooseslicedlg.m_edit1.Format("%d", zSize);
	//chooseslicedlg.m_edit2.Format("%d", 1);
	//chooseslicedlg.m_edit3.Format("%d", zSize);
	//slicenum = zSize;
	//chooseslicedlg.DoModal();


	//if (chooseslicedlg.nResult == false)
	//{
	//	m_pProjectView->m_File_Finish = FALSE;
	//	CString StrFolder;
	//	BROWSEINFO bi;
	//	memset(&bi, 0, sizeof(BROWSEINFO));
	//	bi.hwndOwner = GetSafeHwnd();
	//	bi.ulFlags = 80;
	//	bi.lpszTitle = "选择文件夹";
	//	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	//	if (idl == NULL)
	//		return;
	//	SHGetPathFromIDList(idl, StrFolder.GetBuffer(MAX_PATH));
	//	StrFolder.ReleaseBuffer();
	//	strcpy(m_pProjectView->m_File_Index1, StrFolder);
	//	CString dest_Root = StrFolder;
	//	int zSize = 0;
	//	CFileFind finder;
	//	CString file_Root;
	//	file_Root = StrFolder;
	//	BOOL	bWorking = finder.FindFile(file_Root + "\\*.*");
	//	while (bWorking)//第一次得到xSize,ySize,zSize的值
	//	{
	//		bWorking = finder.FindNextFile();
	//		zSize++;
	//	}
	//	zSize = zSize - 2;

	//	chooseslicedlg.m_edit1.Format("%d", zSize);
	//	chooseslicedlg.m_edit2.Format("%d", 1);
	//	chooseslicedlg.m_edit3.Format("%d", zSize);
	//	slicenum = zSize;
	//	chooseslicedlg.DoModal();
	//}

	//if (chooseslicedlg.nResult == true)
	//{
	//	m_pProjectView->OpenCTFile();
	//	pk = new CFileProDlg;
	//	int nResult = pk->Create(IDD_FILE_PROGRESS);
	//	pk->ShowWindow(SW_SHOW);
	//	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_SEPARATOR), "正在载入CT数据......", true);
	//}
}

UINT ReadCTFile(LPVOID v)
{
	CUltrasoundSimulationView* pv;
	pv = (CUltrasoundSimulationView*)v;
	pv->OpenFile(pv->m_File_Index1);
	/*pk->SendMessage(WM_UPDATESTATUSBAR3);
	pv->SendMessage(WM_UPDATESTATUSBAR);*/
	return 0;
}


void CMainFrame::OnSynthesis()
{
	ImageSynthesis IS;
	IS.Synthesis("image/ReflectionImage.png", "image/ScatteringImage.png");

	IplImage* src = cvLoadImage("image/SynthesisImage.png", -1);
	CDC* pDC = m_wndSplitter.GetPane(1, 1)->GetDC();
	HDC hdc = pDC->GetSafeHdc();
	CRect rect;
	CvvImage cimg;

	m_wndSplitter.GetPane(1, 1)->GetClientRect(&rect);
	cimg.CopyOf(src, src->nChannels);
	cimg.DrawToHDC(hdc, &rect);
	ReleaseDC(pDC);
	cimg.Destroy();
}

void CMainFrame::OnReflection()
{
	IplImage* src = cvLoadImage("image/ReflectionImage.png", -1);
	CDC* pDC = m_wndSplitter.GetPane(0, 1)->GetDC();
	HDC hdc = pDC->GetSafeHdc();
	CRect rect;
	CvvImage cimg;

	m_wndSplitter.GetPane(0, 1)->GetClientRect(&rect);
	cimg.CopyOf(src, src->nChannels);
	cimg.DrawToHDC(hdc, &rect);
	ReleaseDC(pDC);
	cimg.Destroy();
}

void CMainFrame::OnScattering()
{
	IplImage* src = cvLoadImage("image/ScatteringImage.png", -1);
	CDC* pDC = m_wndSplitter.GetPane(1, 0)->GetDC();
	HDC hdc = pDC->GetSafeHdc();
	CRect rect;
	CvvImage cimg;

	m_wndSplitter.GetPane(1, 0)->GetClientRect(&rect);
	cimg.CopyOf(src, src->nChannels);
	cimg.DrawToHDC(hdc, &rect);
	ReleaseDC(pDC);
	cimg.Destroy();
}