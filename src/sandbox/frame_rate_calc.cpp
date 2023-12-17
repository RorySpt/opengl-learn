#include "stdafx.h"
#include "frame_rate_calc.h"


FrameRateCalculator::FrameRateCalculator(double fgta_sampling_time, double fra_sampling_time, int fgt_series_max)
	: m_fgt_series_total(0.0)
	, m_fgt_series_limit(fgt_series_max)
	, m_is_updated_in_tick(false)
	, m_fps(0.0), m_fps_frame_count(0)
	, m_fgt(0.0), m_fgt_frame_count(0)
	, m_fps_sampling_time_limit(fra_sampling_time)
	, m_fgt_sampling_time_limit(fgta_sampling_time)
	, m_fps_sampling_time_cur(0.0)
	, m_fgt_sampling_time_cur(0.0)
{

}

void FrameRateCalculator::tick(std::chrono::nanoseconds elapse_nanoseconds)
{
	update_time();

	const double elapse_sec = static_cast<double>(elapse_nanoseconds.count()) / 1.e9;
	tick(elapse_sec);

	m_is_updated_in_tick = false;
}

void FrameRateCalculator::tick(double sec)
{
	update_time();
	if (sec > 0)
	{
		if (m_fgt_series_limit == -1 || m_fgt_series.size() < m_fgt_series_limit)
		{
			m_fgt_series.push_back(sec);
		}
		else if (m_fgt_series_limit != 0)
		{
			m_fgt_series.pop_front();
			m_fgt_series.push_back(sec);
		}
		m_fps_sampling_time_cur += sec;
		m_fgt_sampling_time_cur += sec;
		++m_fps_frame_count;
		++m_fgt_frame_count;
		if (m_fps_sampling_time_cur >= m_fps_sampling_time_limit)
		{
			m_fps = m_fps_frame_count / m_fps_sampling_time_cur;
			m_fps_sampling_time_cur = 0;
			m_fps_frame_count = 0;
		}
		if (m_fgt_sampling_time_cur >= m_fgt_sampling_time_limit)
		{
			m_fgt = m_fgt_sampling_time_cur / m_fgt_frame_count;
			m_fgt_sampling_time_cur = 0;
			m_fgt_frame_count = 0;
		}
	}

	m_is_updated_in_tick = false;
}

void FrameRateCalculator::tick()
{
	update_time();

	if (m_last_time_point != std::chrono::high_resolution_clock::time_point())
	{
		//标记更新
		tick(m_time_point - m_last_time_point);
	}


	m_is_updated_in_tick = false;
}

void FrameRateCalculator::reset()
{
	m_fgt_series.clear();
}



std::chrono::nanoseconds FrameRateCalculator::fgt() const
{
	if (m_fgt_series_limit != 0&&!m_fgt_series.empty())
	{
		return std::chrono::nanoseconds(static_cast<int>(m_fgt_series.back() * 1e9));
	}
	else
	{
		return m_time_point - m_last_time_point;
	}
}

std::chrono::nanoseconds FrameRateCalculator::fgta() const
{
	return std::chrono::nanoseconds(static_cast<int>(fgta_sec() * 1e9));
}

double FrameRateCalculator::fr_sec() const
{
	if (m_fgt_series_limit != 0&&!m_fgt_series.empty())
	{
		return 1 / m_fgt_series.back();
	}else
	{
		return 1e9 / static_cast<int>((m_time_point - m_last_time_point).count());
	}
}

double FrameRateCalculator::fra_sec() const
{
	return m_fps;
}

double FrameRateCalculator::fgt_sec() const
{
	return 1.0/fr_sec();
}

double FrameRateCalculator::fgta_sec() const
{
	return m_fgt;
}

double FrameRateCalculator::fps() const
{
	return fra_sec();
}

void FrameRateCalculator::update_time()
{
	if (m_is_updated_in_tick)
		return;

	m_last_time_point = m_time_point;
	m_time_point = std::chrono::high_resolution_clock::now();
	m_is_updated_in_tick = true;
}
