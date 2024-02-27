#ifndef time_helpers_hpp
#define time_helpers_hpp

#include <chrono>
#include <thread>
#include <cassert>

/* There is some debate over which clock to use, as different platforms
have different properties for the clock. High resolution clock appears
to work best across most platforms, despite not being steady (monotonic).
*/
using clock_for_now = std::chrono::high_resolution_clock;

/* Returns the time in seconds since some stating point. */
inline double now()
{
    return std::chrono::nanoseconds{clock_for_now::now().time_since_epoch()}.count() * 1e-9;
}

/* Wait for at least the given number of seconds*/
inline void wait_for(double t)
{
    std::this_thread::sleep_for(std::chrono::nanoseconds{int64_t(t*1e9)});
}

/* Wait until now() <= t*/
inline void wait_until(double t)
{
    auto target=clock_for_now::time_point() + std::chrono::nanoseconds(int64_t(t*1e9));

    std::this_thread::sleep_until(target);
    assert(now() <= t);
}

#endif