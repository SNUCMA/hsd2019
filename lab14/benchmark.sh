#!/bin/bash

# Lab2 - Acc: 0.97
#sudo python eval.py --num_test_images 100 --m_size 64 --v_size 64 --network mlp --run_type cpu
#echo -e '\n=> Accuracy should be 0.97\n'

# MV - Acc: 0.97
#sudo python eval.py --num_test_images 100 --m_size 64 --v_size 64 --network mlp --run_type fpga
#echo -e '\n=> Accuracy should be 0.97\n'

# Lab11 - Conv Lowering(CPU) - Acc: 1.0
sudo python eval.py --num_test_images 100 --m_size 64 --v_size 64 --network cnn --run_type cpu
echo -e '\n=> Accuracy should be 1.0\n'

# Lab11 - Conv Lowering(CPU) - Acc: 1.0
#sudo python eval.py --num_test_images 100 --m_size 64 --v_size 64 --network cnn --run_type fpga
#echo -e '\n=> Accuracy should be 1.0'

# Lab14 - Quantization - Acc: 1.0
sudo python eval.py --num_test_images 100 --m_size 64 --v_size 64 --network cnn --run_type cpu --quantized
echo -e '\n=> Accuracy should be 1.0\n'
