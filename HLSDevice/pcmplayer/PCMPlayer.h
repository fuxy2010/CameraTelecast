#ifndef _DIRECTSOUND_PLAY_H_
#define _DIRECTSOUND_PLAY_H_
#include "windows.h"
#include <mmsystem.h>
#include <dsound.h>
#pragma comment(lib, "Dsound.lib")
#include "pcmreader.h"

//必须大于* 2语音才清晰
#define DS_PLAY_BUF_SIZE (PCM_FRAME_LEN * 3)
#define DS_PLAY_BUF_SEG_SIZE (PCM_FRAME_LEN)
#define DS_PLAY_BUF_SEG_CNT (DS_PLAY_BUF_SIZE/DS_PLAY_BUF_SEG_SIZE)

class CPcmReader;


class CPCMPlayer
{
public:
	CPCMPlayer();
	~CPCMPlayer();
public:
	int  Start(const char* pcsFileName);
	void Stop();
protected:
	static DWORD CALLBACK ThreadCallBack(void* pParam); // 回调函数
	void ThreadProc(); // 线程处理
	HANDLE m_hThread;  // 线程句柄
protected:
	void WriteDataToPlayDevice(int iSegIndex);
private:
	enum {STA_NULL, STA_PALYING, STA_STOP};
	WORD m_wStatus;
	CPcmReader* m_pReader;
	enum{DS_BUFFER_CNT=2,};
	LPDIRECTSOUND m_pDsPlayObj;
	LPDIRECTSOUNDBUFFER m_pDsBuffer;  //声明缓冲区指针
	HANDLE m_sPlayEvent[DS_PLAY_BUF_SEG_CNT+1];

private:
	//bool _pause;
public:
	void pause()
	{
		//_pause = true;

		if(NULL != m_pReader)
			m_pReader->pause();
	}

	void resume()
	{
		//_pause = false;

		if(NULL != m_pReader)
			m_pReader->resume();
	}
};

#endif