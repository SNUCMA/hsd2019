#ifndef _DIGIT_DNN_H_
#define _DIGIT_DNN_H_
#include <string>
#include <vector> 
#include <memory>
#include <utility>
#include <unordered_map>

#include "caffe.pb.h"
#include "fpga_api.h"

struct Op
{
  virtual void run(const float* src, float* dst) = 0;
};


class DigitDNN
{

private:
  caffe::NetParameter net_;
  caffe::NetParameter net_trained_;
  std::vector<std::unique_ptr<Op> > ops_;
  std::vector<std::pair<int, int> > op_pair_; // op index to src, dst index pair 

  std::vector<std::vector<float> > data_;
  std::unordered_map<std::string, int> data_name2idx_;

  FPGA* dev_;
  int input_idx_;
  int output_idx_;

  int regDataIdx(std::string name, int size);  // register internal data space and return the index of data
  int getDataIdx(std::string name); // find the index of data. return -1 if the data is not yet registered

public:
  DigitDNN(std::string model, FPGA* dev);
  const float* run(const float* input);
  FPGA* getDev();
  int total_block_call();

};

#endif
