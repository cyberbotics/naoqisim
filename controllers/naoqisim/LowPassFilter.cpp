#include "LowPassFilter.hpp"

static bool custom_isnan(double d) {
  return d != d;
}

LowPassFilter::LowPassFilter(double timeInterval, double RCTimeConstant) :
  mCurrentFilteredValue(0.0),
  mCurrentRawValue(0.0),
  mPreviousFilteredValue(0.0),
  mSize(0)
{
  mAlpha = timeInterval / (RCTimeConstant + timeInterval);
  mOneMinusAlpha = 1.0 - mAlpha;
}

void LowPassFilter::appendRawValue(double value) {
  mPreviousFilteredValue = mCurrentFilteredValue;

  mCurrentRawValue = value;

  if (mSize > 0)
    mCurrentFilteredValue = mAlpha * mCurrentRawValue + mOneMinusAlpha * mPreviousFilteredValue;
  else if (! custom_isnan(value)) // ! NaN + mSize == 0
    mCurrentFilteredValue = mCurrentRawValue;
  else // NaN case
    mCurrentFilteredValue = 0.0;

  mSize++;
}
