/* Sample SPI Example Program to program xcore-200 using spi.
*  Original code written by Derek Molloy (derekmolloy.ie) for the book Exploring
*  Raspberry Pi.
*
*  XMOS primary ref for spi programming is AN00238. It describes construction of
*  boot loader, image file and spi programmer code for the spi master.
*
*  Programming xcore-200 is performed as a continuous spi transaction.
*  SPI configuration as follows (Observe that AN00238 v1.0.2 chapter 3.5.1 is wrong.
*  The text assigns SS to TP27 and MOSI to TP30. However, a check with the port map and
*  schematics says that it is the opposite):
*  - SS (slave select pin): X0D00 (TP30 on kit)
*  - SCLK                 : X0D10 (TP28)
*  - MOSI                 : XOD11 (TP27)
*  - Slave mode is set by a pull-up of 3k3 ohm on X0D05.
*  - SPI mode 0
*  - LSB first is required by the built-in xcore ROM
*    There are some indications that the ioctl method is not implemented on RPI
*    So, make sure that the image file, image.bin, is transformed to LSB first.
*    See, https://github.com/alexjaw/xmos_utils
*    - http://tightdev.net/SpiDev_Doc.pdf
*    - https://www.raspberrypi.org/forums/viewtopic.php?t=36288
*  - xcore is ready for communication 1ms after RESET
*  - Transfer speeds 100kHz - 5MHz ok, and probably faster
*
* Written by A. Jaworowski */

#include<stdio.h>
#include<stdlib.h>  //memset, free
#include<fcntl.h>
#include<string.h>
#include<stdint.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<linux/spi/spidev.h>
#include "spiXMOS.h"

/*  Generation of LSB bin file (and corresponding header file) can be generated with 
*   https://github.com/alexjaw/xmos_utils
*/
char *image = NULL; // image to be populated with image.bin data (after memset)

unsigned long FileSize(FILE *fd){
  unsigned long fileLen;
  fseek(fd, 0, SEEK_END);
  fileLen=ftell(fd);
  //printf("File size: %i\n", fileLen);
  fseek(fd, 0, SEEK_SET);
  return fileLen;
}

/* Example:
*  int i = ProgramXCORE(file, "image.bin", 1000000 )
*  - opened spi device file descriptor : ...
*  - binary image file name (and path) : "image.bin"
*  - spi rate                          : 1MHz
*
*  Returns 0/-1
*  Caller is responsible for open/close of fd_spi
*/
//int ProgramXCORE(const char *device, const char *imageFile, uint32_t speed){
int ProgramXCORE(const int file, const char *imageFile, const uint32_t speed){
   uint8_t mode=0, bits=8;
   uint32_t delay=1000;
   //int file;  // For SPI communication
   
   // Fill image buffer with content in file imageFile
   FILE *fd;
   unsigned long fileLen;
   //Open file fd for image data
   fd = fopen(imageFile, "rb");
   if (!fd) {
      perror("SPI: Unable to open image file");
      return -1;
   }
   //Get file length
   fileLen = FileSize(fd);
   printf("Size of image file %i\n", fileLen);
   //Allocate memory
   image = malloc(fileLen * sizeof(*image));
   if (!image){
      perror("SPI: Memory error!");
      fclose(fd);
      return -1;
   }else{
      printf("Memory allocated\n");
   }
   //Read file contents into buffer
   int num_of_elements = fread(image, fileLen, 1, fd);
   printf("Num of elements %i\n", num_of_elements);  // must be same as the third argument to fread()
   fclose(fd);
   /*for (int i; i<5; i++){
      printf("image[%i]: 0x%02x\n", i, image[i]);
   }
   for (int i=fileLen-5; i<fileLen; i++){
      printf("image[%i]: 0x%02x\n", i, image[i]);
   }*/
   //End, image buffer filled with data

   struct spi_ioc_transfer transfer = {
      .tx_buf = (unsigned long) image,
      .len = fileLen,
      .delay_usecs = delay,
   };
   printf("spi_ioc_transfer struct prepared and ready for I/O\n");
   
   if (ioctl(file, SPI_IOC_WR_MODE, &mode)==-1){
      perror("SPI: Can't set SPI mode.");
      return -1;
   }
   if (ioctl(file, SPI_IOC_RD_MODE, &mode)==-1){
      perror("SPI: Can't get SPI mode.");
      return -1;
   }
   if (ioctl(file, SPI_IOC_WR_BITS_PER_WORD, &bits)==-1){
      perror("SPI: Can't set bits per word.");
      return -1;
   }
   if (ioctl(file, SPI_IOC_RD_BITS_PER_WORD, &bits)==-1){
      perror("SPI: Can't get bits per word.");
      return -1;
   }
   if (ioctl(file, SPI_IOC_WR_MAX_SPEED_HZ, &speed)==-1){
      perror("SPI: Can't set max speed HZ");
      return -1;
   } 
   if (ioctl(file, SPI_IOC_RD_MAX_SPEED_HZ, &speed)==-1){
      perror("SPI: Can't get max speed HZ.");
      return -1;
   }
   //printf("%d bytes to send\n", transfer.len);
   //printf("SPI mode: %d\n", mode);
   //printf("Bits per word: %d\n", bits);
   //printf("Speed: %d Hz\n", speed);
   
   /*SPI_IOC_MESSAGE(n):
   * - n is number of separate messages in the transfer struct, &transfer
   * We are sending the code as one (1) continous sequence.
   * See https://github.com/raspberrypi/tools/blob/master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/arm-linux-gnueabihf/libc/usr/include/linux/spi/spidev.h
   */
   if (ioctl(file, SPI_IOC_MESSAGE(1), &transfer)<0){
      printf("Failed to send message. Default RPi buffer 4096, see https://www.raspberrypi.org/forums/viewtopic.php?p=309582#p309582\n");
      perror("Failed to send SPI message");
      return -1;
   }else{
      printf("Message sent\n");
   }

   free(image);
   //printf("Programming finished.\n");
   //printf("Closed image buffer and file.\n");
   return 0;
}

/*int main(){
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
}*/
