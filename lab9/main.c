#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#define SIZE 64

int main(int argc, char** argv)
{
  int i;

  int foo = open("/dev/mem", O_RDWR | O_NONBLOCK);
  int *fpga_bram = mmap(NULL, SIZE * sizeof(int), PROT_WRITE, MAP_SHARED, foo, 0x40000000);
  for (i = 0; i < SIZE; i++)
    *(fpga_bram + i) = (float) (i * 2); 

  printf("%-10s%-10s\n", "addr", "FPGA(hex)");
  for (i = 0; i < SIZE; i++)
    printf("%-10d%-10X\n", i, *(fpga_bram + i));

  return 0;
}
