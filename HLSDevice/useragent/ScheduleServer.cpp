// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
// ScheduleServer.cpp : Defines the entry point for the console application.
//
#include "ScheduleServer.h"
#include "CommandContextParse.h"
//#include "ICECommunicator.h"
#include "MediaData.h"
//#include "json.h"
#include "TimeConsuming.h"
#include "resource.h"
#include "VUASendMediaTask.h"

using namespace ScheduleServer;

double CTimeConsuming::_clock_frequency = 0;

CScheduleServer::CScheduleServer() :
_cur_path(""),
_enalble(false),
_rtp_recv_session(NULL),
_rtp_recv_thread_num(0),
_rtp_recv_base_port(0),
_audio_send_session(NULL),
_video_send_session(NULL),
//_rtsp_server(NULL),
_register_thread(NULL),
//_ua_render_thread(NULL),
_update_vua_timestamp(0),
_playing_ua_id(0),
_audio_task(NULL),
_video_task(NULL)
{
	_ua_map.clear();
}

CScheduleServer::~CScheduleServer()
{
}

SS_Error CScheduleServer::start(std::string path)
{
	//���õ�ǰ����·��
	_cur_path = path;

	//����ϵͳʱ�����������
	::srand(timeGetTime());

	//��ȡ��������Ϣ
	//_server_ip = SINGLETON(CConfigBox).get_property("ScheduleServerIP", "127.0.0.1");

	std::vector<string> port;
	SINGLETON(CConfigBox).get_properties_with_prefix("ServerRecvPort", port);

	_server_recv_port_vec.clear();
	for(auto iter = port.begin(); iter != port.end(); ++iter)
	{
		_server_recv_port_vec.push_back(MiscTools::parse_string_to_type<unsigned short>(*iter));
	}

	//��ʼ��socket
	//for JRTP
	WSADATA dat;
	WSAStartup(MAKEWORD(2,2),&dat);

	//��ȡϵͳ��Ϣ
	SYSTEM_INFO theSystemInfo;
	::GetSystemInfo(&theSystemInfo);

	//���������߳�////////////////////////////////////////////////////////////////////////
	CTaskThreadPool::add_threads((!(theSystemInfo.dwNumberOfProcessors) ? 1 : theSystemInfo.dwNumberOfProcessors), this);

	_enalble = true;//�������

	return SS_NoErr;

}

SS_Error CScheduleServer::shutdown()
{
	_enalble = false;

	//�ر�UA�����߳�
	//_ua_render_thread->shutdown();
	//delete _ua_render_thread;
	//_ua_render_thread = NULL;

	//�ر�ά���߳�
	//_register_thread->shutdown();
	//delete _register_thread;
	//_register_thread = NULL;

	//�ر������߳�////////////////////////////////////////////////////////////////////////
	CTaskThreadPool::remove_threads();

	LOG_WRITE("Task threads have been removed!", 1, true);

	//ɾ������UA////////////////////////////////////////////////////////////////////////
	remove_all_ua();

	//�ر�socket////////////////////////////////////////////////////////////////////////
	WSACleanup();//for JRTP

	return SS_NoErr;
}

void CScheduleServer::write_log(std::string& log, int level, bool show_on_screen)
{
	SINGLETON(CLogger).trace_out(log, level, DEFAULT_TRACE_TYPE, show_on_screen);
}

SS_Error CScheduleServer::add_task(CTask* task, unsigned long index)
{
	if(false == _enalble)
		return SS_AddTaskFail;

	if(NULL  == task)
		return SS_InvalidTask;

	CTaskThread* task_thread = CTaskThreadPool::select_thread(index);

	if(NULL == task_thread)
		return SS_AddTaskFail;

	if(SS_NoErr != task_thread->add_task(task))
	{
		return SS_AddTaskFail;
	}

	return SS_NoErr;
}

void CScheduleServer::on_recv_rtsp_server_data(unsigned char* data, int length, int type, unsigned long nSequence, unsigned long user_data)
{
	RTPHeader* header = reinterpret_cast<RTPHeader*>(data);

	on_recv_rtp_packet(data, length,
					ntohs(header->sequencenumber),
					ntohl(header->timestamp), 
					user_data,//ntohl(header->ssrc),
					header->payloadtype, header->marker);
}

//dataΪ����ͷ��RTP����lengthΪ���ɼӰ�ͷ����
void CScheduleServer::on_recv_rtp_packet(const unsigned char* data, const unsigned long& length,
	const unsigned short& sequence, const unsigned long& timestamp,
	const unsigned long& ssrc, const unsigned char&payload_type, const bool& mark)
{
}

void CScheduleServer::on_recv_rtcp_packet()
{

}

//����NAT̽��� 
void CScheduleServer::on_recv_nat_probe(const char* content, const unsigned long& length, const unsigned long& sender_ip, const unsigned short& sender_port)
{
}


int CScheduleServer::start_rtmp_push()
{
	reg_ua(1, UA_MobilePhone);

	CUserAgent* ua = SINGLETON(CScheduleServer).fetch_ua(1);
	
	int ret = -1;
	if(NULL != ua)
	{
		ret = ua->rtmp_connect("rtmp://127.0.0.1:1935/hls/test");
		//ret = ua->rtmp_connect("rtmp://121.41.15.6/hls/123");
		//ret = ua->rtmp_connect("rtmp://121.41.15.6:1935/live/camera");
		//ret = ua->rtmp_connect("rtmp://121.41.15.6:2935/hls/camera");
		//ret = ua->rtmp_connect("rtmp://192.168.0.123:2935/hls/camera");
		if(!ret)
			ua->_start_hls = true;
	}

	VUA_SEND_MEDIA_TASK_INFO task_info;

#if 1
	task_info;
	task_info.task_id = 1;
	task_info.ua_id = 1;
	task_info.send_audio = false;

	//����ģ��UA��������Ƶ����
	_video_task = new CVUASendMediaTask(task_info);

	if(SS_NoErr != SINGLETON(CScheduleServer).add_task(_video_task, task_info.task_id))
	{
		delete _video_task;
		_video_task = NULL;
	}
#endif

#if 1
	task_info.task_id = 2;
	task_info.ua_id = 1;
	task_info.send_audio = true;

	//����ģ��UA��������Ƶ����
	_audio_task = new CVUASendMediaTask(task_info);

	if(SS_NoErr != SINGLETON(CScheduleServer).add_task(_audio_task, task_info.task_id))
	{
		delete _audio_task;
		_audio_task = NULL;
	}
#endif

	return ret;
}

int CScheduleServer::stop_rtmp_push()
{
	if(NULL != _audio_task)
		dynamic_cast<CVUASendMediaTask*>(_audio_task)->close();

	if(NULL != _video_task)
		dynamic_cast<CVUASendMediaTask*>(_video_task)->close();

	CUserAgent* ua = SINGLETON(CScheduleServer).fetch_ua(1);

	int ret = -1;
	if(NULL != ua)
	{
		ua->_start_hls = false;
		ret = ua->disconnect();
	}

	//remove_all_ua();

	return ret;
}

void CScheduleServer::singleton_test()
{
	cout << "<ScheduleServer: " << reinterpret_cast<unsigned long>(this) << "> ";
}

void CScheduleServer::h264_test()
{
	FILE* f = fopen("cif.yuv", "rb");
	const int width = 352;
	const int height = 288;

	fseek(f, 0, SEEK_SET);

	unsigned char stream_buf[655360];
	int nal_len[1024];

	long handle = H264EncodeInit(width, height, 30, 1000, 30, 1024, 15);

	while(true)
	{
		int length = width * height * 3 / 2;

		unsigned char yuv[width * height * 3 / 2];
		::memset(yuv, 0, sizeof(yuv));

		if(length != fread(yuv, sizeof(unsigned char), length, f))
		{
			fseek(f, 0, SEEK_SET);
			continue;
		}

		memset(stream_buf, 0, sizeof(stream_buf));
		memset(nal_len, 0, sizeof(nal_len));

		int i_nal = H264EncodeFrame(handle, -1, yuv, stream_buf, nal_len);

		TRACE("\nnal: %d", i_nal);
	}
}

void CScheduleServer::print_contex(const SS_CTX& ctx, const bool to_send)
{
	std::string content = "";

	for(SS_CTX::const_iterator iter = ctx.begin();
		iter != ctx.end();
		++iter)
	{
		content += iter->first;
		content += ",";
		content += iter->second;
		content += ";\r\n";
	}

	if(false == to_send)
	{
		LOG_WRITE("\n<<<<<< Receive ICE request >>>>>>\n" + content, 1, true);
	}
	else
	{
		LOG_WRITE("\n<<<<<< Send ICE request >>>>>>\n" + content, 1, true);
	}
}
