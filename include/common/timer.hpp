#pragma once
#include <chrono>
#include <sys/time.h>


namespace mirrors {

unsigned GetMillisCount();

class Timer {
public:
    Timer(int millis) {
        end_millis_ = GetMillisCount() + millis;
    }

    template <class Rep, class Period>
    Timer(std::chrono::duration<Rep, Period> duration)
            : Timer(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) {}

    int left() const {
        return end_millis_ - GetMillisCount();
    }

    bool timeout() const {
        return GetMillisCount() >= end_millis_;
    }

private:
    int end_millis_;
};

inline unsigned GetMillisCount() {
#ifdef WINDOWS
    return GetTickCount();
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    return unsigned((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
#endif
}

}