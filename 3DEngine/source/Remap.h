//***************************************************************************
// Filename: Remap.h
// Author  : Coleman Jonas
// Description: Use this as a basic layout for all c++ files in the proj.
//***************************************************************************

#ifndef REMAP_H
#define REMAP_H


template<typename T>
//remaps the custom value in the range range low1 to high1 
//to 0 to 1 of the value type
T RemapBasicRange(const T& low1, const T& high1, const T& value)
{
  return (value - low1) / (high1 - low1);
}

template<typename T>
//remaps the custom value in the range range low1 to high1 
//to 0 to 1 of the value
float RemapRange(const T& low1, const T& high1, const T& value)
{
  return float((value - low1) / (high1 - low1));
}

template<typename T, typename F>
/*remaps the value in from the custom range low1 to high1 to low2 to high2

such as RemapCustomRange(0, 50, 1, 100, 0.5f) = 75

remap finds the ratio of the value inside of the low1 to high1 ratio and applies it
to the range low2 to high2 and finds the relative value
*/
F RemapCustomRange(const T& low1, const F& low2, const T& high1, const F& high2, const T& value)
{
  return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

template<typename T>
//remaps the value of its range form low to high 1 to a percentage, 
//essentially linearlly mapping the value to the percentage terms
float RemapPercentRange(const T& low1, const T& high1, const T& value)
{
  return float(((value - low1) * 100.0f) / (high1 - low1));
}


#endif