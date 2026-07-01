#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

#include <errno.h> //header file for error handling
#include <string.h>
#include <math.h>
int init_display(void) {

	int fd;

	char i2c_device[]="/dev/i2c-1";

	unsigned char buffer[17];

	/* Open i2c device */

	fd = open(i2c_device, O_RDWR); //open the device
	if(fd < 0) printf("Error opening %s\n", strerror(errno)); //check for error

	/* Set slave address */

	int result =  ioctl(fd, I2C_SLAVE, 0x70); //set device address
	if(result < 0) printf("Error ioctl %s\n", strerror(errno)); //check for error

	/* Turn on oscillator */

	buffer[0] = (0x2<<4) | (0x1); //value to write to the system setup register
	result = write(fd,buffer,1); //write to fd
	if(result < 0) printf("Error writing %s\n", strerror(errno)); //check for error

	/* Turn on Display, No Blink */

	buffer[0] = (0x8<<4) | (0x1); //command to enable display
	result = write(fd,buffer,1); //write to fd
	if(result < 0) printf("Error writing %s\n", strerror(errno)); //check for error

	/* Set Brightness */
	buffer[0] = (0xe<<4) | (0x9); //set brightness to 9
	result = write(fd,buffer,1); //write to fd
	if(result < 0) printf("Error writing %s\n", strerror(errno)); //check for error

	return fd;
}

int write_score(int fd, int value){
	int result =  ioctl(fd, I2C_SLAVE, 0x70); //set device address
	unsigned char numbers[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
	unsigned char buffer[17];
	buffer[0] = 0x0;
	buffer[1] = 0x00; //off
	buffer[3] = 0x0; //off 
	buffer[5] = 0x0; //turn off :
	buffer[7] = numbers[(value / 10) % 10]; //tens
	buffer[9] = numbers[value % 10]; //ones
	result = write(fd,buffer,17); //write to fd
	if(result < 0) printf("Error writing %s\n", strerror(errno));
	return 0;
}
int write_hiscore(int fd, int value){
	int result =  ioctl(fd, I2C_SLAVE, 0x70); //set device address
	unsigned char numbers[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
	unsigned char buffer[17];
	buffer[0] = 0x0;
	buffer[1] = 0x76; //H
	buffer[3] = 0x06; //I 
	buffer[5] = 0x02; //turn on :
	buffer[7] = numbers[(value / 10) % 10]; //tens
	buffer[9] = numbers[value % 10]; //ones
	result = write(fd,buffer,17); //write to fd
	if(result < 0) printf("Error writing %s\n", strerror(errno));
	return 0;
}
int write_oof(int fd){
	int result =  ioctl(fd, I2C_SLAVE, 0x70); //set device address
	unsigned char buffer[17];
	buffer[0] = 0x0;
	buffer[1] = 0x0; //off
	buffer[3] = 0x5c; //o 
	buffer[5] = 0x0; //turn off :
	buffer[7] = 0x5c; //o
	buffer[9] = 0x71; //F
	result = write(fd,buffer,17); //write to fd
	if(result < 0) printf("Error writing %s\n", strerror(errno));
	return 0;
}
