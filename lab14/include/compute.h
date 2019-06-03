#ifndef _COMPUTE_H_
#define _COMPUTE_H_
#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>

using namespace std;

struct Compute
{
  bool quantized;
  int act_bits;
  float act_min, act_max;
  int weight_bits;
  float weight_min, weight_max;

  Compute(bool quantized, int act_bits, float act_min, float act_max, int weight_bits, float weight_min, float weight_max)
         : quantized(quantized), act_bits(act_bits), act_min(act_min), act_max(act_max),
           weight_bits(weight_bits), weight_min(weight_min), weight_max(weight_max) {}
};
#endif
