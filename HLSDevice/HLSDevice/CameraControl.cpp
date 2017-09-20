#include "StdAfx.h"
#include "CameraControl.h"

#include "resource.h"

#include "scheduleserver.h"
using namespace ScheduleServer;

const GUID WMMEDIASUBTYPE_I420 = {0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71};
const GUID WMMEDIASUBTYPE_M420 = {0x3032344D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71};

typedef struct MY_GUID
{
	LPCTSTR name;
	GUID *guid;
}MyGuid;


MyGuid g_guid[] =
{
    //RGB    
    {_T("RGB1"),   (GUID *)&MEDIASUBTYPE_RGB1},   
    {_T("RGB4"),   (GUID *)&MEDIASUBTYPE_RGB4},   
    {_T("RGB8"),   (GUID *)&MEDIASUBTYPE_RGB8},   
    {_T("RGB565"), (GUID *)&MEDIASUBTYPE_RGB565},   
    {_T("RGB555"), (GUID *)&MEDIASUBTYPE_RGB555},   
    {_T("RGB24"),  (GUID *)&MEDIASUBTYPE_RGB24},   
    {_T("RGB32"),  (GUID *)&MEDIASUBTYPE_RGB32},   
    {_T("ARGB32"), (GUID *)&MEDIASUBTYPE_ARGB32},   
    //YUV    
    {_T("AYUV"), (GUID *)&MEDIASUBTYPE_AYUV},   
    {_T("UYVY"),(GUID *)&MEDIASUBTYPE_UYVY},   
    {_T("Y411"), (GUID *)&MEDIASUBTYPE_Y411},   
    {_T("Y41P"), (GUID *)&MEDIASUBTYPE_Y41P},   
    {_T("Y211"), (GUID *)&MEDIASUBTYPE_Y211},   
    {_T("YUY2"), (GUID *)&MEDIASUBTYPE_YUY2},   
    {_T("YVYU"), (GUID *)&MEDIASUBTYPE_YVYU},   
    {_T("YUYV"), (GUID *)&MEDIASUBTYPE_YUYV},   
    {_T("IF09"), (GUID *)&MEDIASUBTYPE_IF09},   
    {_T("IYUV"), (GUID *)&MEDIASUBTYPE_IYUV},   
    {_T("YV12"), (GUID *)&MEDIASUBTYPE_YV12},   
    {_T("YVU9"), (GUID *)&MEDIASUBTYPE_YVU9},   
    //Miscellaneous Video Subtypes    
    {_T("CFCC"), (GUID *)&MEDIASUBTYPE_CFCC},   
    {_T("CLJR"), (GUID *)&MEDIASUBTYPE_CLJR},   
    {_T("CPLA"), (GUID *)&MEDIASUBTYPE_CPLA},   
    {_T("CLPL"), (GUID *)&MEDIASUBTYPE_CLPL},   
    {_T("IJPG"), (GUID *)&MEDIASUBTYPE_IJPG},   
    {_T("MDVF"), (GUID *)&MEDIASUBTYPE_MDVF},   
    {_T("MJPG"), (GUID *)&MEDIASUBTYPE_MJPG},   
    {_T("MPEG1Packet"), (GUID *)&MEDIASUBTYPE_MPEG1Packet},   
    {_T("MPEG1Payload"),(GUID *)&MEDIASUBTYPE_MPEG1Payload},   
    {_T("Overlay"),    (GUID *)&MEDIASUBTYPE_Overlay},   
    {_T("Plum"),       (GUID *)&MEDIASUBTYPE_Plum},   
    {_T("QTJpeg"),     (GUID *)&MEDIASUBTYPE_QTJpeg},   
    {_T("QTMovie"),    (GUID *)&MEDIASUBTYPE_QTMovie},   
    {_T("QTRle"),      (GUID *)&MEDIASUBTYPE_QTRle},   
    {_T("QTRpza"),     (GUID *)&MEDIASUBTYPE_QTRpza},   
    {_T("QTSmc"),      (GUID *)&MEDIASUBTYPE_QTSmc},   
    {_T("TVMJ"),       (GUID *)&MEDIASUBTYPE_TVMJ},   
	//  {_T("VideoPort:"),  (GUID *)&MEDIASUBTYPE_VideoPort},  
    {_T("VPVBI"),      (GUID *)&MEDIASUBTYPE_VPVBI},   
    {_T("VPVideo"),    (GUID *)&MEDIASUBTYPE_VPVideo},   
    {_T("WAKE"),       (GUID *)&MEDIASUBTYPE_WAKE},   
    //Analog Video Subtypes
    {_T("AnalogVideo_NTSC_M"), (GUID *)&MEDIASUBTYPE_AnalogVideo_NTSC_M},   
    {_T("AnalogVideo_PAL_B"), (GUID *)&MEDIASUBTYPE_AnalogVideo_PAL_B},   
    {_T("AnalogVideo_PAL_D"), (GUID *)&MEDIASUBTYPE_AnalogVideo_PAL_D},   
    {_T("AnalogVideo_PAL_G"), (GUID *)&MEDIASUBTYPE_AnalogVideo_PAL_G},   
    {_T("AnalogVideo_PAL_H"), (GUID *)&MEDIASUBTYPE_AnalogVideo_PAL_H},   
    {_T("AnalogVideo_PAL_I"), (GUID *)&MEDIASUBTYPE_AnalogVideo_PAL_I},   
    {_T("AnalogVideo_PAL_M"), (GUID *)&MEDIASUBTYPE_AnalogVideo_PAL_M},   
    {_T("AnalogVideo_PAL_N"), (GUID *)&MEDIASUBTYPE_AnalogVideo_PAL_N},   
    {_T("AnalogVideo_SECAM_B"), (GUID *)&MEDIASUBTYPE_AnalogVideo_SECAM_B},   
    {_T("AnalogVideo_SECAM_D"), (GUID *)&MEDIASUBTYPE_AnalogVideo_SECAM_D},   
    {_T("AnalogVideo_SECAM_G"), (GUID *)&MEDIASUBTYPE_AnalogVideo_SECAM_G},   
    {_T("AnalogVideo_SECAM_H"), (GUID *)&MEDIASUBTYPE_AnalogVideo_SECAM_H},   
    {_T("AnalogVideo_SECAM_K"), (GUID *)&MEDIASUBTYPE_AnalogVideo_SECAM_K},   
    {_T("AnalogVideo_SECAM_K1"), (GUID *)&MEDIASUBTYPE_AnalogVideo_SECAM_K1},   
    {_T("AnalogVideo_SECAM_L"), (GUID *)&MEDIASUBTYPE_AnalogVideo_SECAM_L},   
    //DirectX Video Acceleration Video Subtypes
    {_T("AI44"), (GUID *)&MEDIASUBTYPE_AI44},   
    {_T("IA44"), (GUID *)&MEDIASUBTYPE_IA44},   
    //DV Video Subtypes
    {_T("dvsl"), (GUID *)&MEDIASUBTYPE_dvsl},   
    {_T("dvsd"), (GUID *)&MEDIASUBTYPE_dvsd},   
    {_T("dvhd"), (GUID *)&MEDIASUBTYPE_dvhd},   
    {_T("dv25"), (GUID *)&MEDIASUBTYPE_dv25},   
    {_T("dv50"), (GUID *)&MEDIASUBTYPE_dv50},   
    {_T("dvh1"), (GUID *)&MEDIASUBTYPE_dvh1},   
    {_T("DVCS"), (GUID *)&MEDIASUBTYPE_DVCS},   
    {_T("DVSD"), (GUID *)&MEDIASUBTYPE_DVSD},   
    //Video Mixing Renderer Video Subtypes  
    {_T("RGB32_D3D_DX7_RT"), (GUID *)&MEDIASUBTYPE_RGB32_D3D_DX7_RT},   
    {_T("RGB16_D3D_DX7_RT"), (GUID *)&MEDIASUBTYPE_RGB16_D3D_DX7_RT},   
    {_T("ARGB32_D3D_DX7_RT"), (GUID *)&MEDIASUBTYPE_ARGB32_D3D_DX7_RT},   
    {_T("ARGB4444_D3D_DX7_RT"), (GUID *)&MEDIASUBTYPE_ARGB4444_D3D_DX7_RT},   
    {_T("ARGB1555_D3D_DX7_RT"), (GUID *)&MEDIASUBTYPE_ARGB1555_D3D_DX7_RT},   
    //自定义
    {_T("I420"), (GUID *)&WMMEDIASUBTYPE_I420},  
	{_T("M420"), (GUID *)&WMMEDIASUBTYPE_M420}
};   

int g_iguid = sizeof(g_guid)/sizeof(MyGuid);


CameraControl::CameraControl(void):
	cameraDeviceNum(0),
	cameraFormatNum(0),
	yuv_buf(NULL),
	pGraph(NULL),
	pBuild(NULL),
	pCap(NULL),
	pWindow(NULL),
	pControl(NULL),
	pEvent(NULL),
	pGrabber(NULL),
	pVSC(NULL)
{
}


CameraControl::~CameraControl(void)
{
	ReleaseCapture();
}


BOOL CameraControl::ListCaptureDevices()
{
	ICreateDevEnum *pDevEnum = NULL; //设备枚举器Interface
    IEnumMoniker *pEnum = NULL;      //名称枚举Interface
    
    //创建设备枚举COM对象
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
									CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
									reinterpret_cast<void**>(&pDevEnum));
    if (SUCCEEDED(hr))
    {
		//创建视频采集设备枚举COM对象 
        hr = pDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory,&pEnum, 0);        
    }
    
    IMoniker *pMoniker = NULL;
    if(pEnum == NULL)
    {
        return FALSE; //如果没有设备,返回
    }

	cameraDeviceNum = 0;
    //依次枚举，直至为空 
    while (pEnum->Next(1, &pMoniker, NULL)==S_OK)
    {
        IPropertyBag *pPropBag;
        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));
        if (FAILED(hr))      
        {
            pMoniker->Release();
            continue; //Skip this one, maybe the next one will work.            
        }
        
        //Find the description or friendly name.
        VARIANT varName;
		varName.vt=VT_BSTR;
        hr = pPropBag->Read(L"Description", &varName, 0);
        if(FAILED(hr))            
        {            
            hr = pPropBag->Read(L"FriendlyName", &varName, 0); //设备友好名称            
        }
        
        if (SUCCEEDED(hr) && cameraDeviceNum<_MAX_CAMERA_DEVICE_NUMBER)            
        {
			//字符转换，枚举名称均为UNICODE码
            WideCharToMultiByte(CP_ACP,0,varName.bstrVal,-1,CameraDeviceName[cameraDeviceNum],1024,NULL,NULL);
			cameraDeviceNum++;       
        }
        pPropBag->Release();
        pMoniker->Release();
    }

	if(cameraDeviceNum<1)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CameraControl::CreateHardwareFilter(const char * deviceName)
{
	BOOL flag = FALSE;

	//设备枚举器Interface
	ICreateDevEnum * enumHardware = NULL;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum,NULL,CLSCTX_ALL,IID_ICreateDevEnum,(void **)&enumHardware);    
    if( FAILED(hr) )        
    {        
        return flag;
    }

	//名称枚举器Interface   
    IEnumMoniker * enumMoniker = NULL;
    hr = enumHardware->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&enumMoniker,0);    
    if(enumMoniker)        
    {
        enumMoniker->Reset();   
        IMoniker * moniker = NULL;

        while(!pCap && SUCCEEDED(enumMoniker->Next(1,&moniker,NULL)))
        {
            if(moniker)                
            {
                IPropertyBag * propertyBag = NULL;   
                VARIANT name;
                hr=moniker->BindToStorage(0,0,IID_IPropertyBag,(void **)&propertyBag);
                if(SUCCEEDED(hr))
                {
                    name.vt=VT_BSTR;
					hr = propertyBag->Read(L"Description", &name, NULL);
					if(FAILED(hr))
					{
						hr = propertyBag->Read(L"FriendlyName", &name, NULL);
					}
                }                
                else
                {
					moniker->Release();
					continue;
                }

                if(SUCCEEDED(hr))                    
                {
					char displayName[1024];
                    WideCharToMultiByte(CP_ACP,0,name.bstrVal,-1,displayName,1024,NULL,NULL);
					if(0==strcmp(deviceName, displayName))
					{
						moniker->BindToObject(0,0,IID_IBaseFilter,(void **)&pCap);
						flag = TRUE;
					}
                }

                if(propertyBag)
                {
                    propertyBag->Release();
                    propertyBag=NULL;
                }
               moniker->Release();
            }
        }
        enumMoniker->Release();
    }
    enumHardware->Release();

	return flag;
}


BOOL CameraControl::CreateCaptureGraphBuilder()
{
	HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pBuild);
    if(FAILED(hr))
    {
        return FALSE;
    }
    
    hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
    if(FAILED(hr))        
    {
		pBuild->Release();
		pBuild = NULL;
        return FALSE;
    }

	pBuild->SetFiltergraph(pGraph);             //过滤器图表添加到管理器中
	pGraph->AddFilter(pCap,L"Capture Filter");  //将Filter添加到过滤器图表中

	return TRUE;
}

void CameraControl::Enumerator(ICaptureGraphBuilder2 * &g_pCapture, IBaseFilter * & pVCap)
{
	HRESULT hr;
    
    hr = g_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pVCap, IID_IAMStreamConfig, (void **)&pVSC);   
	if(hr != NOERROR)   
	{   
		hr = g_pCapture->FindInterface(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video, pVCap, IID_IAMStreamConfig, (void **)&pVSC);   
	}   
	if(SUCCEEDED(hr))   
	{   
		int iCount = 0, iSize = 0;   
        hr = pVSC->GetNumberOfCapabilities(&iCount, &iSize);
        if(iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))   
        {
			cameraFormatNum = 0;
			for (int iFormat = 0; iFormat<iCount; iFormat++)   
			{   
				VIDEO_STREAM_CONFIG_CAPS scc;   
				AM_MEDIA_TYPE *pmtConfig = NULL;   
				hr = pVSC->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);   
				if(SUCCEEDED(hr))
				{
					//颜色空间
					int i;
					for(i = 0; i<g_iguid; i++)   
					{
						if((pmtConfig->subtype == *g_guid[i].guid)&&(cameraFormatNum<_MAX_CAMERA_PARAM_LIST_LENGTH))   
						{
							VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) pmtConfig->pbFormat;
							if((vih->bmiHeader.biWidth>0) &&  (vih->bmiHeader.biHeight>0) && (vih->bmiHeader.biWidth%16==0) && (vih->bmiHeader.biHeight%8==0))
							{
								FPSList[cameraFormatNum]    = vih->AvgTimePerFrame;
								WidthList[cameraFormatNum]  = vih->bmiHeader.biWidth;
								HeightList[cameraFormatNum] =  vih->bmiHeader.biHeight;
								strcpy_s(ColorSpaceNameList[cameraFormatNum], g_guid[i].name);
								ColorSpaceIndexList[cameraFormatNum] = i;
								cameraFormatNum++;
							}
							break;
						}
					}
					if((i>=g_iguid)&&(cameraFormatNum<_MAX_CAMERA_PARAM_LIST_LENGTH))
					{   
						VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) pmtConfig->pbFormat;
						if((vih->bmiHeader.biWidth>0) &&  (vih->bmiHeader.biHeight>0) && (vih->bmiHeader.biWidth%16==0) && (vih->bmiHeader.biHeight%8==0))
						{
							FPSList[cameraFormatNum]    = vih->AvgTimePerFrame;
							WidthList[cameraFormatNum]  = vih->bmiHeader.biWidth;
							HeightList[cameraFormatNum] =  vih->bmiHeader.biHeight;
							strcpy_s(ColorSpaceNameList[cameraFormatNum], _T("UNKNOWTPYE"));
							ColorSpaceIndexList[cameraFormatNum] = i;
							cameraFormatNum++;
						}
					}
				}
			}
		}
	}
}


BOOL CameraControl::SetCaptureParameters(int index)
{
	if(ColorSpaceIndexList[index]>=g_iguid)
	{
		return FALSE;
	}

	if(NULL!=pGrabber)
	{
		pGrabber->Release();
		pGrabber = NULL;
	}
	if(NULL!=pCap && NULL!=pGraph)
	{
		NukeDownstream(pCap);
	}

	HRESULT hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_ISampleGrabber, (void**)&pGrabber);
	if( !pGrabber )
	{
		return FALSE;  //Fail to create SampleGrabber, maybe qedit.dll is not registered?
	}

	CComQIPtr<IBaseFilter, &IID_IBaseFilter> pGrabBase(pGrabber);

	//设置格式
	AM_MEDIA_TYPE *pmt;
	pVSC->GetFormat(&pmt);
	VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
	pmt->majortype = MEDIATYPE_Video;
	pmt->bTemporalCompression = FALSE;
	pmt->subtype = *g_guid[ColorSpaceIndexList[index]].guid;
    pvi->AvgTimePerFrame = FPSList[index];
	pvi->bmiHeader.biWidth = WidthList[index];
	pvi->bmiHeader.biHeight = HeightList[index];

	pVSC->SetFormat(pmt);
	hr = pGrabber->SetMediaType(pmt);
	if(FAILED(hr))
	{
		return FALSE;  //Fail to set media type
	}

	hr = pGraph->AddFilter(pGrabBase, L"Grabber");
	if(FAILED(hr))
	{
		return FALSE;  //Fail to put sample grabber in graph
	}
	
	//try to render preview/capture pin
	hr = pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,pCap,pGrabBase,NULL);
	if(FAILED(hr))
	{
		hr = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,pCap,pGrabBase,NULL);
	}
	if(FAILED(hr))
	{
		return FALSE;  //Can’t build the graph
	}

	hr = pGrabber->GetConnectedMediaType(pmt);
	if(FAILED(hr))
	{
		return FALSE;  //Failt to read the connected media type
	}

	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) pmt->pbFormat;
	this->lWidth = vih->bmiHeader.biWidth;
	this->lHeight = vih->bmiHeader.biHeight;
	this->fps = vih->AvgTimePerFrame;
	for(int i = 0; i<g_iguid; i++)   
	{
		if(pmt->subtype == *g_guid[i].guid)
		{
			strcpy_s(this->colorSpace, g_guid[i].name);
			break;
		}
	}

	if(NULL!=yuv_buf)
	{
		free(yuv_buf);
		yuv_buf = NULL;
	}
	yuv_buf = (unsigned char *)malloc(this->lWidth * this->lHeight * 3 / 2 * sizeof(char));
	
	hr = pGrabber->SetBufferSamples(FALSE);
	hr = pGrabber->SetOneShot(FALSE);
	hr = pGrabber->SetCallback(this, 1);

	return TRUE;
}


void CameraControl::SetPreviewWindow(HWND hWnd, int rect_width, int rect_height)
{
	if(NULL!=pControl)
	{
		pControl->Stop();
		pControl->Release();
		pControl = NULL;
	}
	if(NULL!=pEvent)
	{
		pEvent->Release();
		pEvent = NULL;
	}
	if(NULL!=pWindow)     //IVideoWindow *pVW;
    {
        // stop drawing in our window, or we may get wierd repaint effects
        pWindow->put_Owner(NULL);
        pWindow->put_Visible(OAFALSE);
        pWindow->Release();
		pWindow = NULL;
    }

	//设置视频捕捉窗口
	pGraph->QueryInterface(IID_IVideoWindow,(void**)&pWindow);
	pGraph->QueryInterface(IID_IMediaControl,(void **)&pControl);  //gobal pControl
    pGraph->QueryInterface(IID_IMediaEvent,(void **)&pEvent);      //gobal pEvent
	pWindow->put_Owner((OAHWND)hWnd);
	pWindow->put_WindowStyle(WS_CHILD);
	pWindow->get_Width(&pWidth);
	pWindow->get_Height(&pHeight);
	pWindow->SetWindowPosition(0, 0, rect_width, rect_height);
	pWindow->put_Visible(OATRUE);
}


BOOL CameraControl::StartCapture()
{
	HRESULT hr = pControl->Run(); //开始视频捕捉
	if(FAILED(hr))
	{
		return FALSE; //Couldn't run the graph
	}
	return TRUE;
}

BOOL CameraControl::StopCapture()
{
	HRESULT hr = pControl->Stop();
	if(FAILED(hr))
	{
		return FALSE; //Couldn't stop the graph
	}

	return TRUE;
}


void CameraControl::NukeDownstream(IBaseFilter *pf)
{
	IPin *pP=0, *pTo=0;
    ULONG u;
    IEnumPins *pins = NULL;
    PIN_INFO pininfo;
	
    if(NULL==pf)
        return;
	
    HRESULT hr = pf->EnumPins(&pins);
    pins->Reset();
	
    while(hr == NOERROR)
    {
        hr = pins->Next(1, &pP, &u);
        if(hr == S_OK && pP)
        {
            pP->ConnectedTo(&pTo);
            if(pTo)
            {
                hr = pTo->QueryPinInfo(&pininfo);
                if(hr == NOERROR)
                {
                    if(pininfo.dir == PINDIR_INPUT)
                    {
                        NukeDownstream(pininfo.pFilter);
                        pGraph->Disconnect(pTo);
                        pGraph->Disconnect(pP);
                        pGraph->RemoveFilter(pininfo.pFilter);
                    }
                    pininfo.pFilter->Release();
                }
                pTo->Release();
            }
            pP->Release();
        }
    }
	
    if(pins)
        pins->Release();
}


void CameraControl::ReleaseCapture()
{
	if(NULL!=pControl)
	{
		pControl->Stop();
		pControl->Release();
		pControl = NULL;
	}
	if(NULL!=pEvent)
	{
		pEvent->Release();
		pEvent = NULL;
	}
	if(NULL!=pGrabber)
	{
		pGrabber->Release();
		pGrabber = NULL;
	}
	if(NULL!=pWindow)     //IVideoWindow *pVW;
    {
        // stop drawing in our window, or we may get wierd repaint effects
        pWindow->put_Owner(NULL);
        pWindow->put_Visible(OAFALSE);
        pWindow->Release();
		pWindow = NULL;
    }
	if(NULL!=pCap && NULL!=pGraph)
	{
		NukeDownstream(pCap);
	}
	if(NULL!=pGraph)
	{
		pGraph->Release();
		pGraph = NULL;
	}
	if(NULL!=pBuild)
	{
		pBuild->Release();
		pBuild = NULL;
	}
	if(NULL!=pCap)
	{
		pCap->Release();
		pCap = NULL;
	}
	if(NULL!=pVSC)
	{
		pVSC->Release();
		pVSC = NULL;
	}
	if(NULL!=yuv_buf)
	{
		free(yuv_buf);
		yuv_buf = NULL;
	}
}


ULONG STDMETHODCALLTYPE CameraControl::AddRef()
{
	return 1;
}


ULONG STDMETHODCALLTYPE CameraControl::Release()
{
	return 2;
}


STDMETHODIMP CameraControl::QueryInterface(REFIID riid, void **ppvObject)
{
    if (NULL == ppvObject)
		return E_POINTER;

    if (riid == __uuidof(IUnknown))
    {
        *ppvObject = static_cast<IUnknown*>(this);
		return S_OK;
    }

    if (riid == __uuidof(ISampleGrabberCB))
    {
        *ppvObject = static_cast<ISampleGrabberCB*>(this);
		return S_OK;
    }

    return E_NOTIMPL;
}


STDMETHODIMP CameraControl::SampleCB(double Time, IMediaSample *pSample)
{
    return 0;
}

unsigned long vtime1 = 0;
unsigned long vtime2 = 0;
unsigned long vduration = 0;
unsigned long vframes = 0;
STDMETHODIMP CameraControl::BufferCB(double Time, BYTE *pBuffer, long BufferLen)
{
	if(!pBuffer) return 0;

	if(false)
	{
		vtime2 = timeGetTime();

		vduration += vtime2 - vtime1;
		vframes += 1;

		if(100 == vframes)
		{
			TRACE("\nAvg %d", vduration / 100);

			vduration = 0;
			vframes = 0;
		}
		vtime1 = vtime2;
	}

	if(!strcmp(colorSpace, "RGB24"))  //RGB (24)
	{
		RGB24toYUV(pBuffer, lWidth, lHeight, yuv_buf);
#if _DEBUG_LOG
		//fwrite(yuv_buf, 1, lWidth * lHeight * 3 / 2, fp);
#endif
	} 
	else if(!strcmp(colorSpace, "YUY2")) //YUY2（16）
	{
		YUY2toYUV(pBuffer, lWidth, lHeight, yuv_buf);
#if _DEBUG_LOG
		//fwrite(yuv_buf, 1, lWidth * lHeight * 3 / 2, fp);
#endif
	}
	else if(!strcmp(colorSpace, "I420")) //I420（12）
	{
		memcpy(yuv_buf, pBuffer, lWidth * lHeight * 3 / 2);
	}

	CUserAgent* ua = SINGLETON(CScheduleServer).fetch_ua(1);
	if(NULL == ua)
		return E_NOTIMPL;

	if(false == ua->_start_hls)
		return E_NOTIMPL;

	ua->input_video_frame(yuv_buf);
	
	return E_NOTIMPL;
}


float RGBYUV02990[256],RGBYUV05870[256],RGBYUV01140[256];
float RGBYUV01684[256],RGBYUV03316[256];
float RGBYUV04187[256],RGBYUV00813[256];

void CameraControl::matrxCovert(int n,unsigned char *data)
{
	unsigned char temp;
	for (int i=0;i<n/2;i++)
	{
		temp=*(data+i);
		*(data+i)=*(data+n-i-1);
		*(data+n-i-1)=temp;
	}
}

void CameraControl::initLookupTable()
{
	for (int i=0;i<256;i++)
	{
		RGBYUV02990[i] = (float)0.2990 * i;
		RGBYUV05870[i] = (float)0.5870 * i;
		RGBYUV01140[i] = (float)0.1140 * i;
		RGBYUV01684[i] = (float)0.1684 * i;
		RGBYUV03316[i] = (float)0.3316 * i;
		RGBYUV04187[i] = (float)0.4187 * i;
		RGBYUV00813[i] = (float)0.0813 * i;
	}
}

void CameraControl::RGBtoYUV(unsigned long w,unsigned long h,unsigned char * rgbData,unsigned char * y,unsigned char * u,unsigned char *v)
{
	initLookupTable();//初始化查找表
	unsigned char *ytemp = NULL;
	unsigned char *utemp = NULL;
	unsigned char *vtemp = NULL;
	utemp = (unsigned char *)malloc(w*h);
	vtemp = (unsigned char *)malloc(w*h);

	unsigned long i,nr,ng,nb,nSize;
	//对每个像素进行 rgb -> yuv的转换
	for (i=0,nSize=0;nSize<w*h*3;nSize+=3)
	{
		nb = rgbData[nSize];
		ng = rgbData[nSize+1];
		nr = rgbData[nSize+2];
		y[i] = (unsigned char)(RGBYUV02990[nr]+RGBYUV05870[ng]+RGBYUV01140[nb]);
		utemp[i] = (unsigned char)(-RGBYUV01684[nr]-RGBYUV03316[ng]+nb/2+128);
		vtemp[i] = (unsigned char)(nr/2-RGBYUV04187[ng]-RGBYUV00813[nb]+128);
		i++;
	}
	//对u信号及v信号进行采样
	int k = 0;
	for (i=0;i<h;i+=2)
		for(unsigned long j=0;j<w;j+=2)
		{
			u[k]=(utemp[i*w+j]+utemp[(i+1)*w+j]+utemp[i*w+j+1]+utemp[(i+1)*w+j+1])/4;
			v[k]=(vtemp[i*w+j]+vtemp[(i+1)*w+j]+vtemp[i*w+j+1]+vtemp[(i+1)*w+j+1])/4;
			k++;
		}
		//对y、u、v 信号进行抗噪处理
		for (i=0;i<w*h;i++)
		{
			if(y[i]<16)
				y[i] = 16;
			if(y[i]>235)
				y[i] = 235;
		}
		for(i=0;i<h*w/4;i++)
		{
			if(u[i]<16)
				u[i] = 16;
			if(v[i]<16)
				v[i] = 16;
			if(u[i]>240)
				u[i] = 240;
			if(v[i]>240)
				v[i] = 240;
		}
		if(utemp)
			free(utemp);
		if(vtemp)
			free(vtemp);
}


void CameraControl::RGB24toYUV(unsigned char *data, int width, int height, unsigned char *yuv_buf)
{
	unsigned char *y = yuv_buf;
	unsigned char *u = yuv_buf + width * height;
	unsigned char *v = yuv_buf + width * height * 5 / 4;
	RGBtoYUV(width, height, data, y, u, v);
	matrxCovert(width * height,   y);
	matrxCovert(width * height/4, u);
	matrxCovert(width * height/4, v);
}


void CameraControl::YUY2toYUV(unsigned char *data,int width,int height,unsigned char *yuv_buf)
{
	unsigned char *Y, *U, *V;
	unsigned char * Y2, *U2, *V2;
	unsigned char *p=NULL;
	unsigned char *tem_buf;
	tem_buf = (unsigned char *)malloc(width*height*2*sizeof(char));
	memset(tem_buf, 0, width*height*2);

	p = data;

	Y = tem_buf;
	U = Y + width*height;
	V = U + (width*height>>1);

	Y2 = yuv_buf;
	U2 = Y2 + width*height;
	V2 = U2 + (width*height>>2);

	for(int k=0; k<height; ++k)
	{ 
		for(int j=0; j<(width>>1); ++j)
		{   
			Y[j*2] = p[4*j];    
			U[j] = p[4*j+1];
			Y[j*2+1] = p[4*j+2];
			V[j] = p[4*j+3];
		}
		p = p + width*2;

		Y = Y + width;
		U = U + (width>>1);
		V = V + (width>>1);
	}

	Y = tem_buf;
	U = Y + width*height;
	V = U + (width*height>>1);

	for(int l=0; l<height/2; ++l)
	{
		memcpy(U2, U, width>>1);
		memcpy(V2, V, width>>1);

		U2 = U2 + (width>>1);
		V2 = V2 + (width>>1);

		U = U + (width);
		V = V + (width);
	}

	memcpy(Y2, Y, width*height);
	free(tem_buf);
}