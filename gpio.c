#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "linux/gpio.h"
#include "sys/ioctl.h"

int open_gpio (void){
	int fd = open("/dev/gpiochip0", O_RDWR); //open GPIO device
	if(fd < 0){
		printf("Error opening %s\n", strerror(errno));
		exit(1); //check for error
	}
	return fd;
}
int config_led(int pin, int fd){
	struct gpiohandle_request req; //creates struct for GPIO requests
	memset(&req,0,sizeof(struct gpiohandle_request)); //clear struct
	req.flags = GPIOHANDLE_REQUEST_OUTPUT; //set to output
	req.lines = 1; //only one line
	req.lineoffsets[0] = pin; //set GPIO pin
	int rv = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req); //create handle
	if(rv < 0){ //check for error creating handle
		printf("Error ioctl makeing led handle %s", strerror(errno));
		exit(1);
	} 
	struct gpiohandle_data data;
	data.values[0] = 0; //set initial value to 0
	rv = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data); //write value
	if(rv < 0){ //check for error
		printf("Error ioctl write %s", strerror(errno));
		exit(1);
	} 
	return req.fd;
}
int config_button(int pin, int fd){
	struct gpiohandle_request req; //creates struct for GPIO requests
	memset(&req,0,sizeof(struct gpiohandle_request)); //clear struct
	req.flags = GPIOHANDLE_REQUEST_INPUT; //set to input
	req.lines = 1; //only one line
	req.lineoffsets[0] = pin; //set GPIO pin
	int rv = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req); //create handle
	if(rv < 0){ //check for error creating handle
		printf("Error ioctl making button handle %s", strerror(errno));
		exit(1);
	} 
	return req.fd;
}
int button_pushed(int reqfd){
	int rv;
	struct gpiohandle_data data;
	memset(&data, 0, sizeof(data)); //clear data struct
	rv = ioctl(reqfd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data); //get value of GPIO pin
	if(rv < 0){ //check for error
		printf("Error ioctl read 1 %s", strerror(errno));
		exit(1);
	} 
	if(data.values[0]) return 0; //if output is 1 (not pressed) return
	usleep(20000); //wait 20ms for debouncing
	memset(&data, 0, sizeof(data)); //clear data struct
	rv = ioctl(reqfd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data); //2nd reading of GPIO pin
	if(rv < 0){ //check for error
		printf("Error ioctl read 2 %s", strerror(errno));
		exit(1);
	} 
	return !data.values[0];
}
int button_released(int reqfd){
	int rv;
	struct gpiohandle_data data;
	memset(&data, 0, sizeof(data)); //clear data struct
	rv = ioctl(reqfd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data); //get value of GPIO pin
	if(rv < 0){ //check for error
		printf("Error ioctl %s", strerror(errno));
		exit(1);
	} 
	if(!data.values[0]) return 0; //if output is 0 (pressed) return
	usleep(20000); //wait 20ms for debouncing
	memset(&data, 0, sizeof(data)); //clear data struct
	rv = ioctl(reqfd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data); //2nd reading of GPIO pin
	if(rv < 0){ //check for error
		printf("Error ioctl %s", strerror(errno));
		exit(1);
	} 
	return data.values[0];
	
}
void led_on(int reqfd){
	int rv;
	struct gpiohandle_data data;
	data.values[0] = 1; //write 1 to pin
	rv = ioctl(reqfd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data); //write value
	if(rv < 0){ //check for error
		printf("Error ioctl %s", strerror(errno));
		exit(1);
	} 
	return;
}
void led_off(int reqfd){
	int rv;
	struct gpiohandle_data data;
	data.values[0] = 0; //write 0 to pin
	rv = ioctl(reqfd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data); //write value
	if(rv < 0){ //check for error
		printf("Error ioctl %s", strerror(errno));
		exit(1);
	} 
	return;
}
void write_pwm(char* path, char* val){
	int fd = open(path,O_WRONLY);
	if (fd == -1){
		printf("Error writing export to chip %s", strerror(errno));
		exit(1);
	}
	write(fd, val, strlen(val));
	close(fd);
}
void config_buzzer(){
	write_pwm("/sys/class/pwm/pwmchip0/export", "0"); //export pwm0
	return;
}	
void buzzer_on(int num){
	switch(num){
		case 0:
			write_pwm("/sys/class/pwm/pwmchip0/pwm0/period", "6067408"); //164.815 Hz: E3
			write_pwm("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "3033704"); //50% DC
			break;
		case 1:
			write_pwm("/sys/class/pwm/pwmchip0/pwm0/period", "4545454"); //220Hz: A3
			write_pwm("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "2272727"); //50% DC
			break;
		case 2:
			write_pwm("/sys/class/pwm/pwmchip0/pwm0/period", "3033704"); //329.63Hz: E4
			write_pwm("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "1516852"); //50% DC
			break;
		case 3:
			write_pwm("/sys/class/pwm/pwmchip0/pwm0/period", "2272727"); //440Hz: A4
			write_pwm("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "1136363"); //50% DC
			break;
		case 4:
			write_pwm("/sys/class/pwm/pwmchip0/pwm0/period", "8333333"); //set 10ms period: 100Hz
			write_pwm("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "4166666"); //5ms on time: DC 50%
			break;
	}
	usleep(1000);
	write_pwm("/sys/class/pwm/pwmchip0/pwm0/enable", "1"); //enable pwm
	return;
}
void buzzer_off(void){
	write_pwm("/sys/class/pwm/pwmchip0/pwm0/enable", "0"); //disable pwm
	return;
}

