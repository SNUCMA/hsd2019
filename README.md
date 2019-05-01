# HSD 2019

## Lab9

#### Manual

```
# on your host
$ git clone https://github.com/SNUCMA/hsd2019
$ cp hsd2019 ~/path/your_sd_card_partition1

# on your FPGA board
$ cp /sdcard/hsd2019 /home/zed
$ cd /home/zed
```

#### Note

* For `lab9`, follow the lab material to generate NEW bitstream file(.bit)
  * The bitstream `zynq.bit` on provided sd card is invalid for `lab9`

* For `lab9_practice`, please copy `lab9_practice/zynq.bit` to partition 1(1.1GB)
