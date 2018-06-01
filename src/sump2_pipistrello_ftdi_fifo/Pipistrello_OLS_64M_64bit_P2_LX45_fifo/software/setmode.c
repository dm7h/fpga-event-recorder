#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include "ftd2xx.h"


int main ( int argc, char *argv[] ) {
  FT_HANDLE ftHandle; 
  FT_STATUS ftStatus;
  DWORD i;
  WORD mode = 0, data;
  WORD buff[128];
  WORD checksum = 0xaaaa;

  if (argc > 1) {
    if (strcmp (argv[1], "fifo") == 0)
      mode = 0x0101;
    else if (strcmp (argv[1], "uart") == 0)
      mode = 0x0801;
    if (mode) {
      ftStatus = FT_OpenEx("Pipistrello LX45 B",FT_OPEN_BY_DESCRIPTION,&ftHandle);
      if (ftStatus == FT_OK) { 
        for (i=0; i<128; i++) {
          ftStatus = FT_ReadEE(ftHandle, i, &data);
          if (ftStatus == FT_OK) {
            buff[i] = data;
          }
        }
        buff[0] = mode;
        for (i=0; i<127; i++) {
          checksum = buff[i]^checksum;
          checksum = (checksum << 1) | (checksum >> 15);
        }
        buff[127] = checksum;
        for (i=0; i<128; i++) {
          ftStatus = FT_WriteEE(ftHandle, i, buff[i]);
        }
        FT_Close(ftHandle);
        printf("Mode changed to %s\nYou need to power-cycle the board for the change to take effect\n", argv[1]);
      } else
        printf("Error! Can't find Pipistrello board\n");
    } else
      printf("Usage: setmode fifo or setmode uart\n");
  } else
    printf("Usage: setmode fifo or setmode uart\n");
}
