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
	BOOL ListCaptureDevices();  //�豸�б�
	
	BOOL CreateHardwareFilter(const char * deviceName);  //������Ƶ�ɼ�������
	BOOL CreateCaptureGraphBuilder();  //������Ƶ�ɼ�������ͼ��
	void Enumerator(ICaptureGraphBuilder2 * &g_pCapture, IBaseFilter * & pVCap);  //�õ���������������
	BOOL SetCaptureParameters(int index);  //���ò���
	void SetPreviewWindow(HWND hWnd, int rect_width, int rect_height);  //����Ԥ������
	BOOL StartCapture();  //��ʼ¼��
	BOOL StopCapture();   //ֹͣ¼��
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
	char CameraDeviceName[_MAX_CAMERA_DEVICE_NUMBER][1024];  //�����豸����
	int  cameraDeviceNum;  //�����豸��Ŀ
	long long FPSList[_MAX_CAMERA_PARAM_LIST_LENGTH];
	long WidthList[_MAX_CAMERA_PARAM_LIST_LENGTH];
	long HeightList[_MAX_CAMERA_PARAM_LIST_LENGTH];
	char ColorSpaceNameList[_MAX_CAMERA_PARAM_LIST_LENGTH][64];
	int ColorSpaceIndexList[_MAX_CAMERA_PARAM_LIST_LENGTH];
	int cameraFormatNum;  //��ͬ��������������Ŀ
	unsigned char *yuv_buf;
#if _DEBUG_LOG
	FILE * fp;
#endif

public:
	IBaseFilter *pCap;              //��Ƶ�ɼ�������
	ICaptureGraphBuilder2 *pBuild;  //������ͼ�������
	IGraphBuilder *pGraph;          //�ɼ�������ͼ��
	ISampleGrabber *pGrabber;       //ý������ץȡ��
	IVideoWindow *pWindow;          //��Ƶ���񴰿�
	IMediaControl *pControl;        //�û�����ӿڣ��������ƹ�����ͼ��
	IMediaEvent *pEvent;            //������ͼ���¼��ӿ�
	IAMStreamConfig *pVSC;          //����ͷ����
};

#endif