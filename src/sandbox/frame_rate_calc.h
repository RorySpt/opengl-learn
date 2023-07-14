#pragma once
#include <chrono>
#include <deque>
#include <mutex>
class FrameRateCalculator
{
	
public:
	//fgt_series_max 为-1是记录所有帧生成时间数据
	FrameRateCalculator(
		double	fgta_sampling_time = 1.0,
		double	fra_sampling_time = 1.0,
		int		fgt_series_max = -1
	);
	FrameRateCalculator(FrameRateCalculator&) = delete;

	void tick(std::chrono::nanoseconds);
	void tick(double sec);
	void tick();

	//平均帧生成时间
	[[nodiscard]] std::chrono::nanoseconds fgt() const;	//帧生成时间
	[[nodiscard]] std::chrono::nanoseconds fgta() const;//帧平均生成时间

	[[nodiscard]] double fr_sec() const;	//瞬时帧率
	[[nodiscard]] double fra_sec() const;	//平均帧率
	[[nodiscard]] double fgt_sec() const;	//帧生成时间
	[[nodiscard]] double fgta_sec() const;	//帧平均生成时间

	[[nodiscard]] double fps() const;		//平均帧率
private:
	void update_time();
	void reset();
private:
	std::deque<double> m_fgt_series;		// 帧生成时间记录
	double		m_fgt_series_total;
	const int	m_fgt_series_limit;		// 帧生成时间记录限制上限

	std::chrono::high_resolution_clock::time_point m_time_point;
	std::chrono::high_resolution_clock::time_point m_last_time_point;

	bool	m_is_updated_in_tick;
	double	m_fps;					// 帧率
	int		m_fps_frame_count;		// fps帧计数
	double	m_fgt;					// 帧平均时间
	int		m_fgt_frame_count;		// fgt帧计数

	const double m_fps_sampling_time_limit;
	const double m_fgt_sampling_time_limit;

	double	m_fps_sampling_time_cur;
	double	m_fgt_sampling_time_cur;
};

