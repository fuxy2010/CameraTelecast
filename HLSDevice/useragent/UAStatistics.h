// **********************************************************************
// 作者: 傅佑铭
// 版本: 1.0
// 日期: 2011-01 ~ 2011-11
// 修改历史记录: 
// 日期, 作者, 变更内容
// **********************************************************************
#ifndef _UA_STATISTICS_H_
#define _UA_STATISTICS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "MMSystem.h"

//每10000个包统计一次丢包率
#ifndef PACKET_LOST_RATE_INTERVAL
#define PACKET_LOST_RATE_INTERVAL	500
#endif

//每100个包统计一次包到达间隔平均值
#ifndef PACKET_AVERAGE_TIMESTAMP_INTERVAL
#define PACKET_AVERAGE_TIMESTAMP_INTERVAL	100
#endif

namespace ScheduleServer
{
	class CUAStatistics
	{
	private:
		//统计数据是否未初始化
		bool _data_initialized;

		//是否暂停统计
		bool _pause;

		//统计数据类型
		int _media_type;//0-audio, 1-video

	public:
		CUAStatistics::CUAStatistics(int media_type)
		{
			_media_type = media_type;
			_data_initialized = false;
			_pause = true;

			//丢包统计
			_PACKET_LOST_RATE_STATISTICS_INTERVAL = 15000;//每10秒统计一次丢包率
			_latest_packet_sequence = 0;//最近一次收到的数据包的序号
			_lost_packet_count = 0;//本轮统计中累计丢包数
			_packet_count_for_lost_rate_statistics = 0;//本轮统计中收到的包数
			_packet_lost_rate = 0.0;//本轮统计的丢包率，百分制
			_latest_lost_rate_statistics_timestamp = 0;//上次统计时戳

			//包到达间隔统计
			_PACKET_ARRIVAL_TIMESTAMP_INTERVAL_STATISTICS_INTERVAL = 60000;//每60秒统计一次包到达间隔平均值
			_latest_packet_arrival_timestamp = 0;;//最近一次收到的数据包到达的时戳
			_accumulative_packet_arrival_timestamp_interval = 0;//本轮统计中累计数据包包到达间隔
			_packet_count_for_packet_arrival_timestamp_interval = 0;//本轮统计中收到的包数
			_average_packet_arrival_timestamp_interval = 0.0;//本轮统计的包到达时戳间隔平均值
			_latest_packet_arrival_interval_statistics_timestamp = 0;//上次统计完毕时戳

			//延时统计
			_PACKET_DELAY_STATISTICS_INTERVAL = 15000;//每10秒统计一次包到达延迟
			_latest_packet_timestamp = 0;;//最近一次收到的数据包的时戳
			_latest_packet_relative_timestamp = 0;//最近一次数据包的相对时戳（即到达时戳)
			_accumulative_packet_timestamp_delay = 0;//本轮统计中总计数据包延迟
			_packet_count_for_packet_delay_statistics = 0;//本轮统计中收到的包数
			_average_packet_delay = 0.0;//本轮统计的包延迟平均值（毫秒）
			_latest_packet_delay_statistics_timestamp = 0;//上次统计完毕时戳

			//流量统计
			_BITRATE_STATISTICS_INTERVAL = 15000;//每10秒统计一次流量
			_accumulative_packet_length = 0;//本轮统计中总计数据包长度
			_average_bitrate = 0.0;//本轮统计的码率(kbps)
			_latest_bitreate_statistics_timestamp = 0;//上次统计完毕时戳
		}

		virtual CUAStatistics::~CUAStatistics()
		{

		}

		void pause()
		{
			_pause = true;
		}

		void resume()
		{
			_pause = false;

			_latest_bitreate_statistics_timestamp = _latest_packet_delay_statistics_timestamp = _latest_packet_arrival_interval_statistics_timestamp = _latest_lost_rate_statistics_timestamp = ::timeGetTime();

			reset();
		}

		void reset()
		{
			_data_initialized = false;
		}

		double get_packet_lost_rate()
		{
			return (false == _pause) ? _packet_lost_rate : 0.0;
		}

		double get_averate_packet_timestamp_interval()
		{
			return (false == _pause) ? _average_packet_arrival_timestamp_interval : 0.0;
		}

		double get_packet_delay()
		{
			return (false == _pause) ? _average_packet_delay : 0.0;
		}

		double get_bitrate()
		{
			return (false == _pause) ? _average_bitrate : 0.0;
		}

	private:
		//丢包统计
		unsigned long _PACKET_LOST_RATE_STATISTICS_INTERVAL;//每10秒统计一次丢包率
		unsigned short _latest_packet_sequence;//最近一次收到的数据包的序号
		unsigned long _lost_packet_count;//本轮统计中累计丢包数
		unsigned long _packet_count_for_lost_rate_statistics;//本轮统计中收到的包数
		double _packet_lost_rate;//本轮统计的丢包率，百分制
		unsigned long _latest_lost_rate_statistics_timestamp;//上次统计时戳

		void packet_lost_rate_statistics(unsigned short sequence)
		{
			//更新累计丢包数//////////////////////////////////////////////////////////////////////////
			if(sequence > _latest_packet_sequence)//正常情况
			{
				_lost_packet_count += (sequence - _latest_packet_sequence -1);
			}
			else
			{
				//序号相差过大则认为是序号到达值域上线后归零
				//同时排除且不考虑乱序情况
				if(0xfff < (_latest_packet_sequence - sequence))
				{
					_lost_packet_count += (sequence + (0xffff - _latest_packet_sequence) -1);
				}
			}

			_latest_packet_sequence = sequence;
			_packet_count_for_lost_rate_statistics++;

			//计算丢包率//////////////////////////////////////////////////////////////////////////
			//if(PACKET_LOST_RATE_INTERVAL <= _packet_count_for_lost)
			if(_PACKET_LOST_RATE_STATISTICS_INTERVAL <= (::timeGetTime() - _latest_lost_rate_statistics_timestamp))
			{
				if(0 != _packet_count_for_lost_rate_statistics)
				{
					_packet_lost_rate = 100.0 * ((double)_lost_packet_count / (double)(_packet_count_for_lost_rate_statistics + _lost_packet_count));
				}

				_lost_packet_count = 0;
				_packet_count_for_lost_rate_statistics = 0;

				_latest_lost_rate_statistics_timestamp = ::timeGetTime();
			}
		}

	private:
		//包到达间隔统计
		unsigned long _PACKET_ARRIVAL_TIMESTAMP_INTERVAL_STATISTICS_INTERVAL;//每60秒统计一次包到达间隔平均值
		unsigned long _latest_packet_arrival_timestamp;//最近一次收到的数据包到达的时戳
		unsigned long _accumulative_packet_arrival_timestamp_interval;//本轮统计中累计数据包包到达间隔
		unsigned long _packet_count_for_packet_arrival_timestamp_interval;//本轮统计中收到的包数
		double _average_packet_arrival_timestamp_interval;//本轮统计的包到达时戳间隔平均值
		unsigned long _latest_packet_arrival_interval_statistics_timestamp;//上次统计完毕时戳
		
		void packet_arrival_interval_statistics()
		{
			//更新累计包到达时间间隔//////////////////////////////////////////////////////////////////////////
			long timestamp = ::timeGetTime();

			//不考虑时戳到达值域上限的包
			if(timestamp >= _latest_packet_arrival_timestamp)
			{
				_accumulative_packet_arrival_timestamp_interval += timestamp - _latest_packet_arrival_timestamp;

				_packet_count_for_packet_arrival_timestamp_interval++;
			}

			_latest_packet_arrival_timestamp = timestamp;		

			//计算包到达时间平均值//////////////////////////////////////////////////////////////////////////
			//if(PACKET_AVERAGE_TIMESTAMP_INTERVAL <= _packet_count_for_interval)
			if(_PACKET_ARRIVAL_TIMESTAMP_INTERVAL_STATISTICS_INTERVAL <= (::timeGetTime() - _latest_packet_arrival_interval_statistics_timestamp))
			{
				if(0 != _packet_count_for_packet_arrival_timestamp_interval)
				{
					_average_packet_arrival_timestamp_interval = ((double)_accumulative_packet_arrival_timestamp_interval / (double)(_packet_count_for_packet_arrival_timestamp_interval));
				}

				_accumulative_packet_arrival_timestamp_interval = 0;
				_packet_count_for_packet_arrival_timestamp_interval = 0;

				_latest_packet_arrival_interval_statistics_timestamp = ::timeGetTime();
			}
		}

	private:
		//延时统计
		unsigned long _PACKET_DELAY_STATISTICS_INTERVAL;//每10秒统计一次包到达延迟
		unsigned long _latest_packet_timestamp;//最近一次收到的数据包的时戳
		unsigned long _latest_packet_relative_timestamp;//最近一次数据包的相对时戳（即到达时戳)
		long _accumulative_packet_timestamp_delay;//本轮统计中总计数据包延迟
		unsigned long _packet_count_for_packet_delay_statistics;//本轮统计中收到的包数
		double _average_packet_delay;//本轮统计的包延迟平均值（毫秒）
		unsigned long _latest_packet_delay_statistics_timestamp;//上次统计完毕时戳	
		
		void packet_delay_statistics(unsigned long packet_timestamp, bool mark)
		{
			//只计算mark为true的视频包
			//音频包全部计算
			if(1 == _media_type && false == mark)
			{
				return;
			}

			long timestamp = ::timeGetTime();

			//重新开始统计时
			if(0 == _latest_packet_timestamp && 0 == _latest_packet_relative_timestamp)
			{
				_latest_packet_timestamp = packet_timestamp;
				_latest_packet_relative_timestamp = timestamp;			
				_latest_packet_delay_statistics_timestamp = timestamp;
				_packet_count_for_packet_delay_statistics = 0;

				return;
			}

			if(packet_timestamp <= _latest_packet_timestamp)
			{
				//当packet_timestamp到达值域上限回转时不计算累计数据包延迟
				_latest_packet_timestamp = packet_timestamp;
				_latest_packet_relative_timestamp = timestamp;

				return;
			}

			//更新总计数据包延迟
			_accumulative_packet_timestamp_delay += (timestamp - _latest_packet_relative_timestamp) - (packet_timestamp - _latest_packet_timestamp);

			_latest_packet_relative_timestamp = timestamp;
			_latest_packet_timestamp = packet_timestamp;

			_packet_count_for_packet_delay_statistics++;

			//计算延迟//////////////////////////////////////////////////////////////////////////
			if(_PACKET_DELAY_STATISTICS_INTERVAL <= (::timeGetTime() - _latest_packet_delay_statistics_timestamp))
			{
				if(0 != _packet_count_for_packet_delay_statistics)
				{
					_average_packet_delay = ((double)_accumulative_packet_timestamp_delay / (double)_packet_count_for_packet_delay_statistics);

					//Log.v("Media", ((0 == _media_type) ? "A" : "V") + "D: " + _average_packet_delay + " C: " + _packet_count_for_packet_delay_statistics);
				}

				_latest_packet_delay_statistics_timestamp = ::timeGetTime();

				_accumulative_packet_timestamp_delay = 0;
				_latest_packet_relative_timestamp = 0;
				_latest_packet_timestamp = 0;
				_packet_count_for_packet_delay_statistics = 0;
			}
		}

	private:
		//流量统计
		unsigned long _BITRATE_STATISTICS_INTERVAL;//每10秒统计一次流量
		unsigned long _accumulative_packet_length;//本轮统计中总计数据包长度
		double _average_bitrate;//本轮统计的码率(kbps)
		unsigned long _latest_bitreate_statistics_timestamp;//上次统计完毕时戳

		void bitrate_statistics(unsigned long packet_length)
		{
			//重新开始统计时
			if(0 == _latest_bitreate_statistics_timestamp)
			{
				_latest_bitreate_statistics_timestamp = ::timeGetTime();
				_accumulative_packet_length = 0;
			}

			//更新数据包总计长度
			_accumulative_packet_length += packet_length;

			//计算流量//////////////////////////////////////////////////////////////////////////
			if(_BITRATE_STATISTICS_INTERVAL <= (::timeGetTime() - _latest_bitreate_statistics_timestamp))
			{
				//_average_bitrate = ((double)(_accumulative_packet_length * 1000) / (double)(128 * (::timeGetTime() - _latest_bitreate_statistics_timestamp)));
				_average_bitrate = ((double)(_accumulative_packet_length * 125) / (double)(16 * _BITRATE_STATISTICS_INTERVAL));

				//Log.v("Media", ((0 == _media_type) ? "A" : "V") + "B: " + _average_bitrate + "kbps");

				_accumulative_packet_length = 0;
				_latest_bitreate_statistics_timestamp = ::timeGetTime();
			}
		}

	public:
		//收到数据包后更新
		void update_packet_statistics(unsigned short sequence, unsigned long packet_timestamp, bool mark, unsigned long packet_length)
		{
			if(true == _pause)
				return;

			//是否刚开始统计未初始化数据//////////////////////////////////////////////////////////////////////////
			if(false == _data_initialized)
			{
				//丢包率相关
				_latest_packet_sequence = sequence;
				_packet_count_for_lost_rate_statistics = 1;//必须从1开始
				_lost_packet_count = 0;
				_latest_lost_rate_statistics_timestamp = 0;//上次统计时戳


				//包到达间隔相关
				_latest_packet_arrival_timestamp = ::timeGetTime();
				_packet_count_for_packet_arrival_timestamp_interval = 0;//必须从0开始
				_latest_packet_arrival_interval_statistics_timestamp = 0;
				_accumulative_packet_arrival_timestamp_interval = 0;			

				//包延迟相关
				_latest_packet_timestamp = 0;
				_latest_packet_relative_timestamp = 0;
				_accumulative_packet_timestamp_delay = 0;
				_packet_count_for_packet_delay_statistics = 0;//必须从0开始
				_latest_packet_delay_statistics_timestamp = 0;

				//流量相关
				_accumulative_packet_length = 0;
				_latest_bitreate_statistics_timestamp = 0;

				_data_initialized = true;

				return;
			}

			//丢包率统计
			packet_lost_rate_statistics(sequence);

			//包到达间隔时间统计
			packet_arrival_interval_statistics();

			//数据包延迟统计
			packet_delay_statistics(packet_timestamp, mark);

			//统计流量
			bitrate_statistics(packet_length);
		}
	};

	class CVideoRelayStatistics
	{
	public:
		CVideoRelayStatistics::CVideoRelayStatistics()
		{
			//视频转发间隔统计
			_RELAY_STATISTICS_INTERVAL = 5;//每5秒统计一次转发间隔
			_accumulative_relay_time = 0;//本轮统计中总计转发间隔
			_packet_count_for_relay_statistics = 0;//本轮统计中收到的数据包数
			_average_relay = 0.0;//本轮统计平均转发间隔
			_latest_relay_statistics_timestamp = 0;//上次统计完毕时戳
		}

		virtual CVideoRelayStatistics::~CVideoRelayStatistics()
		{

		}

		double get_relay_interval()
		{
			return _average_relay;
		}
	
	private:
		//视频转发间隔统计
		unsigned long _RELAY_STATISTICS_INTERVAL;//每5秒统计一次转发间隔
		unsigned long _accumulative_relay_time;//本轮统计中总计转发间隔
		unsigned long _packet_count_for_relay_statistics;//本轮统计中收到的数据包数
		double _average_relay;//本轮统计平均转发间隔
		unsigned long _latest_relay_statistics_timestamp;//上次统计完毕时戳

	public:
		void relay_statistics(unsigned long in_timestamp, unsigned long out_timestamp)
		{
			//开始统计时
			if(0 == _latest_relay_statistics_timestamp)
			{
				_latest_relay_statistics_timestamp = ::timeGetTime();
				_accumulative_relay_time = 0;
				_packet_count_for_relay_statistics = 0;
			}

			//更新数据包总计长度
			if(out_timestamp >= in_timestamp)
			{
				_accumulative_relay_time += (out_timestamp - in_timestamp);
				_packet_count_for_relay_statistics++;
			}
			else
			{
				return;
			}

			//计算流量//////////////////////////////////////////////////////////////////////////
			if(_RELAY_STATISTICS_INTERVAL <= (::timeGetTime() - _latest_relay_statistics_timestamp))
			{
				if(_packet_count_for_relay_statistics)
				{
					_average_relay = (double)_accumulative_relay_time / (double)_packet_count_for_relay_statistics;
				}

				_accumulative_relay_time = 0;
				_packet_count_for_relay_statistics = 0;

				_latest_relay_statistics_timestamp = ::timeGetTime();
			}
		}
	};
}

#endif//_UA_STATISTICS_H_