#ifndef TIMER_HPP
#define TIMER_HPP

// Description: Class helping to measure delta time in milliseconds

class Timer {
public:
  // constructor & destructor
  Timer();
  virtual ~Timer() {}

  double delta(); // second
  void reset();

private:
  double time() const; // second

  double beforeTime;
};

#endif
