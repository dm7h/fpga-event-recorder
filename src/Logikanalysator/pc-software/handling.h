//#include <ncurses.h>
//#include <menu.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/types.h>
#include <stdint.h>
#include <time.h>

#define PATH "./testPipe"


enum open_mode {wr,rd};

struct pathes
{
	char hardware_Path[200];
	char output_Path[200];
	int messurement_time;
	char status;

};

struct pathes pathes;



int open_uart(void);
void close_uart(int fd);

char start_Messurement(int file);
char stop_Messurement(int file);
void dump_Data(int uart);

void set_hardware_Path(char path[]);
void get_hardware_Path(char path[]);
void set_output_Path(char path[]);
void get_output_Path(char path[]);
char get_Status(void);

