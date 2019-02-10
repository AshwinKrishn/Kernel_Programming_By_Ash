#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

const char name[100] = "Ashwin Savepo";
char read_buf[100]  = {0} ;
void main(){

	int ash_driver;
	ash_driver = open("/dev/ash_chrdrv",O_RDWR);
	
	if(ash_driver < 0){
		perror("File open failed :");
		return;
	}
	
	read(ash_driver ,read_buf , 20 );
	read_buf[20] = '\0' ;
	printf("the read string is %s\n\n",read_buf);
	printf("waiting for user input ....   \n\n");
	getchar();
	write(ash_driver , name , strlen(name));
	printf("waiting for user input ....   \n\n");
	getchar();
	read(ash_driver ,read_buf , 20 );
	read_buf[20] = '\0' ;
	printf("the new read string is %s\n\n",read_buf);
	printf("waiting for user input ....   \n\n");
	getchar();
	close(ash_driver);
}
