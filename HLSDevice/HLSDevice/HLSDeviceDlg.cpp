
// HLSDeviceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HLSDevice.h"
#include "HLSDeviceDlg.h"
#include "afxdialogex.h"
#include "Sizescale.h"

#include "ScheduleServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//for AAC
#ifndef INP_BUFFER_SIZE
#define INP_BUFFER_SIZE 960//2048
#endif

using namespace ScheduleServer;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CHLSDeviceDlg dialog
CHLSDeviceDlg::CHLSDeviceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHLSDeviceDlg::IDD, pParent)
	, _camera_ctrl(NULL)
	, _yuv_buf(NULL)
	, _yuv_scale(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//AudioSampler
	m_pRecodeSound = NULL;
}

CHLSDeviceDlg::~CHLSDeviceDlg()
{
}

void CHLSDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_pic);
	DDX_Control(pDX, IDC_BUTTON_RTMP_START, m_rtmp_start_btn);
	DDX_Control(pDX, IDC_BUTTON_RTMP_STOP, m_rtmp_stop_btn);
	//DDX_Control(pDX, IDC_BUTTON_ENCODE_START, m_encode_start);
	//DDX_Control(pDX, IDC_BUTTON_ENCODE_STOP, m_encode_stop);
	DDX_Control(pDX, IDC_COMBO1, m_list);
	DDX_Control(pDX, IDC_COMBO2, m_param_lst);
	DDX_Control(pDX, IDC_BUTTON2, m_video_capture_btn);
	DDX_Control(pDX, IDC_BUTTON3, m_video_stop_btn);
}

BEGIN_MESSAGE_MAP(CHLSDeviceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CHLSDeviceDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CHLSDeviceDlg::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_RTMP_START, &CHLSDeviceDlg::OnBnClickedButtonRTMPStart)
	ON_BN_CLICKED(IDC_BUTTON_RTMP_STOP, &CHLSDeviceDlg::OnBnClickedButtonRTMPStop)
	

	ON_BN_CLICKED(IDC_BUTTON1, &CHLSDeviceDlg::OnBnClickedButtonDetect)
	ON_BN_CLICKED(IDC_BUTTON2, &CHLSDeviceDlg::OnBnClickedButtonCapture)
	ON_BN_CLICKED(IDC_BUTTON3, &CHLSDeviceDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON4, &CHLSDeviceDlg::OnBnClickedButtonRelease)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CHLSDeviceDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CHLSDeviceDlg message handlers
void CHLSDeviceDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
}

void CHLSDeviceDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}

void CHLSDeviceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHLSDeviceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHLSDeviceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CHLSDeviceDlg::OnInitDialog()  //主窗口初始化
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_rtmp_start_btn.EnableWindow(TRUE);
	m_rtmp_stop_btn.EnableWindow(FALSE);
	//m_encode_start.EnableWindow(TRUE);
	//m_encode_stop.EnableWindow(FALSE);

	//摄像设备相关设置
	/*camera_label = 0; //将要连接的驱动标号，可能有多个摄像设备，这里仅连接第1个摄像设备
	cameraCtrl = new CameraControl(this);  //创建摄像设备控制对象
	isRuning = false;

	//编码相关设置
	yuv_buf = NULL;
	handle = -1;
	fp = NULL;*/

	//////////////////////////////////////////////////////////////////////////
	start_audio_sampler();

	SetWindowText("HLSDevice");

	//将程序所在路径设为当前路径
	char cur_path[MAX_PATH];

	::ZeroMemory(cur_path, sizeof(cur_path));
	::GetModuleFileName(NULL, cur_path, sizeof(cur_path));

	std::string cur_path_str(cur_path);
	cur_path_str = cur_path_str.substr(0, cur_path_str.rfind("\\")) + "\\";

	::SetCurrentDirectory(cur_path_str.c_str());

	//开启日志
	std::map<int, std::string> log_file_map;
	log_file_map[0] = "HLSDevice.log";	
	SINGLETON(CLogger).open("", log_file_map, cur_path_str + "log");
	LOG_SET_LEVEL(HIGHEST_LOG_LEVEL);

	//读取配置文件
	SINGLETON(CConfigBox).load("HLSDevice.ini");

	//启动服务
	try
	{
		if(SS_NoErr != SINGLETON(CScheduleServer).start(cur_path_str))//启动服务
		{
			AfxMessageBox("Fail in initializing UserAgent!");
		}
	}
	catch(...)
	{
		LOG_WRITE("<FAIL> ScheduleServer failed in stopping!", 1, true);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHLSDeviceDlg::OnClose()  //点击主窗口右上角“关闭”按钮的操作
{
	// TODO: Add your message handler code here and/or call default
	//isRuning = FALSE;
	//releaseResource();

	SINGLETON(CScheduleServer).shutdown();

	//shutdown_audio_sampler();

	CDialogEx::OnClose();
}

void CHLSDeviceDlg::OnBnClickedButtonRTMPStart()  //连接(摄像设备)
{
	//start_audio_sampler();
	
	SINGLETON(CScheduleServer).start_rtmp_push();

	m_rtmp_start_btn.EnableWindow(FALSE);
	m_rtmp_stop_btn.EnableWindow(TRUE);
}

void CHLSDeviceDlg::OnBnClickedButtonRTMPStop()  //断开连接
{
	SINGLETON(CScheduleServer).stop_rtmp_push();

	//shutdown_audio_sampler();

	m_rtmp_start_btn.EnableWindow(TRUE);
	m_rtmp_stop_btn.EnableWindow(FALSE);
}

//bool _first_frame = true;
//unsigned long _start_time = 0;
//unsigned long recv_time = 0;
void CHLSDeviceDlg::on_sample_video_frame(unsigned char* data, int width, int height)
{
	//TRACE("\nV %d", timeGetTime() - recv_time);
	//recv_time = timeGetTime();
	//return;

	/*if(NULL == _yuv_buf || NULL == _yuv_scale)
		return;

	CUserAgent* ua = SINGLETON(CScheduleServer).fetch_ua(1);
	if(NULL == ua)
		return;

	if(false == ua->_start_hls)
		return;

	//转码
	if(_camera_ctrl->imgFormat == 1)  //I420(12)
	{
	}
	else if(_camera_ctrl->imgFormat == 2) //RGB（24）
	{
		unsigned char *y = _yuv_buf;
		unsigned char *u = _yuv_buf + _camera_ctrl->imgWidth * _camera_ctrl->imgHeight;
		unsigned char *v = _yuv_buf + _camera_ctrl->imgWidth * _camera_ctrl->imgHeight * 5 / 4;
		CTranscoding::RGBtoYUV(_camera_ctrl->imgWidth, _camera_ctrl->imgHeight,data,y,u,v);
		CTranscoding::matrxCovert(_camera_ctrl->imgWidth * _camera_ctrl->imgHeight,   y);
		CTranscoding::matrxCovert(_camera_ctrl->imgWidth * _camera_ctrl->imgHeight/4, u);
		CTranscoding::matrxCovert(_camera_ctrl->imgWidth * _camera_ctrl->imgHeight/4, v);
	} 
	else if(_camera_ctrl->imgFormat == 3) //YUY2（16）
	{
		CTranscoding::YUY2toYUV(data, _camera_ctrl->imgWidth, _camera_ctrl->imgHeight, _yuv_buf);
	}
	else
	{
		return;
	}

	CSizescale::scale(_yuv_scale, VIDEO_WIDTH, VIDEO_HEIGHT, _yuv_buf, _camera_ctrl->imgWidth, _camera_ctrl->imgHeight);

	ua->input_video_frame(_yuv_scale);*/
}

//AudioSampler
void CHLSDeviceDlg::start_audio_sampler()
{
	m_pRecodeSound = new CRecodeSound();//this);
	m_pRecodeSound->CreateThread();
	m_pRecodeSound->PostThreadMessage(WM_RECORDSOUND_STARTRECORDING, 0, 0);
}

void CHLSDeviceDlg::shutdown_audio_sampler()
{
	if (m_pRecodeSound)
	{
		m_pRecodeSound->PostThreadMessage(WM_RECORDSOUND_STOPRECORDING, 0, 0);
		m_pRecodeSound->PostThreadMessage(WM_RECORDSOUND_ENDTHREAD, 0, 0);
		
		delete m_pRecodeSound;
		m_pRecodeSound = NULL;
	}
}



void CHLSDeviceDlg::OnBnClickedButtonDetect()
{
	// TODO: Add your control notification handler code here
	m_video_capture_btn.EnableWindow(FALSE);
	m_video_stop_btn.EnableWindow(FALSE);

	m_list.ResetContent();
	m_param_lst.ResetContent();

	if(NULL!=_camera_ctrl)
	{
		_camera_ctrl->ReleaseCapture();
		delete _camera_ctrl;
		_camera_ctrl = NULL;
	}
	_camera_ctrl = new CameraControl();

	if(_camera_ctrl->ListCaptureDevices())
	{
		for(int i=0; i<_camera_ctrl->cameraDeviceNum; i++)
		{
			m_list.InsertString(i, _camera_ctrl->CameraDeviceName[i]);
		}
		m_list.SetCurSel(0);

		CString filterName, str;
		m_list.GetLBText(0, filterName);
		const char * friendlyName = (LPSTR)(LPCTSTR)filterName;

		if(_camera_ctrl->CreateHardwareFilter(friendlyName))
		{
			if(_camera_ctrl->CreateCaptureGraphBuilder())
			{
				_camera_ctrl->Enumerator(_camera_ctrl->pBuild, _camera_ctrl->pCap);
				if(_camera_ctrl->cameraFormatNum>0)
				{
					for(int i=0; i<_camera_ctrl->cameraFormatNum; i++)
					{
						int width = (int)_camera_ctrl->WidthList[i];
						int height = (int)_camera_ctrl->HeightList[i];
						int fps = (int)(10000000/_camera_ctrl->FPSList[i]);
						str.Format("W: %4d, H: %4d, FPS: %2d, %s", width, height, fps, _camera_ctrl->ColorSpaceNameList[i]);
						m_param_lst.InsertString(i, str);
					}
					m_param_lst.SetCurSel(0);
				}
			}
		}
	}

	if(0 < m_param_lst.GetCount())
	{
		m_video_capture_btn.EnableWindow(TRUE);
	}
	else
	{
		AfxMessageBox("检测完毕，未发现可用的摄像设备！");
	}
	m_video_stop_btn.EnableWindow(FALSE);

	UpdateData(FALSE);
}


void CHLSDeviceDlg::OnBnClickedButtonCapture()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//CString filterName,str;
	int index = m_param_lst.GetCurSel();
	if(index<0) return;

	if(_camera_ctrl->SetCaptureParameters(index))
	{
		HWND hWnd = GetDlgItem(IDC_PICTURE)->m_hWnd;
		CRect rect;
		m_pic.GetClientRect(&rect);
		_camera_ctrl->SetPreviewWindow(hWnd, rect.Width(), rect.Height());
		if(!_camera_ctrl->StartCapture())
		{
			AfxMessageBox("error!");
		}
		m_video_capture_btn.EnableWindow(FALSE);
		m_video_stop_btn.EnableWindow(TRUE);

		CUserAgent::_VIDEO_WIDTH = _camera_ctrl->lWidth;
		CUserAgent::_VIDEO_HEIGHT = _camera_ctrl->lHeight;
		CUserAgent::_VIDEO_SAMPLE_INTERVAL = _camera_ctrl->fps / 10000;
		//str.Format("%ld", _camera_ctrl->lWidth);
		//m_width.SetWindowTextA(str);
		//str.Format("%ld", _camera_ctrl->lHeight);
		//m_height.SetWindowTextA(str);
		//str.Format("%lld", 10000000/_camera_ctrl->fps);
		//m_fps.SetWindowTextA(str);
		//str.Format("%s", _camera_ctrl->colorSpace);
		//m_color.SetWindowTextA(str);
	}

	UpdateData(FALSE);
}

void CHLSDeviceDlg::OnBnClickedButtonStop()
{
	// TODO: Add your control notification handler code here
	_camera_ctrl->StopCapture();
	m_video_capture_btn.EnableWindow(TRUE);
	m_video_stop_btn.EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CHLSDeviceDlg::OnBnClickedButtonRelease()
{
	// TODO: Add your control notification handler code here
	//SINGLETON(CScheduleServer).h264_test();

	if(NULL != _camera_ctrl)
	{
		_camera_ctrl->ReleaseCapture();
		delete _camera_ctrl;
		_camera_ctrl = NULL;
	}
	m_video_capture_btn.EnableWindow(FALSE);
	m_video_stop_btn.EnableWindow(FALSE);

	UpdateData(FALSE);
}

void CHLSDeviceDlg::OnCbnSelchangeCombo1()
{
	UpdateData(TRUE);
	CString filterName,str;
	int index = m_list.GetCurSel();
	if(index<0) return;
	m_list.GetLBText(index, filterName);

	m_video_capture_btn.EnableWindow(FALSE);
	m_video_stop_btn.EnableWindow(FALSE);
	//将friendlyName与所有的设备名称依次对比，如果相同，则创建Filter
	const char * friendlyName = (LPSTR)(LPCTSTR)filterName;

	if(NULL != _camera_ctrl)
	{
		_camera_ctrl->ReleaseCapture();
		delete _camera_ctrl;
		_camera_ctrl = NULL;
	}
	_camera_ctrl = new CameraControl();

	m_param_lst.ResetContent();
	if(_camera_ctrl->CreateHardwareFilter(friendlyName))
	{
		if(_camera_ctrl->CreateCaptureGraphBuilder())
		{
			_camera_ctrl->Enumerator(_camera_ctrl->pBuild, _camera_ctrl->pCap);
			if(_camera_ctrl->cameraFormatNum>0)
			{
				for(int i=0; i<_camera_ctrl->cameraFormatNum; i++)
				{
					int width = (int)_camera_ctrl->WidthList[i];
					int height = (int)_camera_ctrl->HeightList[i];
					int fps = (int)(10000000/_camera_ctrl->FPSList[i]);
					str.Format("Width: %d,  Height: %d,  FPS: %d, Format: %s", width, height, fps, _camera_ctrl->ColorSpaceNameList[i]);
					m_param_lst.InsertString(i, str);
				}
				m_param_lst.SetCurSel(0);
			}
		}
	}

	if(0 < m_param_lst.GetCount())
		m_video_capture_btn.EnableWindow(TRUE);
	
	m_video_stop_btn.EnableWindow(FALSE);
	
	UpdateData(FALSE);
}