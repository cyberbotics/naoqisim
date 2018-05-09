#ifndef LOW_PASS_FILTER_HPP
#define LOW_PASS_FILTER_HPP

// Description: Simulate an electronical low pass filter
// Source:      http://en.wikipedia.org/wiki/Lowpass_filter

class LowPassFilter {
public:
  LowPassFilter(double timeInterval, double RCTimeConstant);

  void appendRawValue(double value);
  double filteredValue() const { return mCurrentFilteredValue; }

private:
  double mCurrentFilteredValue;
  double mCurrentRawValue;
  double mPreviousFilteredValue;

  double mAlpha;
  double mOneMinusAlpha;
  int mSize;
};

#endif
