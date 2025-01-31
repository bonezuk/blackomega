#pragma once

//#define USE_STDC20 1

#ifdef USE_STDC20

#include <semaphore>
using semaphore_t = std::binary_semaphore;

#else 

#include <mutex>
#include <condition_variable>

class semaphore_t {
	std::mutex m_mtx;
	std::condition_variable m_cv;
	std::ptrdiff_t m_cnt;
public:
	semaphore_t(std::ptrdiff_t desired) {
		m_cnt = desired;
	}
	semaphore_t(const semaphore_t& sem) = delete;
	semaphore_t(semaphore_t&& sem) = delete;
	semaphore_t operator=(const semaphore_t& sem) = delete;
	void acquire() {
		std::unique_lock<std::mutex> lock(m_mtx);
		while (!m_cnt) {
			m_cv.wait(lock);
		}
		m_cnt--;
	}
	bool try_acquire() {
		std::lock_guard<std::mutex> lock(m_mtx);
		if (m_cnt) {
			m_cnt--;
			return true;
		}
		return false;
	}
	void release() {
		std::lock_guard<std::mutex> lock(m_mtx);
		m_cnt++;
		m_cv.notify_one();
	}
};

#endif
