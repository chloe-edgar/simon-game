#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "display.h"
#include "gpio.h"
int listen(int* sequence, int score, int* button, int* led);
int main(int argc, char **argv) {
	int hiscore = 0;
	int score = 0;
	int fail;
	//configure devices: display, buzzer, LEDs, button
	int fd = open_gpio();
	int led[4]; 
	led[0] = config_led(25,fd); //red
	led[1] = config_led(9,fd); //yellow
	led[2] = config_led(22,fd); //green
	led[3] = config_led(17,fd); //blue
	int button[4]; 
	button[0] = config_button(11,fd); //red
	button[1] = config_button(10,fd); //yellow
	button[2] = config_button(27,fd); //green
	button[3] = config_button(4,fd); //blue
	config_buzzer();
	int display_fd = init_display(); //set up display
	
	//game code
	while(1){
		fail = 0; //reset fail flag
		score = 0; //reset score
		//generate random sequence of 99
		srand(time(0)); //set seed to time to keep it interesting
		int sequence[99];
		for(int i = 0; i < 99; i++){
			sequence[i] = rand() % 4;
		}
		write_score(display_fd, 0);
		usleep(200000); //take time resetting game
		while(1){ //play the game
			for(int i = 0; i <= score; i++){
				usleep(200000);
				buzzer_on(sequence[i]);
				led_on(led[sequence[i]]);
				//buzzer_on(led[sequence[i]]);
				usleep(500000);
				buzzer_on(2); //fixes hardware bug when jumping from low to high freq
				buzzer_off();
				led_off(led[sequence[i]]);
				//buzzer_off();
			}
			fail = listen(sequence, score, button, led); //success = 0; fail = 1
			if(fail) break;
			score++;
			if (score > 99) break; //doubt anyone will actually hit this
			write_score(display_fd, score);
		}
		//game over!
		buzzer_on(4);
		for(int i = 0; i<4; i++){ //light all LEDS
			led_on(led[i]);
		}
		if (hiscore < score) hiscore = score;
		write_oof(display_fd);
		usleep(3000000);
		for(int i = 0; i<4; i++){ //turn off all LEDS
			led_off(led[i]);
		}
		buzzer_on(2);
		buzzer_off();
		write_hiscore(display_fd, hiscore);
		while(1){ 
			if (button_pushed(button[0])) break;
			if (button_pushed(button[1])) break;
			if (button_pushed(button[2])) break;
			if (button_pushed(button[3])) break;
			usleep(10000); //save power by polling less
		}
	}
	return 0;
}
int listen(int* sequence, int score, int* button, int* led){
	time_t timestamp;
	time(&timestamp);
	time_t currenttime;
	time(&currenttime);
	int waittime;
	for(int i = 0; i <= score; i++){
		int j = 0;
		while(1){
			time(&currenttime);
			waittime = currenttime - timestamp;
			if (waittime > 5) return 1; //fail if wait too long (between 5-6 secs)
			if (button_pushed(button[j])){
				if (j != sequence[i]) return 1;
				time(&timestamp); //reset 5-6sec timer
				led_on(led[j]);
				buzzer_on(j);
				while(1){
					if (button_released(button[j])){
						led_off(led[j]);
						buzzer_on(2); //hardware bug fix
						buzzer_off();
						break;
					}
				}
			break;
			}
			usleep(5000); //poll every 5ms
			(j<3)?(j++):(j=0); //iterate throught buttons without for loop
		}
	}
	return 0;
}
