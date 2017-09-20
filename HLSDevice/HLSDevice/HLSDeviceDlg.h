
// HLSDeviceDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "CameraControl.h"
#include "Transcoding.h"
#include "PCMPlayer.h"
#include "RTMPSession.h"
#include "RecodeSound.h"

// CHLSDeviceDlg dialog
class CHLSDeviceDlg : public CDialogEx
{
// Construction
public:
	CHLSDeviceDlg(CWnd* pParent = NULL);	// standard constructor
	~CHLSDeviceDlg();

// Dialog Data
	enum { IDD = IDD_HLSDEVICE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonRTMPStart();
	afx_msg void OnBnClickedButtonRTMPStop();
	CButton m_rtmp_start_btn;
	CButton m_rtmp_stop_btn;

	CStatic m_pic;
	CComboBox m_list;
	CComboBox m_param_lst;
	CButton m_video_capture_btn;
	CButton m_video_stop_btn;

public:
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

//AudioSampler
protected://AudioSampler

	CRecodeSound	* m_pRecodeSound;

public:
	void start_audio_sampler();
	void shutdown_audio_sampler();

//Video Sampler
protected://VideoSampler
	CameraControl* _camera_ctrl;  //定义摄像设备操作对象

	//编码器相关设置
	unsigned char* _yuv_buf;  //一帧视频图像的缓存
	unsigned char* _yuv_scale;

public:
	void on_sample_video_frame(unsigned char* data, int width, int height);

	afx_msg void OnBnClickedButtonDetect();
	afx_msg void OnBnClickedButtonCapture();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonRelease();
	afx_msg void OnCbnSelchangeCombo1();
};
