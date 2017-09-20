// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#include "WUACodec.h"
#include "TimeConsuming.h"

using namespace ScheduleServer;

CWUACodec::CWUACodec() :
_encoder(NULL),
_decoder(NULL),
samplesInput(0),
maxBytesOutput(0),
totalBytesWritten(0)
{
	//Encode//////////////////////////////////////////////////////////////////////////
	_encoder = wuaEncOpen(44100, 1, &samplesInput, &maxBytesOutput);

	//��ȡ��ǰ����
	wuaEncConfigurationPtr myFormat;
	myFormat = wuaEncGetCurrentConfiguration(_encoder);
	myFormat->wuaObjectType = MAIN;	
	myFormat->bitRate = 32000 ;
	myFormat->bandWidth = 22050;
	myFormat->quantqual = 100;
	myFormat->inputFormat = WUACINPUT_16BIT;
	//���õ�ǰ����
	wuaEncSetConfiguration(_encoder, myFormat);

	//Decode//////////////////////////////////////////////////////////////////////////
	//�����׸����ݰ�ʱ��ʼ��������
}

CWUACodec::~CWUACodec()
{
	wuaEncClose(_encoder);
	wuaDecClose(_decoder);
}

int CWUACodec::encode(short* frame, unsigned char* bits)
{
	//CTimeConsuming tc('E', 1.0);
	CSSLocker lock(&_encode_mutext);

	return wuaEncEncode(_encoder, frame, 1024, bits, maxBytesOutput);
}

int CWUACodec::decode(unsigned char* bits, short* frame, int crc)
{
	CSSLocker lock(&_decode_mutext);

	return 0;
}

unsigned long CWUACodec::decode2(unsigned char* bits, unsigned long bits_len, void* frame)
{
	//CTimeConsuming tc('C', 1.0);

	CSSLocker lock(&_decode_mutext);

	if(NULL == _decoder)
	{
		//�����׸����ݰ�ʱ��ʼ��������
		_decoder = wuaDecOpen();

		config = wuaDecGetCurrentConfiguration(_decoder);
		config->defObjectType = 2;
		config->outputFormat = 1;
		config->downMatrix = 0;
		config->useOldADTSFormat = 0;
		config->dontUpSampleImplicitSBR = 1;
		wuaDecSetConfiguration(_decoder, config);

		wuaDecInit(_decoder, bits, bits_len, &samplerate, &channels);
	}

	void* buf = wuaDecDecode(_decoder, &frameInfo, bits, bits_len);

	if(NULL == frame || !frameInfo.samples)
		return 0;

	::memcpy(frame, buf, 2 * frameInfo.samples);//frameInfo.samplesֵΪ��short�ͼ����֡��

	return 2 * frameInfo.samples;
}

int CWUACodec::calculate_energy(unsigned char* bits, int len)
{
	return 0;
}
