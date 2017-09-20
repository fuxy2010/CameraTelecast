// **********************************************************************
// 作者: 傅佑铭
// 版本: 1.0
// 日期: 2011-01 ~ 2011-11
// 修改历史记录: 
// 日期, 作者, 变更内容
// **********************************************************************
#ifndef _SCHEDULE_SERVER_H_       
#define _SCHEDULE_SERVER_H_

#include "stdafx.h"
#include "GeneralDef.h"
#include <boost/pool/detail/singleton.hpp>//#include "singleton.h"
#include "Logger.h"
#include "MiscTool.h"
#include "ModuleManager.h"
#include "ConfigBox.h"
//#include "IDeviceAccess.h"
#include "TaskThread.h"
#include "UserAgent.h"
#include "JRTPSession.h"
//#include "RTSPServer.h"
//#include "VUA.h"
//#include "ICommunicator.h"
#include "RegisterThread.h"
//#include "UARenderThread.h"
#include "PCMPlayer.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

//#define VIDEO_SAMPLE_INTERVAL	50
//#define VIDEO_WIDTH				640
//#define VIDEO_HEIGHT				480

namespace ScheduleServer
{
	class CScheduleServer
	{
	public:
		CScheduleServer();

		~CScheduleServer();

		SS_Error start(std::string path);

		SS_Error shutdown();

		void write_log(std::string& log, int level, bool show_on_screen);

		void wait_for_shutdown();

		void set_cur_path(const std::string& path) { _cur_path = path; };

		std::string get_cur_path() { return _cur_path; };

		//ICommunicator* get_communicator() { return _communicator; };

		CRTPNATSession* get_audio_send_session() { return _audio_send_session; };

		CRTPNATSession* get_video_send_session() { return _video_send_session; };

		//RTSPServerLib::CRTSPServer* get_rtsp_server() { return _rtsp_server; };

		void singleton_test();
		void h264_test();
		
		static void print_contex(const SS_CTX& ctx, const bool to_send);

		static void record_pcm(unsigned long id, void* buffer)
		{
			string file = MiscTools::parse_type_to_string<unsigned long>(id) + ".pcm";

			FILE* f = fopen(file.c_str(), "ab+");
			fwrite(buffer, 1, 960, f);
			fclose(f);

			//cout << file << ", ";
		}

		//开启服务
		void enable() { _enalble = true; }

		//关闭服务
		void disable()
		{
			_enalble = false;
		}

		std::string get_server_ip()
		{
			return _server_ip;
		}

		unsigned short get_server_port(unsigned long reference)
		{
			if(true == _server_recv_port_vec.empty())
				return 0;

			return _server_recv_port_vec[reference % _server_recv_port_vec.size()];
		}

	public:
		std::map<unsigned long, bool> _is_ua_idle;

	private:
		std::string _server_ip;
		std::vector<unsigned short> _server_recv_port_vec;


	public:
		static void CALLBACK on_recv_rtsp_server_data(unsigned char* data, int length, int type, unsigned long nSequence, unsigned long user_data);

		static void CALLBACK on_recv_rtp_packet(const unsigned char* data, const unsigned long& length,
											const unsigned short& sequence, const unsigned long& timestamp,
											const unsigned long& ssrc, const unsigned char&payload_type, const bool& mark);
		static void CALLBACK on_recv_rtcp_packet();

		//接收NAT探测包 
		static void CALLBACK on_recv_nat_probe(const char* content, const unsigned long& length, const unsigned long& sender_ip, const unsigned short& sender_port);

	private:
		volatile bool _enalble;//服务是否可用

		//程序所在文件夹绝对路径
		std::string _cur_path;

		//ICommunicator* _communicator;//ICE通讯器

		std::map<unsigned long, CUserAgent*> _ua_map;
		CSSMutex _ua_map_mutex;

		CRTPRecvSession** _rtp_recv_session;//媒体接收RTP Session的指针数组
		unsigned short _rtp_recv_thread_num;//媒体接收RTP Session的数量
		unsigned short _rtp_recv_base_port;//媒体接收RTP Session的监听基准端口

		CRTPNATSession* _audio_send_session;//发送音频数据的RTP session
		CRTPNATSession* _video_send_session;//发送音频数据的RTP session

		//RTSPServerLib::CRTSPServer* _rtsp_server;

		CRegisterThread* _register_thread;

		//CUARenderThread* _ua_render_thread;

		unsigned long _playing_ua_id;

		CTask* _video_task;
		CTask* _audio_task;

	public:
		CRegisterThread* get_register_thread() { return _register_thread; }

	public:
		//增加任务
		//index为0则是随机选择任务线程，否则选择index模任务线程数取余的任务线程
		SS_Error add_task(CTask* task, unsigned long index);

		//UA登记
		SS_Error reg_ua(const unsigned long& id, USER_AGENT_TYPE type)
		{
			if(!id)
				return SS_NoErr;

			CSSLocker lock(&_ua_map_mutex);

			if(_ua_map.end() == _ua_map.find(id))
			{
				//第一次注册
				USER_AGENT_INFO info;
				info.id = id;
				info.type = type;

				CUserAgent* ua = new CUserAgent(info);
				//ua->update_alive_timestamp();
				_ua_map[ua->_info.id] = ua;
			}
			else
			{
				//已注册过
				CUserAgent* ua = _ua_map[id];

				if(NULL == ua)
					return SS_NoErr;

				ua->_info.type = type;
			}

			return SS_NoErr;
		}

		//获取UA指针
		inline CUserAgent* fetch_ua(const unsigned long& id)
		{
			CSSLocker lock(&_ua_map_mutex);

			if(!id || _ua_map.end() == _ua_map.find(id))
				return NULL;

			return _ua_map[id];
		}

		//获取UA指针
		CUserAgent* fetch_ua(const string& id)
		{
			return fetch_ua(MiscTools::parse_string_to_type<unsigned long>(id));
		}

		//统计并记录UA媒体数据
		void ua_statistics()
		{
		}

		//根据ID从_ua_map中删除UA同时销毁UA
		void remove_ua(const unsigned long& id)
		{
			CUserAgent* ua = NULL;

			{
				CSSLocker lock(&_ua_map_mutex);

				ua = _ua_map[id];
				_ua_map.erase(id);
			}
			
			delete ua;
			ua = NULL;
		}

		//删除全部UA
		void remove_all_ua()
		{
			CSSLocker lock(&_ua_map_mutex);

			for(auto iter = _ua_map.begin(); iter != _ua_map.end(); ++iter)
			{
				delete iter->second;
				iter->second = NULL;
			}

			_ua_map.clear();
		}

		//所有虚拟UA向SIP服务器注册并发送NAT探测包
		void ua_register_and_nat()
		{
			CSSLocker lock(&_ua_map_mutex);

			for(auto iter = _ua_map.begin(); iter != _ua_map.end(); ++iter)
			{
				//NAT探测
				//iter->second->nat_probe(SINGLETON(CConfigBox).get_property("ScheduleServerIP", "127.0.0.1"));

				//SIP注册
				//到期前会自动重新注册 iter->second->sip_register();
			}
		}

		void restart_sip_ua_register()
		{
			if(NULL == _register_thread)
				return;

			_register_thread->restart();
		}

		//所有UA发送注册消息
		//void ua_nat_probe();

		//根据refercnece选择服务器接收某个UA的媒体数据的端口
		void select_media_recv_port(const unsigned long& refercnece, unsigned short& audio_port, unsigned short& video_port)
		{
			//audio_port和video_port不能相同
			audio_port = _rtp_recv_base_port + 10 * (refercnece % _rtp_recv_thread_num);
			video_port = _rtp_recv_base_port + 10 * ((refercnece + 1) % _rtp_recv_thread_num);

			return;
		}

		int start_rtmp_push();

		int stop_rtmp_push();

	public:
		void set_update_vua_timestamp()
		{
			_update_vua_timestamp = ::time(NULL);
		}

		unsigned long get_update_vua_timestamp()
		{
			return _update_vua_timestamp;
		}

	private:
		unsigned long _update_vua_timestamp;

	};
}

#endif//_SCHEDULE_SERVER_H_