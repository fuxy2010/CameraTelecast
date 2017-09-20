#include "PCMReader.h"
#include "string.h"
#include "UserAgent.h"
#include "ScheduleServer.h"

using namespace ScheduleServer;

CPcmReader::CPcmReader(): m_bOpend(false), m_fpFile(0), audio_buf_index(0), _pause(true)
{	
}


CPcmReader::~CPcmReader()
{
}


int CPcmReader::Open(const char* pcsFileName)
{
	return 0;
}


void CPcmReader::Close()
{
}

int CPcmReader::Read(BYTE* sDataBuf, int iDataLen)
{
	if(false == _pause)
	{
		if(!audio_buf_index)
		{
			/*RAW_AUDIO_FRAME_PTR audio_frame_ptr;

			CUserAgent* ua = SINGLETON(CScheduleServer).fetch_ua(SINGLETON(CScheduleServer).get_playing_ua_id());

			if(NULL == ua)
				return 0;

			//audio_frame_ptr = ua->fetch_raw_audio_frame();

			if(NULL == audio_frame_ptr.frame)
				return 0;

			::memcpy(audio_buf, audio_frame_ptr.frame->payload, iDataLen * 3);

			audio_buf_index = 3;

			CMemPool::free_raw_audio_frame(audio_frame_ptr);*/
		}
	}
	else
	{
		::memset(audio_buf, 0, iDataLen * 3);
		audio_buf_index = 3;
	}

	::memcpy(sDataBuf, audio_buf + (3 - audio_buf_index--) * iDataLen, iDataLen);

	return 0;
}