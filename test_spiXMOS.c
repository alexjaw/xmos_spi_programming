#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<stdint.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include "spiXMOS.h"

#define BUFSZ 128

int main(int argc, char *argv[]){
   char device[BUFSZ];
   char imagefile[BUFSZ];
   int fd_spi;
   // Input
   if(argc < 3 || argc > 3){
      printf("Error, syntax: ./test spi-device image-file\n");
      printf("Ex: ./test /dev/spidev0.3 img-lsb.bin\n");
      return -1;
   }else{
      strncpy(device, argv[1], BUFSZ);
      strncpy(imagefile, argv[2], BUFSZ);
      printf("i2s : %s\n", device);
      printf("file: %s\n", imagefile);
   }
   // Open spi device
   if ((fd_spi = open(device, O_RDWR))<0){
      perror("ProgramXCORE: Can't open device.");
      return -1;
   }
   // Program
   if (ProgramXCORE(fd_spi, imagefile, 1000000 )==-1){
      perror("ProgramXCORE: Programming failed.");
      return -1;
   }
   close(fd_spi);
   return 0;
}

