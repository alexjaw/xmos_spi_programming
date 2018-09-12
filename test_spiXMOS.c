#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<stdint.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include "spiXMOS.h"

int main(){
   const char *device = "/dev/spidev0.0";
   int fd_spi;
   // Open spi device
   if ((fd_spi = open(device, O_RDWR))<0){
      perror("ProgramXCORE: Can't open device.");
      return -1;
   }
   // Program
   if (ProgramXCORE(fd_spi, "image.bin", 1000000 )==-1){
      perror("ProgramXCORE: Programming failed.");
      return -1;
   }
   close(fd_spi);
   return 0;
}

