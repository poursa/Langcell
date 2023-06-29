#pragma once
#include <thread>
#include <condition_variable>
#include <mutex>
#include <deque>
#include <functional>
#include <iostream>

inline constexpr int TEX_THREADS = 10;

class TexThreadPool
{
public:
	static inline size_t pool_size = TEX_THREADS;
	static inline std::thread thread_pool[TEX_THREADS];
	static inline std::mutex mutex_pool[TEX_THREADS];
	static inline std::condition_variable cond_var;

	static inline std::deque<std::function<void()>> work_queue;
	static inline std::mutex popmutex;
	static inline std::atomic<int> work_done = 0;
	static inline std::atomic<bool> finish = false;
	
	~TexThreadPool() {
		finish = true;
		
		for (auto& mut : mutex_pool) {
			mut.unlock();
			
		}
		
		for (auto& thr : thread_pool) {
			cond_var.notify_all();
			if (popmutex.try_lock())
			{
				popmutex.unlock();
			}
			if (thr.joinable()) thr.join();
		}
	}

	TexThreadPool() {
		stop_running();
		for (int i = 0; i < TEX_THREADS; i++) {
			thread_pool[i] = std::thread([this, i](){
				while (!finish) {
					std::unique_lock<std::mutex> lock{mutex_pool[i]};
					cond_var.wait(lock);
					{
						std::lock_guard<std::mutex> pop_lock{popmutex};
						if (!work_queue.empty()) {
							std::function<void()> work = std::move(work_queue.front());
							work_queue.pop_front();
							
							work();
							work_done--;
						}
						
					}
					lock.unlock();
				}
				});
		}
	}
	inline void add_work(std::function<void(int, int)> work, int hbegin, int hend) {
		work_queue.push_back([work = std::move(work), hbegin, hend]() { work(hbegin, hend); });
		work_done++;
	}

	inline void wait_to_finish() {
		while(work_done != 0){
			cond_var.notify_all();
		}
		stop_running();
	}

	inline void start_running() {
		for (auto& mut : mutex_pool) {
			mut.unlock();
		}
	}

	inline void stop_running() {
		for (auto& mut : mutex_pool) {
			mut.lock();
		}
	}

};

