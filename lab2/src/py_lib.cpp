#include"py_lib.h"
#include"digit_dnn.h"
#include"fpga_api.h"
#include<string>
#include<stdio.h>

void* getNet(void* network, int m_size, int v_size)
//void* getNet(void* network)
{
  char* net_char = static_cast<char*>(network);
  FPGA* dev = new FPGA(0x40000000, 0x43c00000, m_size, v_size);
  return new DigitDNN(std::string(net_char), dev);
}

void delNet(void* net_ptr)
{
  DigitDNN* net = static_cast<DigitDNN*>(net_ptr);
  FPGA* dev = net->getDev();
  
  delete net;
  delete dev;
}

void inference(void* net_ptr, const void* in, void* out, int* num_call)
{
  DigitDNN* net = static_cast<DigitDNN*>(net_ptr);
  const float* ret = net->run(static_cast<const float*>(in));

  float* output = static_cast<float*>(out);
  for(int i = 0; i < 10; ++i)
  {
    output[i] = ret[i];
  }
  *num_call = net->total_block_call();
}
