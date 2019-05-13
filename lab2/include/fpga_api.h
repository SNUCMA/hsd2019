#ifndef _FPGA_API_H_
#define _FPGA_API_H_

#include<sys/types.h>

// matrix vector multiplicator
// matrix M: M_SIZE by V_SIZE
// vector V: V_SIZE
// output = M * V

class FPGA
{

private:
  int fd_;	
  float* data_;
  unsigned int* output_;

  int m_size_;
  int v_size_;
  int data_size_;
  int num_block_call_;
	
public:
  FPGA(off_t data_addr, off_t output_addr, int m_size, int v_size);
  ~FPGA();
      
  // return internal pointer for the data
  float* matrix(void);
  float* vector(void);
  void reset(void);
  int num_block_call(void);
      
  // perform matrix multiplication and return output array pointer
  const float* blockMV();	
      
  // Input vector size: num_input
  // Matrix size: num_output * num_input
  // Output vector size: num_output
  // O = M * I
  void largeMV(const float* mat, const float* input, float* output, int num_input, int num_output);

};

#endif

