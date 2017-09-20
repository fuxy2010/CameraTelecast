#include "PCMPlayer.h"

const GUID IID_IDirectSoundNotify={0xb0210783, 0x89cd, 0x11d0, {0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16}};  
const GUID IID_IDirectSoundBuffer={0x279AFA85, 0x4981, 0x11CE, {0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60}};

CPCMPlayer::CPCMPlayer(): m_pReader(NULL), m_pDsPlayObj(NULL), m_pDsBuffer(NULL), m_hThread(NULL), m_wStatus(STA_NULL)//, _pause(true)
{
	m_pReader = new CPcmReader;
	// create event object
	int i = 0;
	for (i = 0; i < DS_PLAY_BUF_SEG_CNT + 1; i++) 
	{
		m_sPlayEvent[i] = CreateEvent(NULL, false, false, NULL);
	}
	// create 
	HRESULT hr = DirectSoundCreate(NULL, &m_pDsPlayObj, NULL);
	if (hr != DS_OK)
	{
		printf("create ds play obj fail!!!\n");
		return;
	}
	HWND hWnd = GetForegroundWindow();
	if (hWnd == NULL)
		hWnd = GetDesktopWindow();
	//设置设备的协作度
	hr = m_pDsPlayObj->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	if (hr != DS_OK)
	{
		printf("set cooperative level fail!!!\n");
		return;
	}

	PCMWAVEFORMAT pcmwf = {0};
	DSBUFFERDESC dsbdesc = {0};
	//设置PCMWAVEFORMAT结构
	memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
	pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
	pcmwf.wf.nChannels = 1;
	pcmwf.wf.nSamplesPerSec = 8000;
	pcmwf.wf.nBlockAlign = 2;
	pcmwf.wf.nAvgBytesPerSec = 16000;
	pcmwf.wBitsPerSample = 16;	
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); //将结构置0
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY |DSBCAPS_GETCURRENTPOSITION2;
	dsbdesc.dwBufferBytes = DS_PLAY_BUF_SIZE;
	dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;

	//创建辅助缓冲区对象
	hr = m_pDsPlayObj->CreateSoundBuffer(&dsbdesc, &m_pDsBuffer, NULL);
	if (hr != DS_OK){
		printf("create sound buffer fail!!!\n");
		return;
	}
	LPDIRECTSOUNDNOTIFY lpDSBNotify;
	hr = m_pDsBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpDSBNotify); 
	if (hr != DS_OK)
	{
		printf("query interface fail!!!\n");
		return;
	}
	DSBPOSITIONNOTIFY* pPosNotify = new DSBPOSITIONNOTIFY[DS_PLAY_BUF_SEG_CNT];
	DWORD nPos = PCM_FRAME_LEN - 1;
	for (i = 0; i < DS_PLAY_BUF_SEG_CNT; i++)
	{
		pPosNotify[i].dwOffset = nPos;
		pPosNotify[i].hEventNotify = m_sPlayEvent[i + 1];
		nPos += PCM_FRAME_LEN;
	}
	hr = lpDSBNotify->SetNotificationPositions(DS_PLAY_BUF_SEG_CNT, pPosNotify);
	if (pPosNotify)
	{
		delete[] pPosNotify;
		pPosNotify = NULL;
	}
}



CPCMPlayer::~CPCMPlayer()
{
	//Stop();
}



int CPCMPlayer::Start(const char* pcsFileName)
{
//	if (!pcsFileName)
	//	return -1;
	if (m_pReader->Open(pcsFileName) != 0)
	{
		printf("Ds play file fail!!!");
		return -1;
	}

	// 创建播放线程
	m_hThread = CreateThread(NULL, NULL, ThreadCallBack, this, 0, 0);
	//if (m_hThread == NULL) 
		//return false;
	m_wStatus = STA_PALYING;
	for (int i = 0; i < DS_PLAY_BUF_SEG_CNT; i++)
		WriteDataToPlayDevice(i + 1);
	m_pDsBuffer->Play(0, 0, DSBPLAY_LOOPING);
	return 0;
}

void CPCMPlayer::Stop()
{
	if (m_pDsBuffer)
		m_pDsBuffer->Stop();

	if (m_hThread != NULL) 
	{
		SetEvent(m_sPlayEvent[0]);
		WaitForSingleObject(m_sPlayEvent, INFINITE);
		CloseHandle(m_hThread);
	}
	if (m_pDsBuffer) 
	{
		m_pDsBuffer->Release();
		m_pDsBuffer = 0;
	}	
	if (m_pDsPlayObj)
	{
		m_pDsPlayObj->Release();
		m_pDsPlayObj = 0;
	}
}


DWORD CPCMPlayer::ThreadCallBack(void* pParam)
{
	CPCMPlayer* pDsPlayer = (CPCMPlayer*)(pParam);
	pDsPlayer->ThreadProc();
	return 0;
}


void CPCMPlayer::ThreadProc()
{
	for(;;)
	{
		/*if(true == _pause)
		{
			Sleep(10);
			continue;
		}*/

		DWORD iRet = WaitForMultipleObjects(DS_PLAY_BUF_SEG_CNT + 1, m_sPlayEvent, FALSE, INFINITE);
		
		if (iRet >= WAIT_OBJECT_0 && iRet <= WAIT_OBJECT_0 + DS_PLAY_BUF_SEG_CNT + 1)
		{
			if (iRet == WAIT_OBJECT_0)
				break;
			WriteDataToPlayDevice(iRet - WAIT_OBJECT_0);
		}
	}
	
	if (m_pDsBuffer) // 播放结束停止
		m_pDsBuffer->Stop();
	printf("Ds play thread over!!!");
}


#include <iostream>
void CPCMPlayer::WriteDataToPlayDevice(int iSegIndex)
{
	//std::cout << iSegIndex << " ";

	LPVOID lpvPtr1, lpvPtr2;
	DWORD dwBytes1, dwBytes2;	
	lpvPtr1 = 0;
	HRESULT iRet;
	iRet = m_pDsBuffer->Lock((iSegIndex - 1)*DS_PLAY_BUF_SEG_SIZE, DS_PLAY_BUF_SEG_SIZE, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
	if (DSERR_BUFFERLOST == iRet)
	{
		m_pDsBuffer->Restore();
		iRet = m_pDsBuffer->Lock((iSegIndex - 1)*DS_PLAY_BUF_SEG_SIZE, DS_PLAY_BUF_SEG_SIZE, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
	}
	if (iRet != DS_OK)
		return;
	if (lpvPtr1 != NULL)
	{ //读文件
		iRet = m_pReader->Read((BYTE*)lpvPtr1, dwBytes1);
		if (iRet == 1)
		{ // 停止
			m_wStatus = STA_STOP;
			SetEvent(m_sPlayEvent[0]);
			return;
		}
	}
	if (lpvPtr2 != NULL)
	{
		iRet = m_pReader->Read((BYTE*)lpvPtr2, dwBytes2);
		if (iRet == 1)
		{ // 停止
			m_wStatus = STA_STOP;
			SetEvent(m_sPlayEvent[0]);
			return;
		}
	}
	m_pDsBuffer->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);	
}


