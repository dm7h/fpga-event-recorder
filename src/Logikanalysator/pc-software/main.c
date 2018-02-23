//#include "/usr/lib/bcc/include/conio.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "handling.h"

void print_menu()
{
	if (pathes.status == 'r')
	{
		printf("Running\n");
	}
	else if (pathes.status == 's')
	{
		printf("Stopped\n");
	}
	printf("[1] Optionen\n[2] Messung Starten\n[3] Messung Stoppen\n[4] Daten Holen\n[5] Beenden\n\n");
}
void print_options()
{
	printf("[1] Output Pfad\n[2] Zurück\n\n");
}



void option_hardware_path()
{
	char path[200] = {0};

	system("clear");
	get_hardware_Path(path);
	printf("Alter Hardware Pfad: %s\n",path);
	printf("Hardware Pfad eingeben:");
	scanf("%s",path);
	set_hardware_Path(path);
}

void option_output_path()
{
	char path[200] = {0};
	
	system("clear");
	get_output_Path(path);
	printf("Alter Output Pfad: %s\n",path);
	printf("Output Pfad eingeben:");
	scanf("%s",path);
	set_output_Path(path);
}

void option_messurement_time(void)
{
	system("clear");
	printf("Alte Messzeit: %d\n",pathes.messurement_time);
	printf("Ist die Messzeit 0 wird Unendlich lange gemessen.\n");
	printf("Neue Messzeit angeben:");
	scanf("%d",pathes.messurement_time);
}

void options()
{
char buffer;
	while(1)
	{

		print_options();
		buffer = getchar();
		system("clear");
		switch(buffer)
		{
			/*case '1':
				option_hardware_path();
			break;*/
			case '1':
				option_output_path();
			break;
			/*case '3':
			option_messurement_time();
			break;*/
			case '2':
			return;
			break;

		}
	}

}

int test_fd(int fd)
{
	printf("test_fd\n");
  if (fd < 0)
  {
    printf("error while opening Uart\n");
    option_hardware_path();
    if (!strcmp(pathes.hardware_Path,"exit"))
    {
    	exit(0);
    }
    fd = open_uart();
    test_fd(fd);

  }


  return fd;
}

/*void print_device_name(int file)
{
	c_buffer = 
	send_byte(file,'i');
	while
	printf ("On ")
}*/

int main(int argc, char const *argv[])
{
	strcpy(pathes.hardware_Path,"/dev/ttyACM0");
	strcpy(pathes.output_Path,"test.vcd");
	pathes.messurement_time = 0;

	int file = 0;
	file = open_uart();
	file = test_fd(file);
	char buffer;
	while(1)
	{
		if(buffer != '\n'){
			print_menu();			
		}

		buffer = getchar();
		if(buffer != '\n'){
			system("clear");			
		}
		switch(buffer)
		{
			case '1':
				//system("clear");
				options();
			break;
			case '2':
				//system("clear");
				start_Messurement(file);

				/*if (pathes.messurement_time > 0)
				{
					wait(pathes.messurement_time);
					stop_Messurement(file);
				}*/

			break;
			case '3':
				//system("clear");
				stop_Messurement(file);
			break;
			case '4':
				//system("clear");
				dump_Data(file);
			break;
			case '5':
			exit(0);
			system("clear");
			break;
			//case '\r':
			//system("clear");

			//break;
			default:
			break;
		}
	}
	close_uart(file);
	return 0;
}