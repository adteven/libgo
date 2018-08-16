#pragma once
#include "../../libgo/libgo.h"
#include <thread>
#include "gtest/gtest.h"

#ifndef TEST_MIN_THREAD
#define TEST_MIN_THREAD 4
#endif

#ifndef TEST_MAX_THREAD
#define TEST_MAX_THREAD 4
#endif

struct startScheduler {
    startScheduler(co::Scheduler & scheduler) {
        pThread = new std::thread([&]{
                scheduler.Start(TEST_MIN_THREAD, TEST_MAX_THREAD);
            });
    }
    std::thread *pThread;
};

startScheduler __startScheduler(g_Scheduler);

inline void __WaitUntilNoTask(co::Scheduler & scheduler, int line, std::size_t val = 0) {
    int i = 0;
    while (scheduler.TaskCount() > val) {
        usleep(1000);
        if (++i == 5000) {
            printf("LINE: %d, TaskCount: %d\n", line, (int)g_Scheduler.TaskCount());
        }
    }
}

#define WaitUntilNoTask() __WaitUntilNoTask(g_Scheduler, __LINE__)
#define WaitUntilNoTaskN(n) __WaitUntilNoTask(g_Scheduler, __LINE__, n)

#define WaitUntilNoTaskS(scheduler) __WaitUntilNoTask(scheduler, __LINE__)

inline void DumpTaskCount() {
    printf("TaskCount: %d\n", (int)g_Scheduler.TaskCount());
}

template <typename Clock>
struct GTimerT {
    GTimerT() : tp_(Clock::now()) {}

    void reset() {
        tp_ = Clock::now();
    }

    typename Clock::duration duration() {
        return Clock::now() - tp_;
    }

    int ms() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                Clock::now() - tp_).count();
    }

private:
    typename Clock::time_point tp_;
};
typedef GTimerT<co::FastSteadyClock> GTimer;

#define DEFAULT_DEVIATION 50
#define TIMER_CHECK(t, val, deviation) \
        do { \
            auto c = t.ms(); \
            EXPECT_GT(c, val - 2); \
            EXPECT_LT(c, val + deviation); \
        } while (0)

#ifdef _WIN32
#include <stdlib.h>
struct exit_pause {
	~exit_pause()
	{
		system("pause");
	}
} g_exit;
#endif
