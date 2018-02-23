#include "handling.h"



int open_uart(void)
{
  int file = 0;

   file = open(pathes.hardware_Path,O_RDWR);    

  printf("open Uart fd:\t%d\n",file);
  return file;
}



void close_uart(int fd)
{
  close(fd);
}

int send_byte(int file ,char buffer)
{

  int bytes = 0;

  //printf("write\t%c\n",buffer);
  
  bytes = write(file, & buffer,1);

 //printf("written:\t%d*%c\n",bytes,buffer);

}

char read_byte(int file)
{
  char buffer;

 // printf("try to read\n");  
  read(file,& buffer,1);

 // printf("read\t%c\n",buffer);
  
  return buffer;
}

void set_hardware_Path(char path[])
{
	strcpy(pathes.hardware_Path,path);
}


void get_hardware_Path(char path[])
{
	strcpy(path,pathes.hardware_Path);
}
void set_output_Path(char path[])
{
	strcpy(pathes.output_Path,path);
}
void get_output_Path(char path[])
{
	strcpy(path,pathes.output_Path);
}

char get_Status(void)
{
	return pathes.status;
}

char start_Messurement(int file)
{
	char buffer = 0;
	send_byte(file,'g');

	buffer = read_byte(file);
	pathes.status = buffer;
	printf("Buffer:%c\n",buffer);

	return buffer;
}

char stop_Messurement(int file)
{
	
	char buffer = 0;
	send_byte(file,'s');

	buffer = read_byte(file);
	pathes.status = buffer;
	printf("Buffer:%c\n",buffer);
	return buffer;
}

void dump_Data(int uart)
{
	
	send_byte(uart,'d');

	if ('s' == read_byte(uart))
	{
	write_VCD(uart);
	}
	else
	{
		printf("Messung läuft noch\n\n");
	}
}
 void int_to_bin_a(uint8_t input,char output[])
 {
 	int i = 0;
 	uint8_t mask = 0x01;

 	//for (i = (sizeof(output)/sizeof(output[0]))-1; i >= 0; i--)
	for (i = 8-1; i >= 0; i--)
 	{
 		//printf("i:%i\n",i);
 		if(input & mask)
 		{
 			output[i]='1';
 		}
 		else
 		{
 			output[i]='0';
 		}
 		input >>= 1;
 	}
 	//printf("%s\n",output);
 }

char *get_time_string(char buffer[])
{
	time_t t;
	struct tm *s_time;

	t = time(NULL);

	s_time = localtime(&t);

	sprintf(buffer,"%s",asctime(s_time));
	printf("time: %s\n",buffer);

	return buffer;
}

void head_VCD(FILE *file,char wire_id[])
{
	char time_string[80] = {0};

	int i = 0;

	get_time_string(time_string);

	fprintf(file, "$date %s $end\n",time_string);
	fprintf(file, "$version USB -TPLE 1.0 $end\n");
	fprintf(file, "$comment 64 Bit Timestamp , 8Bit Data $end\n");
	fprintf(file, "$timescale 160ns $end\n");
	fprintf(file, "$scope module logic $end\n");
	fprintf(file, "$var wire 8 %% data $end\n");

	for (i = 0; i < 8; i++)
	{
		fprintf(file, "$var wire 1 %c %d $end\n",wire_id[i],8-i);
	}

	fprintf(file, "$upscope $end\n$enddefinitions $end\n");
}

void write_changes_VCD(FILE *file,char old[],char new[],char wire_id[])
{
	int i = 0;
	for (i = 0;i < 8 ;i++)
	{
		if(old[i] != new[i])
		{
			fprintf(file, "%c%c\n",new[i],wire_id[i]);
		}
	}

}

void write_VCD(int uart)
{
	FILE *file;

	uint8_t probes = 0;
	uint16_t timestamp_relativ = 0;
	unsigned long int timestamp_absulut = 0;
	unsigned long int timestamp_real = 0;
	uint8_t status = 0;

	uint8_t i_buffer = 0;

	char c_buffer_new[9] = {0};
	char c_buffer_old[9] = {0};
	char filepath[100] = {0};
	char wire_id[8] = {'!','=','$','&','/','(',')','?'};

	get_output_Path(filepath);
	
	file = fopen(filepath,"w+");
	
	printf("\n Write VCD to: %s\n\n",filepath);
	head_VCD(file,wire_id);
	while (status != 0xff)
	{
		probes = read_byte(uart);
		//printf("probes:\t0x%.2X\n",probes);
		i_buffer = read_byte(uart);
		timestamp_relativ = 0;
		//printf("timestamp 0x%.2X\n",timestamp_relativ);
		timestamp_relativ = timestamp_relativ | i_buffer ;
		//printf("timestamp 0x%.2X\n",timestamp_relativ);
		timestamp_relativ = timestamp_relativ << 8;
		//printf("timestamp 0x%.2X\n",timestamp_relativ);


		i_buffer = read_byte(uart);

		timestamp_relativ = timestamp_relativ | i_buffer ;
		//printf("timestamp 0x%.2X\n",timestamp_relativ);

		status = read_byte(uart);
		//printf("status:\t0x%.2X\n",status);

		if(timestamp_relativ == 0xffff)
		{
			timestamp_absulut += timestamp_relativ;
		}
		//else
		{
			timestamp_real = timestamp_absulut + timestamp_relativ;
		}
		
		int_to_bin_a(probes,c_buffer_new);
		printf("#%i\nb%.8s\n",timestamp_real,c_buffer_new);
		fprintf(file, "#%i\nb%.8s %%\n",timestamp_real,c_buffer_new);
		write_changes_VCD(file,c_buffer_old,c_buffer_new,wire_id);
		//printf("###### 1 ####### %d \n", status);
		strcpy(c_buffer_old,c_buffer_new);

		//printf("###### 2 ####### %d \n", status);


	}
	fclose(file);

}
