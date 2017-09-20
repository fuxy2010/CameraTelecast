#ifndef _CAMERA_CONTROL_H
#define _CAMERA_CONTROL_H


#include "../DirectShow/ATLBASE.H"
#include "../DirectShow/DShow.h"
#include "../DirectShow/qedit.h"

#define _MAX_CAMERA_DEVICE_NUMBER      5
#define _MAX_CAMERA_PARAM_LIST_LENGTH  100

#pragma comment(lib,"../DirectShow/Strmiids.lib")
#pragma comment(lib,"../DirectShow/quartz.lib")
#pragma comment(lib,"../DirectShow/strmbasd.lib")


class CameraControl : public ISampleGrabberCB
{
public:
	CameraControl(void);
	~CameraControl(void);

public:
	BOOL ListCaptureDevices();  //设备列表
	
	BOOL CreateHardwareFilter(const char * deviceName);  //创建视频采集过滤器
	BOOL CreateCaptureGraphBuilder();  //创建视频采集过滤器图表
	void Enumerator(ICaptureGraphBuilder2 * &g_pCapture, IBaseFilter * & pVCap);  //得到各种摄像参数组合
	BOOL SetCaptureParameters(int index);  //设置参数
	void SetPreviewWindow(HWND hWnd, int rect_width, int rect_height);  //设置预览窗口
	BOOL StartCapture();  //开始录像
	BOOL StopCapture();   //停止录像
	void ReleaseCapture();
	void NukeDownstream(IBaseFilter *pf);

public:
	/* ISampleGrabberCB */
	STDMETHODIMP_(ULONG) AddRef(); 
    STDMETHODIMP_(ULONG) Release(); 
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP SampleCB(double Time, IMediaSample *pSample);          //Callback function
	STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen);  //Callback function

public:
	static void RGB24toYUV(unsigned char *data, int width, int height, unsigned char *yuv_buf);
	static void matrxCovert(int n,unsigned char *data);
	static void initLookupTable();
	static void RGBtoYUV(unsigned long w,unsigned long h,unsigned char * rgbData,unsigned char * y,unsigned char * u,unsigned char *v);
	static void YUY2toYUV(unsigned char *data, int width, int height, unsigned char *yuv_buf);

public:
	long lWidth;
	long lHeight;
	long long fps;
	char colorSpace[64];
	long pWidth;
	long pHeight;
	char CameraDeviceName[_MAX_CAMERA_DEVICE_NUMBER][1024];  //摄像设备名称
	int  cameraDeviceNum;  //摄像设备数目
	long long FPSList[_MAX_CAMERA_PARAM_LIST_LENGTH];
	long WidthList[_MAX_CAMERA_PARAM_LIST_LENGTH];
	long HeightList[_MAX_CAMERA_PARAM_LIST_LENGTH];
	char ColorSpaceNameList[_MAX_CAMERA_PARAM_LIST_LENGTH][64];
	int ColorSpaceIndexList[_MAX_CAMERA_PARAM_LIST_LENGTH];
	int cameraFormatNum;  //不同摄像参数的组合数目
	unsigned char *yuv_buf;
#if _DEBUG_LOG
	FILE * fp;
#endif

public:
	IBaseFilter *pCap;              //视频采集过滤器
	ICaptureGraphBuilder2 *pBuild;  //过滤器图表管理器
	IGraphBuilder *pGraph;          //采集过滤器图表
	ISampleGrabber *pGrabber;       //媒体数据抓取器
	IVideoWindow *pWindow;          //视频捕获窗口
	IMediaControl *pControl;        //用户命令接口，用来控制过滤器图表
	IMediaEvent *pEvent;            //过滤器图表事件接口
	IAMStreamConfig *pVSC;          //摄像头参数
};

#endif