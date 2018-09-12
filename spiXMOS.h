#ifndef SPIXMOS_H
# define SPIXMOS_H

/* Example:
*  int i = ProgramXCORE(file, "image.bin", 1000000 )
*  - opened spi device file descriptor : ...
*  - binary image file name (and path) : "image.bin"
*  - spi rate                          : 1MHz
*
*  Returns 0/-1
*  Caller is responsible for open/close of fd_spi
*/
int ProgramXCORE(const int file, const char *imageFile, const uint32_t speed);

#endif
