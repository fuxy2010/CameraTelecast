#ifndef _PCM_READER_H_
#define _PCM_READER_H_
#include "stdio.h"

enum
{
	PCM_FRAME_LEN=320,
}; // 20ms pcm data len

class CPcmReader
{
	typedef unsigned char BYTE;
public:
	CPcmReader();
	~CPcmReader();
public:
	int  Open(const char* pcsFileName); 
	void Close();
	int  Read(BYTE* sDataBuf, int iDataLen); 
	/*
	return 1 : the end of the file  
	return 0 : read ok
	return -1: read fail     */
private:
	bool  m_bOpend;
	FILE* m_fpFile;

	unsigned char audio_buf[960];
	int audio_buf_index;

private:
	bool _pause;
public:
	void pause() { _pause = true; }
	void resume() { _pause = false; }
};


#endif