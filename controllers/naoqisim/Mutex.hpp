#ifndef MUTEX_HPP
#define MUTEX_HPP

// Description: Class that implements a platform independent mutex

#ifdef _WIN32
#include "windows.h"
#else
#include "pthread.h"
#endif

class Mutex {
public:
  Mutex();
  virtual ~Mutex();
  void lock();
  void unlock();
private:
#ifdef _WIN32
  HANDLE mutex;
#else
  pthread_mutex_t *mutex;
#endif
};

#endif
