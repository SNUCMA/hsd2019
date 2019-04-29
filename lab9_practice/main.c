#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>


#define SIZE 64

typedef union {
  float f;
  unsigned int i;
}foo;

int main(int argc, char** argv)
{
  int i, j;
  foo container;

  float flat[SIZE * (SIZE + 1)];
  float mat[SIZE][SIZE];
  float input[SIZE];
  float output[SIZE];
  float output_fpga[SIZE];

  // initialization
  FILE *fd;
  fd = fopen("./input.txt", "r");

  unsigned int a;
  i = 0;
  while ( !feof(fd) )
  {
    if (fscanf(fd, "%X", &a) != EOF)
    {
      container.i = a;
      //printf("%f %X\n", container.f, container.i);
      if (i < SIZE)
        input[i] = container.f;
      else
        mat[i % 64][i/64 - 1] = container.f;

      flat[i] = container.f;
      //printf("%d, %f\n", i, container.f);
      i++;
    }
  }
  fclose(fd);

  for (i = 0; i < SIZE; i++)
    output[i] = 0.0f;

  // computation
  for (i = 0; i < SIZE; i++)
    for (j = 0; j < SIZE; j++)
      output[i] += input[SIZE-1-j] * mat[SIZE-1-j][i];

  // FPGA offloading
  // memory load
  int foo;
  foo = open("/dev/mem", O_RDWR | O_NONBLOCK);
  float *fpga_bram = mmap(NULL, (SIZE * (SIZE + 1))* sizeof(float), PROT_WRITE, MAP_SHARED, foo, 0x40000000);
  for (i = 0; i < SIZE * (SIZE + 1); i++)
  {
    *(fpga_bram + i) = flat[i];
  }

  // run
  unsigned int *fpga_ip = mmap(NULL, sizeof(float), PROT_WRITE, MAP_SHARED, foo, 0x43c00000);
  *fpga_ip = 0x5555;
  
  // wait
  while (*fpga_ip == 0x5555);

  // result
  for (i = 0; i < SIZE; i++)
    output_fpga[i] = *(fpga_bram + i); 


  // display
  printf("%-10s%-10s%-10s%-10s\n", "index", "CPU", "FPGA", "FPGA(hex)");
  for (i = 0; i < SIZE; i++)
  {
    container.f = output_fpga[i];
    printf("%-10d%-10f%-10f%-10X\n", i, output[i], output_fpga[i], container.i);
  }

  close(foo);

  return 0;
}
