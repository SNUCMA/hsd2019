#-*- coding: utf-8 -*-

import numpy as np
import ctypes
import pprint as pp
import os
import time

from data.load_mnist import load_mnist

#NUM_TEST_IMAGES = 100 # fast test
NUM_TEST_IMAGES = 10000 # full test

# matrix vector multiplicator
# matrix M: M_SIZE by V_SIZE
# vector V: V_SIZE
# output = M * V
M_SIZE = 16
V_SIZE = 16

class MLP(object):
  def __init__(self, net_path, m_size, v_size):
    self.lib = ctypes.cdll.LoadLibrary("./build/libpylib.so")
    self.net = self.lib.getNet(ctypes.c_char_p(net_path.encode("utf-8")), m_size, v_size)

    self.m_size = m_size
    self.v_size = v_size
    self.total_num_call = np.zeros(1, dtype=np.int)
    self.output = np.zeros(10, dtype=np.float32)

  def __del__(self):
    self.lib.delNet(self.net)
  
  def inference(self, input):
    self.lib.inference(self.net, input.ctypes.data, self.output.ctypes.data, self.total_num_call.ctypes.data)
    return self.output


if __name__ == "__main__":
  print("read dataset...")
  images, labels = load_mnist("test", path="./data")
  images, labels = images[:NUM_TEST_IMAGES, :, :], labels[:NUM_TEST_IMAGES]
  images = images.astype(np.float32)
  images = images/255.
  print("images", images.shape)
  
  print("create network...")		
  model_path = os.path.join("./pretrained_weights", "mlp_iter_10000.caffemodel")
  net = MLP(model_path, m_size=M_SIZE, v_size=V_SIZE)
  test_images = [images[i, :, :].copy() for i in xrange(NUM_TEST_IMAGES)]
  
  print("run test...")		
  n_correct = 0
  start_time = time.time()
  for i in xrange(NUM_TEST_IMAGES):
    X = test_images[i]
    X = X.reshape((28*28)) # 28x28->784

    logit = net.inference(X)
    prediction = np.argmax(logit)
    label = labels[i,]

    n_correct += (label == prediction)
  
  model_stats = {
    "total_time": time.time()-start_time,
    "total_image": NUM_TEST_IMAGES,
    "accuracy": float(n_correct)/NUM_TEST_IMAGES,
    "avg_num_call": net.total_num_call[0]/NUM_TEST_IMAGES,
    "m_size": net.m_size,
    "v_size": net.v_size,
  }
  pp.pprint(model_stats)
