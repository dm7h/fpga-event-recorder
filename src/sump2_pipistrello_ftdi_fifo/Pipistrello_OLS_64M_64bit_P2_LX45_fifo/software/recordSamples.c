#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <time.h>
#include <sys/time.h>
#include "ftd2xx.h"


int timeval_subtract (result, x, y)
  struct timeval *result, *x, *y;
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }
     
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;
     
  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

FT_STATUS write_long_command (FT_HANDLE ftHandle, unsigned char cmd, unsigned int value) {
  unsigned char buffer[8];
  DWORD BytesWritten;
  buffer[0] = cmd;
  buffer[1] = value & 0xff;
  buffer[2] = (value >> 8) & 0xff;
  buffer[3] = (value >> 16) & 0xff;
  buffer[4] = (value >> 24) & 0xff;
  return (FT_Write(ftHandle, buffer, 5, &BytesWritten));
}

FT_STATUS write_command (FT_HANDLE ftHandle, unsigned char cmd) {
  unsigned char buffer[8];
  DWORD BytesWritten;
  buffer[0] = cmd;
  return (FT_Write(ftHandle, buffer, 1, &BytesWritten));
}

int main ( int argc, char *argv[] ) {
  FT_STATUS ftStatus;
  FT_HANDLE ftHandle;
  DWORD BytesWritten, BytesReceived; 
  unsigned char TxBuffer[256]; // Contains data to write to device
  unsigned char RxBuffer[64*1024]; // Receive data buffer
  unsigned int bytes, bytes_left;
  struct timeval t_start, t_end, t_diff;
  int request_size;
  FILE *fp;
  UCHAR Mask = 0xff;
  UCHAR Mode;

  setvbuf(stdout, 0, _IONBF, 0);

  // first argument is the transfer size in MB
  if (argc > 1) {
    bytes = atol(argv[1]) * 1024 * 1024;
  } else {
    // default is the full 64MB
    bytes = 1024 * 1024 * 64;
  }
  fp = fopen("sampledata.dat", "wb");
  // open the B port on Pipistrello v2 (async FIFO mode only!)
  ftStatus = FT_OpenEx("Pipistrello LX45 B",FT_OPEN_BY_DESCRIPTION,&ftHandle);
  if (ftStatus != FT_OK) {
    printf("Can't open FT2232H device! \n");
  } else {
    // no sync FIFO on port B
/*
    Mode = 0x40; //Sync FIFO mode
    ftStatus = FT_SetBitMode(ftHandle, Mask, Mode);
*/
    if (ftStatus != FT_OK) {
      printf("Can't set sync mode! \n");
    } else {
      int size = 65536;
//      printf("Transfer size set to %d\n", size);
      ftStatus = FT_SetUSBParameters(ftHandle, size, size);	//Set USB request transfer size
      ftStatus |= FT_SetFlowControl(ftHandle,FT_FLOW_RTS_CTS,0,0);
      ftStatus |= FT_SetChars(ftHandle, false, 0, false, 0);	 //Disable event and error characters
      ftStatus |= FT_SetTimeouts(ftHandle, 1000, 0);		//Sets the read and write timeouts in milliseconds for the FT2232H
      ftStatus |= FT_SetLatencyTimer(ftHandle, 16);		//Set the latency timer
      ftStatus |= FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX); // Clear buffers
      if (ftStatus == FT_OK) {
//        printf("Successfully initiated FT2232H device! \n");
        write_long_command(ftHandle, 0x00, 0x00000000); // reset
        write_long_command(ftHandle, 0xc0, 0x00000000); // trigger mask 0
        write_long_command(ftHandle, 0xc1, 0x00000000); // trigger value 0
        write_long_command(ftHandle, 0xc2, 0x08000000); // trigger config 0
        write_long_command(ftHandle, 0x82, 0x00000400); // flags (all channels, external test pattern enabled)
        write_long_command(ftHandle, 0x80, 0x00000000); // sample rate (100 MHz)
        write_long_command(ftHandle, 0x83, bytes>>4); // read count
        write_long_command(ftHandle, 0x84, bytes>>4); // delay count
        // send the trigger
        TxBuffer[0] = 0x01;
        ftStatus = FT_Write(ftHandle, TxBuffer, 1, &BytesWritten);
        if (ftStatus == FT_OK) {
          // how much data to ask for in each request
          request_size = 4096;
          bytes_left = bytes;
          // start timer
          gettimeofday(&t_start, NULL);
          while (1) {
            if (bytes_left > request_size) {
              ftStatus = FT_Read(ftHandle,RxBuffer,request_size,&BytesReceived); 
              if (ftStatus == FT_OK) { 
                if (BytesReceived == request_size) {
                  // all bytes received, i.e. no timeout
                  fwrite (RxBuffer, 1, request_size, fp);
                } else {
                  // did not get all bytes bytes -> timeout!
                  printf("Timeout! ");
                  fwrite (RxBuffer, 1, BytesReceived, fp);
                  bytes_left -= BytesReceived;
                  break;
                } 
              }
              bytes_left -= request_size;
            } else {
              request_size = bytes_left;
              ftStatus = FT_Read(ftHandle,RxBuffer,request_size,&BytesReceived); 
              if (ftStatus == FT_OK) { 
                if (BytesReceived == request_size) {
                  // all bytes received, i.e. no timeout
                  fwrite (RxBuffer, 1, request_size, fp);
                } else {
                  // did not get all bytes bytes -> timeout!
                  printf("Timeout! ");
                  fwrite (RxBuffer, 1, BytesReceived, fp);
                  bytes_left -= BytesReceived;
                  break;
                } 
              }
              bytes_left -= request_size;
              break;
            }
          }
          // stop timer
          gettimeofday(&t_end, NULL);
          // get the time difference
          timeval_subtract (&t_diff, &t_end, &t_start);
          // compute the time in microseconds
          DWORD usec = t_diff.tv_sec*1000000 + t_diff.tv_usec;
          // compute the transmission rate (in MBytes/sec)
          float rate = (float)(bytes - bytes_left)/usec;
          float elapsed = (float)usec/1000000;
          printf("Bytes received: %d\n", bytes - bytes_left);
          printf("Transfer rate: %f MBytes/sec\n", rate);
          printf("Time: %f seconds\n", elapsed);
        } else { 
          printf("Couldn't write to FT2232H device! \n"); 
        }
      }
      Mode = 0x00; //reset mode
      ftStatus = FT_SetBitMode(ftHandle, Mask, Mode);
    }
    ftStatus = FT_Close(ftHandle);
    if (ftStatus != FT_OK) {
      printf("Can't close FT2232H device! \n");
    } else {
      // Port closed successfully
//      printf("Successfully closed FT2232H device! \n");
    }
    fclose(fp);
  }
}
