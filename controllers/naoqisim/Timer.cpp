#include "Timer.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <stdlib.h>
#include <sys/time.h>
#endif

Timer::Timer() {
  reset();
}

double Timer::delta() {
  double t = time();
  double delta = t - beforeTime;
  return delta;
}

void Timer::reset() {
  beforeTime = time();
}

double Timer::time() const {
#ifdef _WIN32
  SYSTEMTIME time;
  GetSystemTime(&time);
  WORD millis = (time.wSecond * 1000) + time.wMilliseconds;
#else
  timeval time;
  gettimeofday(&time, NULL);
  long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
#endif
  return (double) millis / 1000.0;
}
