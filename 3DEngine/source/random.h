//***************************************************************************
// Filename: random.h
// Author  : Coleman Jonas
// Description: Random 
//***************************************************************************

#ifndef RANDOM_H
#define RANDOM_H
#include <cstdlib>
#include <ctime>

//seeds initial random value
inline void RandomInit()
{
  //std::srand(int('SEED'));//seeding value
  //std::srand(int('0123'));//seeding value

  //more "truer" random
  std::srand(std::time(NULL));
}

//values are from low to high; RandomValue(0,5) returns [0,5]
inline int RandomValue(int low, int high)
{
  return low + (std::rand() % (abs(low) + high + 1));
}




#endif