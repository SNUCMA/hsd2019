#include"digit_dnn.h"
#include<cassert>
#include<fstream>
#include<iostream>
#include<algorithm>
#include<cstring>

struct MatVecOp:Op
{
  FPGA* dev_;
  const float* weight_;
  const float* bias_;
  int input_size_;
  int output_size_;
  
  MatVecOp(FPGA* dev, const float* weight, const float* bias, int input_size, int output_size)
    : dev_(dev), weight_(weight), bias_(bias), input_size_(input_size), output_size_(output_size){}

  void run(const float* src, float* dst)
  {
    dev_->largeMV(weight_, src, dst, input_size_, output_size_);

    if(bias_ != nullptr)
    {
      for(int i = 0; i < output_size_; ++i)
      {
        dst[i] += bias_[i];
      }
    }
  }
};

struct ReLUOp:Op
{
  int input_size_;
  ReLUOp(int input_size):input_size_(input_size){}

  void run(const float* src, float* dst)
  {
    for(int i = 0; i < input_size_; ++i)
    {
      dst[i] = src[i] > 0 ? src[i] : 0;
    }
  }
};

struct SoftmaxOp:Op
{
  int input_size_;
  SoftmaxOp(int input_size):input_size_(input_size){}

  void run(const float* src, float* dst)
  {
    float max_val = *std::max_element(src, src+input_size_);

    float sum = 0;
    for(int i = 0; i < input_size_; ++i)
    {
      dst[i] = exp(src[i] - max_val);
      sum += dst[i];
    }

    for(int i = 0; i < input_size_; ++i)
    {
      dst[i] /= sum;
    }
  }
};

int DigitDNN::regDataIdx(std::string name, int size)
{
  int rtn = getDataIdx(name);

  if(rtn != -1) { return rtn; }
  else
  {
    data_.resize(data_.size()+1);

    int idx = data_.size()-1;
    data_[idx].resize(size);
    data_name2idx_[name] = idx;

    return idx;
  }
}

int DigitDNN::getDataIdx(std::string name)
{
  if(data_name2idx_.find(name) != data_name2idx_.end())
  {
    return data_name2idx_[name];    
  }
  else
  {
    return -1;    
  }
}

DigitDNN::DigitDNN(std::string model, FPGA* dev):dev_(dev)
{  
  {
    std::fstream input(model.c_str(), std::ios::in | std::ios::binary);
    if(!net_.ParseFromIstream(&input))
    {
      assert(0 && "Failed to parse network.");
    }
  }

  // assumption 1: input data is provided by "data" blob and input size is 28 x 28
  // assumption 2: the first input blob of Softmaxloss layer is connected to final ouptut of network
  input_idx_ = regDataIdx("data", 28*28);

  for(int i = 0; i < net_.layer().size(); ++i)
  {
    if(std::string(net_.layer(i).type()).compare("InnerProduct") == 0) {
      const float* weight = net_.layer(i).blobs(0).data().data();
      const float* bias = nullptr;

      if(net_.layer(i).inner_product_param().bias_term())
       bias = net_.layer(i).blobs(1).data().data();

      std::string input = net_.layer(i).bottom(0);
      std::string output = net_.layer(i).top(0);

      int in_idx = getDataIdx(input);
      int input_size = data_[in_idx].size();

      int output_size = net_.layer(i).inner_product_param().num_output();
      int out_idx = regDataIdx(output, output_size);      

      // append new op 
      ops_.push_back(std::move(std::unique_ptr<Op>(new MatVecOp(dev_, weight, bias, input_size, output_size))));
      op_pair_.push_back({in_idx, out_idx});
    }
    else if(std::string(net_.layer(i).type()).compare("ReLU") == 0)
    {
      std::string input = net_.layer(i).bottom(0);
      std::string output = net_.layer(i).top(0);

      int in_idx = getDataIdx(input);
      int input_size = data_[in_idx].size();

      int output_size = input_size;
      int out_idx = regDataIdx(output, output_size);   

      ops_.push_back(std::move(std::unique_ptr<Op>(new ReLUOp(input_size))));
      op_pair_.push_back({in_idx, out_idx});
    }
    else if(std::string(net_.layer(i).type()).compare("SoftmaxWithLoss") == 0)
    {
      std::string input = net_.layer(i).bottom(0);
      std::string output = "prob";

      int in_idx = getDataIdx(input);
      int input_size = data_[in_idx].size();

      int output_size = input_size;
      int out_idx = regDataIdx(output, output_size);   

      ops_.push_back(std::move(std::unique_ptr<Op>(new SoftmaxOp(input_size))));
      op_pair_.push_back({in_idx, out_idx});

      output_idx_ = out_idx;
    }
  }
}

const float* DigitDNN::run(const float* input)
{
  memcpy(data_[input_idx_].data(), input, sizeof(float)*data_[input_idx_].size());

  for(int i = 0; i < ops_.size(); ++i)
  {
    auto& pair = op_pair_[i];
    ops_[i]->run(data_[pair.first].data(), data_[pair.second].data());
  }

  return data_[output_idx_].data();  
}

int DigitDNN::total_block_call()
{
    return dev_->num_block_call();
}

FPGA* DigitDNN::getDev(){ return dev_; }
