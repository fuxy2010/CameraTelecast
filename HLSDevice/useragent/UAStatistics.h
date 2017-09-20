// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#ifndef _UA_STATISTICS_H_
#define _UA_STATISTICS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "MMSystem.h"

//ÿ10000����ͳ��һ�ζ�����
#ifndef PACKET_LOST_RATE_INTERVAL
#define PACKET_LOST_RATE_INTERVAL	500
#endif

//ÿ100����ͳ��һ�ΰ�������ƽ��ֵ
#ifndef PACKET_AVERAGE_TIMESTAMP_INTERVAL
#define PACKET_AVERAGE_TIMESTAMP_INTERVAL	100
#endif

namespace ScheduleServer
{
	class CUAStatistics
	{
	private:
		//ͳ�������Ƿ�δ��ʼ��
		bool _data_initialized;

		//�Ƿ���ͣͳ��
		bool _pause;

		//ͳ����������
		int _media_type;//0-audio, 1-video

	public:
		CUAStatistics::CUAStatistics(int media_type)
		{
			_media_type = media_type;
			_data_initialized = false;
			_pause = true;

			//����ͳ��
			_PACKET_LOST_RATE_STATISTICS_INTERVAL = 15000;//ÿ10��ͳ��һ�ζ�����
			_latest_packet_sequence = 0;//���һ���յ������ݰ������
			_lost_packet_count = 0;//����ͳ�����ۼƶ�����
			_packet_count_for_lost_rate_statistics = 0;//����ͳ�����յ��İ���
			_packet_lost_rate = 0.0;//����ͳ�ƵĶ����ʣ��ٷ���
			_latest_lost_rate_statistics_timestamp = 0;//�ϴ�ͳ��ʱ��

			//��������ͳ��
			_PACKET_ARRIVAL_TIMESTAMP_INTERVAL_STATISTICS_INTERVAL = 60000;//ÿ60��ͳ��һ�ΰ�������ƽ��ֵ
			_latest_packet_arrival_timestamp = 0;;//���һ���յ������ݰ������ʱ��
			_accumulative_packet_arrival_timestamp_interval = 0;//����ͳ�����ۼ����ݰ���������
			_packet_count_for_packet_arrival_timestamp_interval = 0;//����ͳ�����յ��İ���
			_average_packet_arrival_timestamp_interval = 0.0;//����ͳ�Ƶİ�����ʱ�����ƽ��ֵ
			_latest_packet_arrival_interval_statistics_timestamp = 0;//�ϴ�ͳ�����ʱ��

			//��ʱͳ��
			_PACKET_DELAY_STATISTICS_INTERVAL = 15000;//ÿ10��ͳ��һ�ΰ������ӳ�
			_latest_packet_timestamp = 0;;//���һ���յ������ݰ���ʱ��
			_latest_packet_relative_timestamp = 0;//���һ�����ݰ������ʱ����������ʱ��)
			_accumulative_packet_timestamp_delay = 0;//����ͳ�����ܼ����ݰ��ӳ�
			_packet_count_for_packet_delay_statistics = 0;//����ͳ�����յ��İ���
			_average_packet_delay = 0.0;//����ͳ�Ƶİ��ӳ�ƽ��ֵ�����룩
			_latest_packet_delay_statistics_timestamp = 0;//�ϴ�ͳ�����ʱ��

			//����ͳ��
			_BITRATE_STATISTICS_INTERVAL = 15000;//ÿ10��ͳ��һ������
			_accumulative_packet_length = 0;//����ͳ�����ܼ����ݰ�����
			_average_bitrate = 0.0;//����ͳ�Ƶ�����(kbps)
			_latest_bitreate_statistics_timestamp = 0;//�ϴ�ͳ�����ʱ��
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
		//����ͳ��
		unsigned long _PACKET_LOST_RATE_STATISTICS_INTERVAL;//ÿ10��ͳ��һ�ζ�����
		unsigned short _latest_packet_sequence;//���һ���յ������ݰ������
		unsigned long _lost_packet_count;//����ͳ�����ۼƶ�����
		unsigned long _packet_count_for_lost_rate_statistics;//����ͳ�����յ��İ���
		double _packet_lost_rate;//����ͳ�ƵĶ����ʣ��ٷ���
		unsigned long _latest_lost_rate_statistics_timestamp;//�ϴ�ͳ��ʱ��

		void packet_lost_rate_statistics(unsigned short sequence)
		{
			//�����ۼƶ�����//////////////////////////////////////////////////////////////////////////
			if(sequence > _latest_packet_sequence)//�������
			{
				_lost_packet_count += (sequence - _latest_packet_sequence -1);
			}
			else
			{
				//�������������Ϊ����ŵ���ֵ�����ߺ����
				//ͬʱ�ų��Ҳ������������
				if(0xfff < (_latest_packet_sequence - sequence))
				{
					_lost_packet_count += (sequence + (0xffff - _latest_packet_sequence) -1);
				}
			}

			_latest_packet_sequence = sequence;
			_packet_count_for_lost_rate_statistics++;

			//���㶪����//////////////////////////////////////////////////////////////////////////
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
		//��������ͳ��
		unsigned long _PACKET_ARRIVAL_TIMESTAMP_INTERVAL_STATISTICS_INTERVAL;//ÿ60��ͳ��һ�ΰ�������ƽ��ֵ
		unsigned long _latest_packet_arrival_timestamp;//���һ���յ������ݰ������ʱ��
		unsigned long _accumulative_packet_arrival_timestamp_interval;//����ͳ�����ۼ����ݰ���������
		unsigned long _packet_count_for_packet_arrival_timestamp_interval;//����ͳ�����յ��İ���
		double _average_packet_arrival_timestamp_interval;//����ͳ�Ƶİ�����ʱ�����ƽ��ֵ
		unsigned long _latest_packet_arrival_interval_statistics_timestamp;//�ϴ�ͳ�����ʱ��
		
		void packet_arrival_interval_statistics()
		{
			//�����ۼư�����ʱ����//////////////////////////////////////////////////////////////////////////
			long timestamp = ::timeGetTime();

			//������ʱ������ֵ�����޵İ�
			if(timestamp >= _latest_packet_arrival_timestamp)
			{
				_accumulative_packet_arrival_timestamp_interval += timestamp - _latest_packet_arrival_timestamp;

				_packet_count_for_packet_arrival_timestamp_interval++;
			}

			_latest_packet_arrival_timestamp = timestamp;		

			//���������ʱ��ƽ��ֵ//////////////////////////////////////////////////////////////////////////
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
		//��ʱͳ��
		unsigned long _PACKET_DELAY_STATISTICS_INTERVAL;//ÿ10��ͳ��һ�ΰ������ӳ�
		unsigned long _latest_packet_timestamp;//���һ���յ������ݰ���ʱ��
		unsigned long _latest_packet_relative_timestamp;//���һ�����ݰ������ʱ����������ʱ��)
		long _accumulative_packet_timestamp_delay;//����ͳ�����ܼ����ݰ��ӳ�
		unsigned long _packet_count_for_packet_delay_statistics;//����ͳ�����յ��İ���
		double _average_packet_delay;//����ͳ�Ƶİ��ӳ�ƽ��ֵ�����룩
		unsigned long _latest_packet_delay_statistics_timestamp;//�ϴ�ͳ�����ʱ��	
		
		void packet_delay_statistics(unsigned long packet_timestamp, bool mark)
		{
			//ֻ����markΪtrue����Ƶ��
			//��Ƶ��ȫ������
			if(1 == _media_type && false == mark)
			{
				return;
			}

			long timestamp = ::timeGetTime();

			//���¿�ʼͳ��ʱ
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
				//��packet_timestamp����ֵ�����޻�תʱ�������ۼ����ݰ��ӳ�
				_latest_packet_timestamp = packet_timestamp;
				_latest_packet_relative_timestamp = timestamp;

				return;
			}

			//�����ܼ����ݰ��ӳ�
			_accumulative_packet_timestamp_delay += (timestamp - _latest_packet_relative_timestamp) - (packet_timestamp - _latest_packet_timestamp);

			_latest_packet_relative_timestamp = timestamp;
			_latest_packet_timestamp = packet_timestamp;

			_packet_count_for_packet_delay_statistics++;

			//�����ӳ�//////////////////////////////////////////////////////////////////////////
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
		//����ͳ��
		unsigned long _BITRATE_STATISTICS_INTERVAL;//ÿ10��ͳ��һ������
		unsigned long _accumulative_packet_length;//����ͳ�����ܼ����ݰ�����
		double _average_bitrate;//����ͳ�Ƶ�����(kbps)
		unsigned long _latest_bitreate_statistics_timestamp;//�ϴ�ͳ�����ʱ��

		void bitrate_statistics(unsigned long packet_length)
		{
			//���¿�ʼͳ��ʱ
			if(0 == _latest_bitreate_statistics_timestamp)
			{
				_latest_bitreate_statistics_timestamp = ::timeGetTime();
				_accumulative_packet_length = 0;
			}

			//�������ݰ��ܼƳ���
			_accumulative_packet_length += packet_length;

			//��������//////////////////////////////////////////////////////////////////////////
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
		//�յ����ݰ������
		void update_packet_statistics(unsigned short sequence, unsigned long packet_timestamp, bool mark, unsigned long packet_length)
		{
			if(true == _pause)
				return;

			//�Ƿ�տ�ʼͳ��δ��ʼ������//////////////////////////////////////////////////////////////////////////
			if(false == _data_initialized)
			{
				//���������
				_latest_packet_sequence = sequence;
				_packet_count_for_lost_rate_statistics = 1;//�����1��ʼ
				_lost_packet_count = 0;
				_latest_lost_rate_statistics_timestamp = 0;//�ϴ�ͳ��ʱ��


				//�����������
				_latest_packet_arrival_timestamp = ::timeGetTime();
				_packet_count_for_packet_arrival_timestamp_interval = 0;//�����0��ʼ
				_latest_packet_arrival_interval_statistics_timestamp = 0;
				_accumulative_packet_arrival_timestamp_interval = 0;			

				//���ӳ����
				_latest_packet_timestamp = 0;
				_latest_packet_relative_timestamp = 0;
				_accumulative_packet_timestamp_delay = 0;
				_packet_count_for_packet_delay_statistics = 0;//�����0��ʼ
				_latest_packet_delay_statistics_timestamp = 0;

				//�������
				_accumulative_packet_length = 0;
				_latest_bitreate_statistics_timestamp = 0;

				_data_initialized = true;

				return;
			}

			//������ͳ��
			packet_lost_rate_statistics(sequence);

			//��������ʱ��ͳ��
			packet_arrival_interval_statistics();

			//���ݰ��ӳ�ͳ��
			packet_delay_statistics(packet_timestamp, mark);

			//ͳ������
			bitrate_statistics(packet_length);
		}
	};

	class CVideoRelayStatistics
	{
	public:
		CVideoRelayStatistics::CVideoRelayStatistics()
		{
			//��Ƶת�����ͳ��
			_RELAY_STATISTICS_INTERVAL = 5;//ÿ5��ͳ��һ��ת�����
			_accumulative_relay_time = 0;//����ͳ�����ܼ�ת�����
			_packet_count_for_relay_statistics = 0;//����ͳ�����յ������ݰ���
			_average_relay = 0.0;//����ͳ��ƽ��ת�����
			_latest_relay_statistics_timestamp = 0;//�ϴ�ͳ�����ʱ��
		}

		virtual CVideoRelayStatistics::~CVideoRelayStatistics()
		{

		}

		double get_relay_interval()
		{
			return _average_relay;
		}
	
	private:
		//��Ƶת�����ͳ��
		unsigned long _RELAY_STATISTICS_INTERVAL;//ÿ5��ͳ��һ��ת�����
		unsigned long _accumulative_relay_time;//����ͳ�����ܼ�ת�����
		unsigned long _packet_count_for_relay_statistics;//����ͳ�����յ������ݰ���
		double _average_relay;//����ͳ��ƽ��ת�����
		unsigned long _latest_relay_statistics_timestamp;//�ϴ�ͳ�����ʱ��

	public:
		void relay_statistics(unsigned long in_timestamp, unsigned long out_timestamp)
		{
			//��ʼͳ��ʱ
			if(0 == _latest_relay_statistics_timestamp)
			{
				_latest_relay_statistics_timestamp = ::timeGetTime();
				_accumulative_relay_time = 0;
				_packet_count_for_relay_statistics = 0;
			}

			//�������ݰ��ܼƳ���
			if(out_timestamp >= in_timestamp)
			{
				_accumulative_relay_time += (out_timestamp - in_timestamp);
				_packet_count_for_relay_statistics++;
			}
			else
			{
				return;
			}

			//��������//////////////////////////////////////////////////////////////////////////
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