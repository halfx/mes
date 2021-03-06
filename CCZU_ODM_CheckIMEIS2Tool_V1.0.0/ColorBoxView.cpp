// ColorBoxView.cpp : implementation of the CColorBoxView class
//

#include "stdafx.h"
#include "ColorBox.h"
#include "Splash.h"
#include "ColorBoxDoc.h"
#include "ColorBoxView.h"
#include <math.h>

#include "mmsystem.h"//导入声音头文件
#pragma comment(lib,"winmm.lib")//导入声音头文件库

#define IDLE                101
#define FAIL                102
#define PASS                103
#define Testing             104
#define InitFail            105

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorBoxView

IMPLEMENT_DYNCREATE(CColorBoxView, CFormView)

BEGIN_MESSAGE_MAP(CColorBoxView, CFormView)
	//{{AFX_MSG_MAP(CColorBoxView)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_EN_CHANGE(IDC_EDIT_CIMEI1, OnChangeEditCimei1)
	ON_EN_CHANGE(IDC_EDIT_CIMEI2, OnChangeEditCimei2)
	ON_EN_CHANGE(IDC_EDIT_CIMEI3, OnChangeEditCimei3)
	ON_EN_CHANGE(IDC_EDIT_CIMEI4, OnChangeEditCimei4)
	ON_EN_CHANGE(IDC_EDIT_MIMEI1, OnChangeEditMimei1)
	ON_EN_CHANGE(IDC_EDIT_MIMEI2, OnChangeEditMimei2)
	ON_EN_CHANGE(IDC_EDIT_MIMEI3, OnChangeEditMimei3)
	ON_EN_CHANGE(IDC_EDIT_MIMEI4, OnChangeEditMimei4)
	ON_COMMAND(IDM_SCAN_MAIN_IMEI_YES, OnScanMainImeiYes)
	ON_COMMAND(IDM_SCAN_MAIN_IMEI_NO, OnScanMainImeiNo)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATEDATA,OnUpdateData)
	ON_MESSAGE(WM_SETMYFOCUS,OnSetMyFocus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorBoxView construction/destruction

CColorBoxView::CColorBoxView()
	: CFormView(CColorBoxView::IDD)
{
	//{{AFX_DATA_INIT(CColorBoxView)
	m_cscIMEI1 = _T("");
	m_cscIMEI2 = _T("");
	m_cscIMEI3 = _T("");
	m_cscIMEI4 = _T("");
	m_csmIMEI1 = _T("");
	m_csmIMEI2 = _T("");
	m_csmIMEI3 = _T("");
	m_csmIMEI4 = _T("");
	m_csCurrentProduct = _T("");
	m_csCurrentColor = _T("");
	m_csCurrentOrder = _T("");
	m_csCurrentLine = _T("");
	//}}AFX_DATA_INIT
}

CColorBoxView::~CColorBoxView()
{
}

void CColorBoxView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorBoxView)
	DDX_Control(pDX, IDC_STATIC_RESULT, m_cResult);
	DDX_Control(pDX, IDC_STATIC_CURRENT_LINE, m_ctrCurrentLine);
	DDX_Control(pDX, IDC_EDIT_MIMEI4, m_ctrmIMEI4);
	DDX_Control(pDX, IDC_EDIT_MIMEI3, m_ctrmIMEI3);
	DDX_Control(pDX, IDC_EDIT_MIMEI2, m_ctrmIMEI2);
	DDX_Control(pDX, IDC_EDIT_MIMEI1, m_ctrmIMEI1);
	DDX_Control(pDX, IDC_EDIT_CIMEI4, m_ctrcIMEI4);
	DDX_Control(pDX, IDC_EDIT_CIMEI3, m_ctrcIMEI3);
	DDX_Control(pDX, IDC_EDIT_CIMEI2, m_ctrcIMEI2);
	DDX_Control(pDX, IDC_EDIT_CIMEI1, m_ctrcIMEI1);
	DDX_Text(pDX, IDC_EDIT_CIMEI1, m_cscIMEI1);
	DDX_Text(pDX, IDC_EDIT_CIMEI2, m_cscIMEI2);
	DDX_Text(pDX, IDC_EDIT_CIMEI3, m_cscIMEI3);
	DDX_Text(pDX, IDC_EDIT_CIMEI4, m_cscIMEI4);
	DDX_Text(pDX, IDC_EDIT_MIMEI1, m_csmIMEI1);
	DDX_Text(pDX, IDC_EDIT_MIMEI2, m_csmIMEI2);
	DDX_Text(pDX, IDC_EDIT_MIMEI3, m_csmIMEI3);
	DDX_Text(pDX, IDC_EDIT_MIMEI4, m_csmIMEI4);
	DDX_Control(pDX, IDC_STATIC_CURRENT_PRODUCT, m_ctrCurrentProduct);
	DDX_Control(pDX, IDC_STATIC_CURRENT_ORDER, m_ctrCurrentOrder);
	DDX_Control(pDX, IDC_STATIC_CURRENT_COLOR, m_ctrCurrentColor);
	DDX_Control(pDX, IDC_STATIC_CURRENT_COUNT, m_cCurrentCount);
	DDX_Text(pDX, IDC_STATIC_CURRENT_PRODUCT, m_csCurrentProduct);
	DDX_Text(pDX, IDC_STATIC_CURRENT_COLOR, m_csCurrentColor);
	DDX_Text(pDX, IDC_STATIC_CURRENT_ORDER, m_csCurrentOrder);
	DDX_Control(pDX, IDC_LIST_STATE, m_state_list);
	DDX_Text(pDX, IDC_STATIC_CURRENT_LINE, m_csCurrentLine);
	//}}AFX_DATA_MAP
}


BOOL CColorBoxView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CColorBoxView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CString path;
	GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	path.ReleaseBuffer();
	int pos = path.ReverseFind('\\');
	path = path.Left(pos);
	m_csErrorSndPath = path + _T("\\MES_ERROR.wav");
	m_csOkSndPath = path + _T("\\MES_OK.wav");

	this->SetMyFont();

	this->m_Config.InitConfigPath();

	this->m_Config.ReadConfig();
	
	m_csCurrentOrder = ((CColorBoxApp *)AfxGetApp())->InputOrder;
	m_csCurrentLine  = ((CColorBoxApp *)AfxGetApp())->InputProductionLine;

	this->SetTimer(1,500,0);
}

/////////////////////////////////////////////////////////////////////////////
// CColorBoxView diagnostics

#ifdef _DEBUG
void CColorBoxView::AssertValid() const
{
	CFormView::AssertValid();
}

void CColorBoxView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CColorBoxDoc* CColorBoxView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CColorBoxDoc)));
	return (CColorBoxDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CColorBoxView message handlers
void CColorBoxView::SetMyFont()
{
	//===============set imagelist=====================================
	HICON hIcon[3];
	hIcon[0]=AfxGetApp()->LoadIcon(IDI_PASS);
	hIcon[1]=AfxGetApp()->LoadIcon(IDI_ERRORS);
	hIcon[2]=AfxGetApp()->LoadIcon(IDI_STOP);
	if(m_imageList.m_hImageList!=NULL)
		m_imageList.DeleteImageList();
	m_imageList.Create(16,16,ILC_COLOR32,3,3);
	m_imageList.SetBkColor(RGB(255,255,255));
	m_imageList.Add(hIcon[0]);
	m_imageList.Add(hIcon[1]);
	m_imageList.Add(hIcon[2]);	

	//==========init state list=====================================
    DWORD dwStyle;
	LVCOLUMN   Column;	
	
	m_state_list.SetImageList(&m_imageList,LVSIL_SMALL);
	dwStyle = m_state_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	//dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	//dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件
	m_state_list.SetExtendedStyle(dwStyle); //设置扩展风格

	//((CWnd*)GetDlgItem(IDC_LIST_STATE))->SetFont(&m_sysfont);
	Column.mask =   LVCF_TEXT|LVCF_FMT|LVCF_WIDTH;
	Column.cchTextMax =   30;   
	Column.cx =   170;     //column宽度，不工作！   
	Column.fmt =   LVCFMT_LEFT;     //不工作   
	Column.iImage =   0;     
	Column.iOrder =   0;     //不明白干什么用   
	Column.iSubItem =   8;        
	Column.pszText =   "Time";   
	m_state_list.InsertColumn(0,&Column);
	Column.pszText =   "Status"; 
	Column.cx =   280;
	m_state_list.InsertColumn(1,&Column);

	this->m_ctrCurrentProduct.SetFontSize(18);
	this->m_ctrCurrentProduct.SetFontName("Arial");
	this->m_ctrCurrentProduct.SetTextColor(RGB(0,0,255));

	this->m_ctrCurrentOrder.SetFontSize(16);
	this->m_ctrCurrentOrder.SetFontName("bold");
	this->m_ctrCurrentOrder.SetTextColor(RGB(0,0,255));

	this->m_ctrCurrentColor.SetFontSize(16);
	this->m_ctrCurrentColor.SetFontName("bold");
	this->m_ctrCurrentColor.SetTextColor(RGB(0,0,255));
	
	this->m_ctrCurrentLine.SetFontSize(16);
	this->m_ctrCurrentLine.SetFontName("bold");
	this->m_ctrCurrentLine.SetTextColor(RGB(0,0,255));

	this->m_cCurrentCount.SetFontSize(22);
	this->m_cCurrentCount.SetFontName("Arial");
	this->m_cCurrentCount.SetTextColor(RGB(0,0,255));
	
	VERIFY(m_staticfont.CreateFont(
		18,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Arial"));                 // lpszFacename

	((CWnd*)GetDlgItem(IDC_STATIC_IMEI1))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_STATIC_IMEI2))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_STATIC_IMEI3))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_STATIC_IMEI4))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_STATIC_IMEI5))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_STATIC_IMEI6))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_STATIC_IMEI7))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_STATIC_IMEI8))->SetFont(&m_staticfont);

	((CWnd*)GetDlgItem(IDC_EDIT_CIMEI1))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_EDIT_CIMEI2))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_EDIT_CIMEI3))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_EDIT_CIMEI4))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_EDIT_MIMEI1))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_EDIT_MIMEI2))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_EDIT_MIMEI3))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_EDIT_MIMEI4))->SetFont(&m_staticfont);

	this->m_cResult.SetFontSize(45);
	this->m_cResult.SetFontName("Arial");
}


void CColorBoxView::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == 1)
	{
		this->KillTimer(1);
		CFormView::OnTimer(nIDEvent);
		
		this->m_Config.ReadConfig();

		switch (this->m_Config.m_iIsScanMainImei)
		{
		case 0:
			AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_NO, MF_BYCOMMAND | MF_CHECKED );
			break;
		case 1:
			AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_YES, MF_BYCOMMAND | MF_CHECKED );
			break;
		default:
			this->m_Config.m_iIsScanMainImei=0;
			this->m_Config.SaveConfig();
			AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_NO, MF_BYCOMMAND | MF_CHECKED );
			break;
		}

		this->SetUIDisableAll();

		InsertListInfo("The current operation mode：Scan Mode...",0);

		if(!this->GetProductOrderInfo())
		{
			//警告提示音
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
			InsertListInfo("For project order information Fail...",2);
			return;
		}
		else
		{
			UpdateData(FALSE);
			InsertListInfo("For project order information Pass...",0);
		}

		if(!this->Load_Current_Product_Count())
		{
			//警告提示音
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC); 
			InsertListInfo("Load Current Product Count Fail",2);
			return;
		}
		InsertListInfo("Load Current Product Count Pass",0);
		this->UpdateCount(this->m_csCurrentCount);
		
		//根据IMEIType情况设置控件
		switch(this->m_iIMEIType)
		{
		case 0:
			this->m_ctrmIMEI1.EnableWindow(TRUE);
			this->m_ctrcIMEI1.EnableWindow(TRUE);
			break;
		case 1:
			this->m_ctrmIMEI1.EnableWindow(TRUE);
			this->m_ctrmIMEI2.EnableWindow(TRUE);
			this->m_ctrcIMEI1.EnableWindow(TRUE);
			this->m_ctrcIMEI2.EnableWindow(TRUE);
			break;
		case 2:
			this->m_ctrmIMEI1.EnableWindow(TRUE);
			this->m_ctrmIMEI2.EnableWindow(TRUE);
			this->m_ctrmIMEI3.EnableWindow(TRUE);
			this->m_ctrcIMEI1.EnableWindow(TRUE);
			this->m_ctrcIMEI2.EnableWindow(TRUE);
			this->m_ctrcIMEI3.EnableWindow(TRUE);
			break;
		case 3:
			this->m_ctrmIMEI1.EnableWindow(TRUE);
			this->m_ctrmIMEI2.EnableWindow(TRUE);
			this->m_ctrmIMEI3.EnableWindow(TRUE);
			this->m_ctrmIMEI4.EnableWindow(TRUE);
			this->m_ctrcIMEI1.EnableWindow(TRUE);
			this->m_ctrcIMEI2.EnableWindow(TRUE);
			this->m_ctrcIMEI3.EnableWindow(TRUE);
			this->m_ctrcIMEI4.EnableWindow(TRUE);
			break;
		default:
			break;
		}
		
		if(this->m_Config.m_iIsScanMainImei==1)
		{
			this->m_ctrmIMEI1.EnableWindow(TRUE);
			this->m_ctrmIMEI2.EnableWindow(FALSE);
			this->m_ctrmIMEI3.EnableWindow(FALSE);
			this->m_ctrmIMEI4.EnableWindow(FALSE);
			this->m_ctrcIMEI1.EnableWindow(TRUE);
			this->m_ctrcIMEI2.EnableWindow(FALSE);
			this->m_ctrcIMEI3.EnableWindow(FALSE);
			this->m_ctrcIMEI4.EnableWindow(FALSE);
		}
		
		this->m_ctrmIMEI1.SetFocus();
	}
}


void CColorBoxView::InsertListInfo(CString state,int imageIndex)
{
	CString date;
	date=GetCurTimes();
	m_state_list.InsertItem(m_state_list.GetItemCount(),date,imageIndex);
	m_state_list.SetItemText(m_state_list.GetItemCount()-1,1,state);
	m_state_list.EnsureVisible(m_state_list.GetItemCount()-1,TRUE);
}


CString CColorBoxView::GetCurTimes()
{
	CTime t=CTime::GetCurrentTime();
	CString str=t.Format( "%Y-%m-%d  %H:%M:%S");
	return str;
}


void CColorBoxView::SetUIDisableAll()
{
	this->m_ctrcIMEI1.EnableWindow(FALSE);
	this->m_ctrcIMEI2.EnableWindow(FALSE);
	this->m_ctrcIMEI3.EnableWindow(FALSE);
	this->m_ctrcIMEI4.EnableWindow(FALSE);
	this->m_ctrmIMEI1.EnableWindow(FALSE);
	this->m_ctrmIMEI2.EnableWindow(FALSE);
	this->m_ctrmIMEI3.EnableWindow(FALSE);
	this->m_ctrmIMEI4.EnableWindow(FALSE);
}


CString CColorBoxView::VariantToCString(VARIANT   var)   
{   
	CString strValue;   
	_variant_t var_t;   
	_bstr_t bst_t;   
	//time_t cur_time;   
	CTime time_value;   
	COleCurrency var_currency; 
	COleDateTime cur_time;
    
	switch(var.vt)   
	{   
	case   VT_EMPTY:   
		strValue   =   _T("");   
		break;   
	case   VT_UI1:   
		strValue.Format("%d",var.bVal);   
		break;   
	case   VT_I2:   
		strValue.Format("%d",var.iVal);   
		break;   
	case   VT_I4:   
		strValue.Format("%d",var.lVal);   
		break;   
	case   VT_R4:   
		strValue.Format("%f",var.fltVal);   
		break;   
	case   VT_R8:   
		strValue.Format("%f",var.dblVal);   
		break;   
	case   VT_CY:   
		var_currency   =   var;   
		strValue   =   var_currency.Format(0);   
		break;   
	case   VT_BSTR:   
		var_t   =   var;   
		bst_t   =   var_t;   
		strValue.Format   ("%s",(const   char*)bst_t);   
		break;   
	case   VT_NULL:   
		strValue   =   _T("");   
		break;   
	case   VT_DATE:   
		//cur_time   =   (long)var.date;   
		// time_value   =   cur_time;   
		// strValue   =   time_value.Format("%Y-%m-%d");  
		cur_time   =   var; 
		strValue   =   cur_time.Format("%Y-%m-%d"); 
		break;   
	case   VT_BOOL:   
		strValue.Format("%d",var.boolVal   );   
		break;   
	default:     
		strValue   =   _T("");   
		break;   
	}   
	return   strValue;   
}


void CColorBoxView::ClearUI()
{
	switch(this->m_iIMEIType) 
	{
	case 0:
		this->m_cscIMEI1.Empty();
		this->m_csmIMEI1.Empty();
		break;
	case 1:
		this->m_cscIMEI1.Empty();
		this->m_csmIMEI1.Empty();
		this->m_cscIMEI2.Empty();
		this->m_csmIMEI2.Empty();
		break;
	case 2:
		this->m_cscIMEI1.Empty();
		this->m_csmIMEI1.Empty();
		this->m_cscIMEI2.Empty();
		this->m_csmIMEI2.Empty();
		this->m_cscIMEI3.Empty();
		this->m_csmIMEI3.Empty();
		break;
	case 3:
		this->m_cscIMEI1.Empty();
		this->m_csmIMEI1.Empty();
		this->m_cscIMEI2.Empty();
		this->m_csmIMEI2.Empty();
		this->m_cscIMEI3.Empty();
		this->m_csmIMEI3.Empty();
		this->m_cscIMEI4.Empty();
		this->m_csmIMEI4.Empty();
		break;
	default:
		break;
	}

	SendMessage(WM_UPDATEDATA, FALSE, FALSE);
	SendMessage(WM_SETMYFOCUS,(WPARAM)(&this->m_ctrmIMEI1),FALSE);
}


BOOL CColorBoxView::CheckImei1()
{
	//检测长度是否为15位
	if(m_csmIMEI1.GetLength()!=15)
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		InsertListInfo("IMEI1 Length is not 15 Fail...",2);
		this->m_ctrmIMEI1.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csmIMEI1.Left(14)>this->m_imei1_end || m_csmIMEI1.Left(14)<this->m_imei1_start)
		{
			//警告提示音
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
			InsertListInfo("IMEI1 Out of the Range Fail...",2);
			this->m_ctrmIMEI1.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI1='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csmIMEI1);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				InsertListInfo("IMEI1 is not exist in IMEI Table Fail...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	//检测IMEI校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csmIMEI1.Left(14),calculate_digit);

	if(calculate_digit!=m_csmIMEI1.Right(1))
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		InsertListInfo("illegal IMEI1,The 15th Digit of IMEI Error...",2);
		this->m_ctrmIMEI1.SetSel(0,-1);
		return FALSE;
	}
	return TRUE;
}


BOOL CColorBoxView::CheckImei2()
{
	//检测长度是否为15位
	if(m_csmIMEI2.GetLength()!=15)
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		InsertListInfo("IMEI2 Length is not 15 Fail...",2);
		this->m_ctrmIMEI2.SetSel(0,-1);
		return FALSE;
	}
	
	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csmIMEI2.Left(14)>this->m_imei2_end || m_csmIMEI2.Left(14)<this->m_imei2_start)
		{
			//警告提示音
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
			InsertListInfo("IMEI2 Out of the Range Fail...",2);
			this->m_ctrmIMEI2.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI2='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csmIMEI2);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				InsertListInfo("IMEI2 is not exist in IMEI Table Fail...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	//检测IMEI校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csmIMEI2.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csmIMEI2.Right(1))	
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		InsertListInfo("illegal IMEI2,The 15th Digit of IMEI Error...",2);
		this->m_ctrmIMEI2.SetSel(0,-1);
		return FALSE;
	}
	return TRUE;
}


BOOL CColorBoxView::CheckImei3()
{
	//检测长度是否为15位
	if(m_csmIMEI3.GetLength()!=15)
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		InsertListInfo("IMEI3 Length is not 15 Fail...",2);
		this->m_ctrmIMEI3.SetSel(0,-1);
		return FALSE;
	}
	
	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csmIMEI3.Left(14)>this->m_imei3_end || m_csmIMEI3.Left(14)<this->m_imei3_start)
		{
			//警告提示音
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
			InsertListInfo("IMEI3 Out of the Range Fail...",2);
			this->m_ctrmIMEI3.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI3='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csmIMEI3);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				InsertListInfo("IMEI3 is not exist in IMEI Table Fail...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	//检测IMEI校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csmIMEI3.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csmIMEI3.Right(1))	
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		InsertListInfo("illegal IMEI3,The 15th Digit of IMEI Error...",2);
		this->m_ctrmIMEI3.SetSel(0,-1);
		return FALSE;
	}
	return TRUE;
}


BOOL CColorBoxView::CheckImei4()
{
	//检测长度是否为15位
	if(m_csmIMEI4.GetLength()!=15)
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		InsertListInfo("IMEI4 Length is not 15 Fail...",2);
		this->m_ctrmIMEI4.SetSel(0,-1);
		return FALSE;
	}
	
	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csmIMEI4.Left(14)>this->m_imei4_end || m_csmIMEI4.Left(14)<this->m_imei4_start)
		{
			//警告提示音
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
			InsertListInfo("IMEI4 Out of the Range Fail...",2);
			this->m_ctrmIMEI4.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI4='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csmIMEI4);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				InsertListInfo("IMEI4 is not exist in IMEI Table Fail...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	//检测IMEI校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csmIMEI4.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csmIMEI4.Right(1))	
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		InsertListInfo("illegal IMEI4,The 15th Digit of IMEI Error...",2);
		this->m_ctrmIMEI4.SetSel(0,-1);
		return FALSE;
	}
	return TRUE;
}


void CColorBoxView::calculate_15_digit_imei( char *imei_14,char &calculate_digit)
{
	int check_digit,sum1,sum2,total_sum,temp_value,temp_value_2_part;
	int i;
	char temp[16];
	
	
	// the following is calculate IMEI 15th digit -- check digit //
	// according   3G TS 22.016  V3.1.0 //
	/* 
	A.2 Computation of Check Digit for an IMEI
	Computation of CD from the IMEI proceeds as follows:
	Step 1: Double the values of the odd labelled digits D1, D3, D5 ... D13 of the IMEI.
	Step 2: Add together the individual digits of all the seven numbers obtained in Step 1, and then add this sum to
	the sum of all the even labelled digits D2, D4, D6 ... D14 of the IMEI.
	Step 3: If the number obtained in Step 2 ends in 0, then set CD to be 0. If the number obtained in Step 2 does not
	end in 0, then set CD to be that number subtracted from the next higher number which does end in 0.
	
	  
		A.3 Example of computation
		IMEI (14 most significant digits):
		TAC FAC SNR
		D14  D13 D12 D11 D10 D9 D8 D7  D6 D5 D4 D3 D2 D1
		2    6   0   5   3   1  7  9  3   1  1  3  8  3
		Step 1:
		2    6   0   5   3   1  7  9  3   1  1  3  8  3 
		x2      x2      x2    x2     x2    x2   x2
		12      10      2     18     2      6    6
		Step 2:
		2 + 1 + 2 + 0 + 1 + 0 + 3 + 2 + 7 + 1 + 8 + 3 + 2 + 1 + 6 + 8 + 6 = 53
		Step 3:
		CD = 60 - 53 = 7
	*/
	for(i=0;i<14;i++)
		temp[i] = imei_14[i];
	
	sum1 = 0;
	for (i=1;i<=13;i++)		  // sum digit 1,3,5,7,9,11,13
	{
		sum1 = sum1 + (temp[i-1]-48);
		i++;
	}
	sum2 = 0;
	for (i=2;i<=14;i++)			 // sum digit 2,4,6,8,10,12,14   
	{
		temp_value =  (temp[i-1]-48) * 2 ;			// temp_value range from 0 --> 9*2 = 18
		if ( temp_value < 10 )  
			sum2 = sum2 +  temp_value ;
		else  
		{
			temp_value_2_part = temp_value - 10 ;
			sum2 = sum2 + 1 +temp_value_2_part;
		} 
		
		i++;				  
	}
	
	total_sum = sum1 + sum2 ;
	
	if ( fmod(total_sum,10) == 0 )
		check_digit = 0  ; 	   // if total_sum ends in 0 ( mod 10 reminder is 0 ) 
	else 
	{
		check_digit = (total_sum /10)*10 + 10 - total_sum;
		
	}
	calculate_digit=check_digit+48;
	
}


int CColorBoxView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CSplashWnd::ShowSplashScreen(this);
	
	return 0;
}


void CColorBoxView::OnUpdateData(WPARAM wParam, LPARAM lParam) 
{ 
	UpdateData(wParam); 
}


void CColorBoxView::OnSetMyFocus(WPARAM wParam, LPARAM lParam)
{
	CEdit* a;
	a=(CEdit*)wParam;
	a->SetFocus();
}


bool CColorBoxView::GetProductOrderInfo()
{
	CString temp;
	_variant_t var;
	CString sql;

	try
	{
		sql.Format("select * from ProductList where Enable=1 and OrderName='%s'",m_csCurrentOrder);
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
	
		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("ProductName");
			temp=VariantToCString(var);
			this->m_csCurrentProduct= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEIType");
			this->m_iIMEIType=var.intVal;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEIRangeType");
			this->m_iIMEIRangeType=var.intVal;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI1_STR");
			temp=VariantToCString(var);
			this->m_imei1_start= temp; 

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI1_END");
			temp=VariantToCString(var);
			this->m_imei1_end= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI2_STR");
			temp=VariantToCString(var);
			this->m_imei2_start= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI2_END");
			temp=VariantToCString(var);
			this->m_imei2_end= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI3_STR");
			temp=VariantToCString(var);
			this->m_imei3_start= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI3_END");
			temp=VariantToCString(var);
			this->m_imei3_end= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI4_STR");
			temp=VariantToCString(var);
			this->m_imei4_start= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI4_END");
			temp=VariantToCString(var);
			this->m_imei4_end= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("Color");
			temp=VariantToCString(var);
			this->m_csCurrentColor= temp;
		}
		else
		{
			return FALSE;
		}
		
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		return FALSE;
	}

	return TRUE;
}


bool CColorBoxView::Load_Current_Product_Count() 
{
	CString temp,states;
	_variant_t var;
	CString sql;
	
	sql.Format("select count(*) as COUNT from %s_%s_ColorBox where Status=3 and Enable=1",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder);
	
	try
	{
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{			
			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("COUNT");
			temp=VariantToCString(var);
			this->m_csCurrentCount= temp;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State) 
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		return FALSE;
	}

	return TRUE;
}


void CColorBoxView::UpdateCount(CString strCount)
{
	CString temp;
	temp="Count:"+strCount; 
	this->m_cCurrentCount.SetWindowText(temp);
	UpdateData(FALSE);
}
 

void CColorBoxView::OnChangeEditMimei1() 
{
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);
	
	switch(this->m_iIMEIType)
	{
	case 0:
		if(m_csmIMEI1.Right(2)=="\r\n")
		{
			m_csmIMEI1.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(!CheckImei1())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				InsertListInfo("Check Module IMEI Fail...",2);
				ShowCalResult(FAIL);
				m_ctrmIMEI1.SetFocus();
				m_ctrmIMEI1.SetSel(0,-1);
				return;
			}
			InsertListInfo("Check Module IMEI Pass...",0);
			m_ctrcIMEI1.SetFocus();
			m_ctrcIMEI1.SetSel(0,-1);
		}
		break;
	case 1:
		if(this->m_Config.m_iIsScanMainImei==0)
		{
			if(m_csmIMEI1.Right(2)=="\r\n")
			{
				m_csmIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(!CheckImei1())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					InsertListInfo("Check Module IMEI Fail...",2);
					ShowCalResult(FAIL);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					return;
				}
				InsertListInfo("Check Module IMEI Pass...",0);
				m_ctrmIMEI2.SetFocus();
				m_ctrmIMEI2.SetSel(0,-1);
			}
		}
		else
		{
			if(m_csmIMEI1.Right(2)=="\r\n")
			{
				m_csmIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(!CheckImei1())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					InsertListInfo("Check Module IMEI Fail...",2);
					ShowCalResult(FAIL);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					return;
				}
				InsertListInfo("Check Module IMEI Pass...",0);
				m_ctrcIMEI1.SetFocus();
				m_ctrcIMEI1.SetSel(0,-1);
			}
		}
		break;
	case 2:
		if(this->m_Config.m_iIsScanMainImei==0)
		{
			if(m_csmIMEI1.Right(2)=="\r\n")
			{
				m_csmIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(!CheckImei1())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					InsertListInfo("Check Module IMEI Fail...",2);
					ShowCalResult(FAIL);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					return;
				}
				InsertListInfo("Check Module IMEI Pass...",0);
				m_ctrmIMEI2.SetFocus();
				m_ctrmIMEI2.SetSel(0,-1);
			}
		}
		else
		{
			if(m_csmIMEI1.Right(2)=="\r\n")
			{
				m_csmIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(!CheckImei1())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					InsertListInfo("Check Module IMEI Fail...",2);
					ShowCalResult(FAIL);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					return;
				}
				InsertListInfo("Check Module IMEI Pass...",0);
				m_ctrcIMEI1.SetFocus();
				m_ctrcIMEI1.SetSel(0,-1);
			}			
		}
		break;
	case 3:
		if(this->m_Config.m_iIsScanMainImei==0)
		{
			if(m_csmIMEI1.Right(2)=="\r\n")
			{
				m_csmIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(!CheckImei1())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					InsertListInfo("Check Module IMEI Fail...",2);
					ShowCalResult(FAIL);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					return;
				}
				InsertListInfo("Check Module IMEI Pass...",0);
				m_ctrmIMEI2.SetFocus();
				m_ctrmIMEI2.SetSel(0,-1);
			}
		}
		else
		{
			if(m_csmIMEI1.Right(2)=="\r\n")
			{
				m_csmIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(!CheckImei1())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					InsertListInfo("Check Module IMEI Fail...",2);
					ShowCalResult(FAIL);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					return;
				}
				InsertListInfo("Check Module IMEI Pass...",0);
				m_ctrcIMEI1.SetFocus();
				m_ctrcIMEI1.SetSel(0,-1);
			}
		}
		break;
	default:
		break;
	}		
}


void CColorBoxView::OnChangeEditMimei2() 
{
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);
	
	switch(this->m_iIMEIType)
	{
	case 1:	
		if(m_csmIMEI2.Right(2)=="\r\n")
		{
			m_csmIMEI2.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(!CheckImei2())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				InsertListInfo("Check Module IMEI Fail...",2);
				ShowCalResult(FAIL);
				m_ctrmIMEI2.SetFocus();
				m_ctrmIMEI2.SetSel(0,-1);
				return;
			}
			InsertListInfo("Check Module IMEI Pass...",0);
			m_ctrcIMEI1.SetFocus();
			m_ctrcIMEI1.SetSel(0,-1);
		}
		break;
	case 2:
		if(m_csmIMEI2.Right(2)=="\r\n")
		{
			m_csmIMEI2.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(!CheckImei2())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				InsertListInfo("Check Module IMEI Fail...",2);
				ShowCalResult(FAIL);
				m_ctrmIMEI2.SetFocus();
				m_ctrmIMEI2.SetSel(0,-1);
				return;
			}
			InsertListInfo("Check Module IMEI Pass...",0);
			m_ctrmIMEI3.SetFocus();
			m_ctrmIMEI3.SetSel(0,-1);
		}
		break;
	case 3:
		if(m_csmIMEI2.Right(2)=="\r\n")
		{
			m_csmIMEI2.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(!CheckImei2())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				InsertListInfo("Check Module IMEI Fail...",2);
				ShowCalResult(FAIL);
				m_ctrmIMEI2.SetFocus();
				m_ctrmIMEI2.SetSel(0,-1);
				return;
			}
			InsertListInfo("Check Module IMEI Pass...",0);
			m_ctrmIMEI3.SetFocus();
			m_ctrmIMEI3.SetSel(0,-1);
		}
		break;
	default:
		break;
	}
}


void CColorBoxView::OnChangeEditMimei3()
{
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);
	
	switch(this->m_iIMEIType)
	{
	case 2:
		if(m_csmIMEI3.Right(2)=="\r\n")
		{
			m_csmIMEI3.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(!CheckImei3())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				InsertListInfo("Check Module IMEI Fail...",2);
				ShowCalResult(FAIL);
				m_ctrmIMEI3.SetFocus();
				m_ctrmIMEI3.SetSel(0,-1);
				return;
			}
			InsertListInfo("Check Module IMEI Pass...",0);
			m_ctrcIMEI1.SetFocus();
			m_ctrcIMEI1.SetSel(0,-1);
		}
		break;
	case 3:
		if(m_csmIMEI3.Right(2)=="\r\n")
		{
			m_csmIMEI3.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(!CheckImei3())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				InsertListInfo("Check Module IMEI Fail...",2);
				ShowCalResult(FAIL);
				m_ctrmIMEI3.SetFocus();
				m_ctrmIMEI3.SetSel(0,-1);
				return;
			}
			InsertListInfo("Check Module IMEI Pass...",0);
			m_ctrmIMEI4.SetFocus();
			m_ctrmIMEI4.SetSel(0,-1);
		}
		break;
	default:
		break;
	}
}


void CColorBoxView::OnChangeEditMimei4() 
{
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);
	
	switch(this->m_iIMEIType)
	{
	case 3:
		if(m_csmIMEI4.Right(2)=="\r\n")
		{
			m_csmIMEI4.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(!CheckImei4())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				InsertListInfo("Check Module IMEI Fail...",2);
				ShowCalResult(FAIL);
				m_ctrmIMEI4.SetFocus();
				m_ctrmIMEI4.SetSel(0,-1);
				return;
			}
			InsertListInfo("Check Module IMEI Pass...",0);
			m_ctrcIMEI1.SetFocus();
			m_ctrcIMEI1.SetSel(0,-1);
		}
		break;
	default:
		break;
	}
}


void CColorBoxView::OnChangeEditCimei1() 
{
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);
	
	switch(this->m_iIMEIType)
	{
	case 0:	
		if(m_cscIMEI1.Right(2)=="\r\n")
		{
			m_cscIMEI1.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(m_csmIMEI1 != m_cscIMEI1)
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				m_ctrmIMEI1.SetFocus();
				m_ctrmIMEI1.SetSel(0,-1);
				InsertListInfo("Compare IMEI Fail...",2);
				ShowCalResult(FAIL);
				return;
			}
			else //进行流程检查
			{
				if(!CheckColorPrintStatus_Mode1())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					InsertListInfo("Position detection Error,Please Check Whether The GiftBox Print Station Has Printed Fail...",2);
					ShowCalResult(FAIL);
					return;
				}
				else
				{
					if(!SaveToColorInfo())
					{
						//警告提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
						m_ctrmIMEI1.SetFocus();
						m_ctrmIMEI1.SetSel(0,-1);
						InsertListInfo("Save Status Value To ColorInfo Fail...",2);
						ShowCalResult(FAIL);
						return;
					}
					InsertListInfo("Save Status Value To ColorInfo Pass...",0);
					
					if(!this->Load_Current_Product_Count())
					{
						//警告提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
						InsertListInfo("Load Current Product Count Fail...",2);
						ShowCalResult(FAIL);
						return;
					}
					InsertListInfo("Load Current Product Count Pass...",0);
					ClearUI();
					this->UpdateCount(this->m_csCurrentCount);
					ShowCalResult(PASS);
					///成功提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csOkSndPath, NULL, SND_FILENAME | SND_ASYNC);
					
				}
			}
		}
		break;
	case 1:
		if(this->m_Config.m_iIsScanMainImei==0)
		{
			if(m_cscIMEI1.Right(2)=="\r\n")
			{
				m_cscIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(m_csmIMEI1 != m_cscIMEI1)
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					InsertListInfo("Compare IMEI Fail...",2);
					ShowCalResult(FAIL);
					return;
				}
				this->m_ctrcIMEI2.SetFocus();
				this->m_ctrcIMEI2.SetSel(0,-1);
			}
		}
		else
		{
			if(m_cscIMEI1.Right(2)=="\r\n")
			{
				m_cscIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(m_csmIMEI1 != m_cscIMEI1)
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					InsertListInfo("Compare IMEI Fail...",2);
					ShowCalResult(FAIL);
					return;
				}
				else //进行流程检查
				{
					if(!CheckColorPrintStatus_Mode1())
					{
						//警告提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
						m_ctrmIMEI1.SetFocus();
						m_ctrmIMEI1.SetSel(0,-1);
						InsertListInfo("Position detection Error,Please Check Whether The GiftBox Print Station Has Printed Fail...",2);
						ShowCalResult(FAIL);
						return;
					}
					else
					{					
						if(!SaveToColorInfo())
						{
							//警告提示音
							PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
							m_ctrmIMEI1.SetFocus();
							m_ctrmIMEI1.SetSel(0,-1);
							InsertListInfo("Save Status Value To ColorInfo Fail...",2);
							ShowCalResult(FAIL);
							return;
						}
						InsertListInfo("Save Status Value To ColorInfo Pass...",0);
						
						if(!this->Load_Current_Product_Count())
						{
							//警告提示音
							PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
							InsertListInfo("Load Current Product Count Fail...",2);
							return;
						}

						//成功提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csOkSndPath, NULL, SND_FILENAME | SND_ASYNC);
						InsertListInfo("Load Current Product Count Pass...",0);
						ClearUI();
						this->UpdateCount(this->m_csCurrentCount);
						ShowCalResult(PASS);
							
					}
				}
			}
		}
		break;
	case 2:
		if(this->m_Config.m_iIsScanMainImei==0)
		{
			if(m_cscIMEI1.Right(2)=="\r\n")
			{
				m_cscIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(m_csmIMEI1 != m_cscIMEI1)
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					InsertListInfo("Compare IMEI Fail...",2);
					ShowCalResult(FAIL);
					return;
				}
				this->m_ctrcIMEI2.SetFocus();
				this->m_ctrcIMEI2.SetSel(0,-1);
			}
		}
		else
		{
			if(m_cscIMEI1.Right(2)=="\r\n")
			{
				m_cscIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(m_csmIMEI1 != m_cscIMEI1)
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					InsertListInfo("Compare IMEI Fail...",2);
					ShowCalResult(FAIL);
					return;
				}
				else //进行流程检查
				{
					if(!CheckColorPrintStatus_Mode1())
					{
						//警告提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
						m_ctrmIMEI1.SetFocus();
						m_ctrmIMEI1.SetSel(0,-1);
						InsertListInfo("Position detection Error,Please Check Whether The GiftBox Print Station Has Printed Fail...",2);
						ShowCalResult(FAIL);
						return;
					}
					else
					{					
						if(!SaveToColorInfo())
						{
							//警告提示音
							PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
							m_ctrmIMEI1.SetFocus();
							m_ctrmIMEI1.SetSel(0,-1);
							InsertListInfo("Save Status Value To ColorInfo Fail...",2);
							ShowCalResult(FAIL);
							return;
						}
						InsertListInfo("Save Status Value To ColorInfo Pass...",0);
						
						if(!this->Load_Current_Product_Count())
						{
							//警告提示音
							PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
							InsertListInfo("Load Current Product Count Fail...",2);
							return;
						}
						InsertListInfo("Load Current Product Count Pass...",0);
						ClearUI();
						this->UpdateCount(this->m_csCurrentCount);
						ShowCalResult(PASS);

						//成功提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csOkSndPath, NULL, SND_FILENAME | SND_ASYNC);
						
					}
				}
			}
		}
		break;
	case 3:
		if(this->m_Config.m_iIsScanMainImei==0)
		{
			if(m_cscIMEI1.Right(2)=="\r\n")
			{
				m_cscIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(m_csmIMEI1 != m_cscIMEI1)
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					InsertListInfo("Compare IMEI Fail...",2);
					ShowCalResult(FAIL);
					return;
				}
				this->m_ctrcIMEI2.SetFocus();
				this->m_ctrcIMEI2.SetSel(0,-1);
			}
		}
		else
		{
			if(m_cscIMEI1.Right(2)=="\r\n")
			{
				m_cscIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(m_csmIMEI1 != m_cscIMEI1)
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					InsertListInfo("Compare IMEI Fail...",2);
					ShowCalResult(FAIL);
					return;
				}
				else //进行流程检查
				{
					if(!CheckColorPrintStatus_Mode1())
					{
						//警告提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
						m_ctrmIMEI1.SetFocus();
						m_ctrmIMEI1.SetSel(0,-1);
						InsertListInfo("Position detection Error,Please Check Whether The GiftBox Print Station Has Printed Fail...",2);
						ShowCalResult(FAIL);
						return;
					}
					else
					{
						if(!SaveToColorInfo())
						{
							//警告提示音
							PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
							m_ctrmIMEI1.SetFocus();
							m_ctrmIMEI1.SetSel(0,-1);
							InsertListInfo("Save Status Value To ColorInfo Fail...",2);
							ShowCalResult(FAIL);
							return;
						}
						InsertListInfo("Save Status Value To ColorInfo Pass...",0);
						
						if(!this->Load_Current_Product_Count())
						{
							//警告提示音
							PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
							InsertListInfo("Load Current Product Count Fail...",2);
							return;
						}
						InsertListInfo("Load Current Product Count Pass...",0);
						ClearUI();
						this->UpdateCount(this->m_csCurrentCount);
						ShowCalResult(PASS);

						//成功提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csOkSndPath, NULL, SND_FILENAME | SND_ASYNC);
						
					}
				}
			}
		}
		break;
	default:
		break;
	}	
}


void CColorBoxView::OnChangeEditCimei2() 
{
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);
	
	switch(this->m_iIMEIType)
	{
	case 1:	
		if(m_cscIMEI2.Right(2)=="\r\n")
		{
			m_cscIMEI2.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(m_csmIMEI2 != m_cscIMEI2)
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				m_ctrmIMEI1.SetFocus();
				m_ctrmIMEI1.SetSel(0,-1);
				InsertListInfo("Compare IMEI Fail...",2);
				ShowCalResult(FAIL);
				return;
			}
			else //进行流程检查
			{
				if(!CheckColorPrintStatus_Mode2())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					InsertListInfo("Please check the database for failure, whether done color printing...",2);
					ShowCalResult(FAIL);
					return;
				}
				else
				{
					if(!SaveToColorInfo())
					{
						//警告提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
						m_ctrmIMEI1.SetFocus();
						m_ctrmIMEI1.SetSel(0,-1);
						InsertListInfo("Update database against failure...",2);
						ShowCalResult(FAIL);
						return;
					}
					InsertListInfo("Update database against Pass......",0);
					
					if(!this->Load_Current_Product_Count())
					{
						//警告提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
						InsertListInfo("Gets the number of current orders Fail",2);
						ShowCalResult(FAIL);
						return;
					}
					InsertListInfo("Gets the number of current orders Pass",0);
					ClearUI();
					this->UpdateCount(this->m_csCurrentCount);
					ShowCalResult(PASS);

					//成功提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csOkSndPath, NULL, SND_FILENAME | SND_ASYNC);
					
				}
			}
		}
		break;
	case 2:
		if(m_cscIMEI2.Right(2)=="\r\n")
		{
			m_cscIMEI2.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(m_csmIMEI2 != m_cscIMEI2)
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				m_ctrmIMEI2.SetFocus();
				m_ctrmIMEI2.SetSel(0,-1);
				InsertListInfo("Compare IMEI Fail...",2);
				ShowCalResult(FAIL);
				return;
			}
			this->m_ctrcIMEI3.SetFocus();
			this->m_ctrcIMEI3.SetSel(0,-1);
		}
		break;
	case 3:
		if(m_cscIMEI2.Right(2)=="\r\n")
		{
			m_cscIMEI2.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(m_csmIMEI2 != m_cscIMEI2)
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				m_ctrmIMEI2.SetFocus();
				m_ctrmIMEI2.SetSel(0,-1);
				InsertListInfo("Compare IMEI Fail...",2);
				ShowCalResult(FAIL);
				return;
			}
			this->m_ctrcIMEI3.SetFocus();
			this->m_ctrcIMEI3.SetSel(0,-1);
		}
		break;
	default:
		break;
	}
}


void CColorBoxView::OnChangeEditCimei3() 
{
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);
	
	switch(this->m_iIMEIType)
	{
	case 2:	
		if(m_cscIMEI3.Right(2)=="\r\n")
		{
			m_cscIMEI3.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(m_csmIMEI3 != m_cscIMEI3)
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				m_ctrmIMEI3.SetFocus();
				m_ctrmIMEI3.SetSel(0,-1);
				InsertListInfo("Compare IMEI Fail...",2);
				ShowCalResult(FAIL);
				return;
			}
			else //进行流程检查
			{
				if(!CheckColorPrintStatus_Mode3())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					InsertListInfo("Please check the database for failure, whether done color printing...",2);
					ShowCalResult(FAIL);
					return;
				}
				else
				{				
					if(!SaveToColorInfo())
					{
						//警告提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
						m_ctrmIMEI1.SetFocus();
						m_ctrmIMEI1.SetSel(0,-1);
						InsertListInfo("Update database against failure...",2);
						ShowCalResult(FAIL);
						return;
					}
					InsertListInfo("Update database against Pass......",0);
					
					if(!this->Load_Current_Product_Count())
					{
						//警告提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
						InsertListInfo("Gets the number of current orders Fail",2);
						ShowCalResult(FAIL);
						return;
					}
					InsertListInfo("Gets the number of current orders Pass",0);
					ClearUI();
					this->UpdateCount(this->m_csCurrentCount);
					ShowCalResult(PASS);

					//成功提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csOkSndPath, NULL, SND_FILENAME | SND_ASYNC);
				
				}
			}
		}
		break;
	case 3:
		if(m_cscIMEI3.Right(2)=="\r\n")
		{
			m_cscIMEI3.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(m_csmIMEI3 != m_cscIMEI3)
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				m_ctrmIMEI3.SetFocus();
				m_ctrmIMEI3.SetSel(0,-1);
				InsertListInfo("Compare IMEI Fail...",2);
				ShowCalResult(FAIL);
				return;
			}
			this->m_ctrcIMEI4.SetFocus();
			this->m_ctrcIMEI4.SetSel(0,-1);
		}
		break;
	default:
		break;
	}
}


void CColorBoxView::OnChangeEditCimei4() 
{
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);
	
	switch(this->m_iIMEIType)
	{
	case 3:
		if(m_cscIMEI4.Right(2)=="\r\n")
		{
			m_cscIMEI4.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(m_csmIMEI4 != m_cscIMEI4)
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
				m_ctrmIMEI4.SetFocus();
				m_ctrmIMEI4.SetSel(0,-1);
				InsertListInfo("Compare IMEI Fail...",2);
				ShowCalResult(FAIL);
				return;
			}
			else //进行流程检查
			{
				if(!CheckColorPrintStatus_Mode4())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
					m_ctrmIMEI1.SetFocus();
					m_ctrmIMEI1.SetSel(0,-1);
					InsertListInfo("Please check the database for failure, whether done color printing...",2);
					ShowCalResult(FAIL);
					return;
				}
				else
				{
					if(!SaveToColorInfo())
					{
						//警告提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
						m_ctrmIMEI1.SetFocus();
						m_ctrmIMEI1.SetSel(0,-1);
						InsertListInfo("Update database against failure...",2);
						ShowCalResult(FAIL);
						return;
					}
					InsertListInfo("Update database against Pass...",0);
					
					if(!this->Load_Current_Product_Count())
					{
						//警告提示音
						PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
						InsertListInfo("Gets the number of current orders Fail",2);
						ShowCalResult(FAIL);
						return;
					}
					InsertListInfo("Gets the number of current orders Pass",0);
					ClearUI();
					this->UpdateCount(this->m_csCurrentCount);
					ShowCalResult(PASS);

					//成功提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csOkSndPath, NULL, SND_FILENAME | SND_ASYNC);
					
				}
			}
		}
		break;
	}
}

//单IMEI
bool CColorBoxView::CheckColorPrintStatus_Mode1()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;

	try
	{
		sql.Format("select * from %s_%s_ColorBox where (IMEI1='%s') and (Status=2 or Status=3) and Enable=1",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csmIMEI1);
//		AfxMessageBox(sql);
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			m_checkColorDone=TRUE;
		}
		else//彩盒数据库内无记录
		{
			m_checkColorDone=FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown Error...",2);
		return FALSE;
	}	

	if(m_checkColorDone)
		return TRUE;
	else
		return FALSE;
}


//双IMEI
bool CColorBoxView::CheckColorPrintStatus_Mode2()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;

	try
	{
		sql.Format("select * from %s_%s_ColorBox where (IMEI1='%s') and (IMEI2='%s') and (Status=2 or Status=3) and Enable=1",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csmIMEI1,(char*)(LPCTSTR)m_csmIMEI2);
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			m_checkColorDone=TRUE;
		}
		else//彩盒数据库内无记录
		{
			m_checkColorDone=FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown Error...",2);
		return FALSE;
	}	
	
	if(m_checkColorDone)
		return TRUE;
	else
		return FALSE;
}


//三IMEI
bool CColorBoxView::CheckColorPrintStatus_Mode3()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;

	try
	{
		sql.Format("select * from %s_%s_ColorBox where (IMEI1='%s') and (IMEI2='%s') and (IMEI3='%s') and (Status=2 or Status=3) and Enable=1",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csmIMEI1,(char*)(LPCTSTR)m_csmIMEI2,(char*)(LPCTSTR)m_csmIMEI3);
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			m_checkColorDone=TRUE;
		}
		else//彩盒数据库内无记录
		{
			m_checkColorDone=FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown Error...",2);
		return FALSE;
	}	
	
	if(m_checkColorDone)
		return TRUE;
	else
		return FALSE;
}


//四IMEI
bool CColorBoxView::CheckColorPrintStatus_Mode4()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;

	try
	{
		sql.Format("select * from %s_%s_ColorBox where (IMEI1='%s') and (IMEI2='%s') and (IMEI3='%s') and (IMEI4='%s') and (Status=2 or Status=3) and Enable=1",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csmIMEI1,(char*)(LPCTSTR)m_csmIMEI2,(char*)(LPCTSTR)m_csmIMEI3,(char*)(LPCTSTR)m_csmIMEI4);
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			m_checkColorDone=TRUE;
		}
		else//彩盒数据库内无记录
		{
			m_checkColorDone=FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown Error...",2);
		return FALSE;
	}	
	
	if(m_checkColorDone)
		return TRUE;
	else
		return FALSE;
}


BOOL CColorBoxView::SaveToColorInfo()
{
	CString temp;
	_variant_t var;
	CString sql;

	try
	{
		sql.Format("UPDATE %s_%s_ColorBox SET Status=3 where IMEI1='%s' and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csmIMEI1);
		((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

	}
	catch(_com_error e)
	{
		InsertListInfo("Save Record to GiftBox Server Fail...",2);
		return FALSE;
	}

	sql.Format("UPDATE %s_%s_ProductionLine SET CheckIMEIS2='%s',CheckS2Date=getdate() where IMEI1='%s'", \
		(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder, \
		this->m_csCurrentLine,this->m_csmIMEI1);
	
	try
	{
		((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
	}
	catch(_com_error e)
	{
		return FALSE;
	}
	
	return TRUE;
}


void CColorBoxView::OnScanMainImeiYes()
{
	CPassword dlg;
	dlg.ilevel = 1; //需超级权限验证
	if(dlg.DoModal()!=IDOK)
		return;
	
	AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_NO, MF_BYCOMMAND | MF_UNCHECKED );
	AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_YES, MF_BYCOMMAND | MF_CHECKED );
	this->m_Config.m_iIsScanMainImei=1;
	this->m_Config.SaveConfig();
}


void CColorBoxView::OnScanMainImeiNo() 
{
	CPassword dlg;
	dlg.ilevel = 1; //需超级权限验证
	if(dlg.DoModal()!=IDOK)
		return;
	
	AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_NO, MF_BYCOMMAND | MF_CHECKED );
	AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_YES, MF_BYCOMMAND | MF_UNCHECKED );
	this->m_Config.m_iIsScanMainImei=0;
	this->m_Config.SaveConfig();
}


void CColorBoxView::ShowCalResult(int state)
{
	switch(state)
	{
	case IDLE:
		this->m_cResult.SetTextColor(RGB(0,128,0));
		this->m_cResult.SetWindowText("IDLE");
		break;
	case FAIL:
		this->m_cResult.SetTextColor(RGB(255,0,0));
		this->m_cResult.SetWindowText("FAIL");
		break;
	case PASS:
		this->m_cResult.SetTextColor(RGB(0,128,0));
		this->m_cResult.SetWindowText("PASS");
		break;
	case Testing:
		this->m_cResult.SetTextColor(RGB(0,0,255));
		this->m_cResult.SetWindowText("Testing");
		break;
	case InitFail:
		this->m_cResult.SetTextColor(RGB(255,0,0));
		this->m_cResult.SetWindowText("InitFail");
	default:
		break;
	}
}
