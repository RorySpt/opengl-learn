#pragma once
#include <chrono>
#include <deque>
#include <mutex>
class FrameRateCalculator
{
	
public:
	//fgt_series_max Ϊ-1�Ǽ�¼����֡����ʱ������
	FrameRateCalculator(
		double	fgta_sampling_time = 1.0,
		double	fra_sampling_time = 1.0,
		int		fgt_series_max = -1
	);
	FrameRateCalculator(FrameRateCalculator&) = delete;

	void tick(std::chrono::nanoseconds);
	void tick(double sec);
	void tick();

	//ƽ��֡����ʱ��
	[[nodiscard]] std::chrono::nanoseconds fgt() const;	//֡����ʱ��
	[[nodiscard]] std::chrono::nanoseconds fgta() const;//֡ƽ������ʱ��

	[[nodiscard]] double fr_sec() const;	//˲ʱ֡��
	[[nodiscard]] double fra_sec() const;	//ƽ��֡��
	[[nodiscard]] double fgt_sec() const;	//֡����ʱ��
	[[nodiscard]] double fgta_sec() const;	//֡ƽ������ʱ��

	[[nodiscard]] double fps() const;		//ƽ��֡��
private:
	void update_time();
	void reset();
private:
	std::deque<double> m_fgt_series;		// ֡����ʱ���¼
	double		m_fgt_series_total;
	const int	m_fgt_series_limit;		// ֡����ʱ���¼��������

	std::chrono::high_resolution_clock::time_point m_time_point;
	std::chrono::high_resolution_clock::time_point m_last_time_point;

	bool	m_is_updated_in_tick;
	double	m_fps;					// ֡��
	int		m_fps_frame_count;		// fps֡����
	double	m_fgt;					// ֡ƽ��ʱ��
	int		m_fgt_frame_count;		// fgt֡����

	const double m_fps_sampling_time_limit;
	const double m_fgt_sampling_time_limit;

	double	m_fps_sampling_time_cur;
	double	m_fgt_sampling_time_cur;
};

