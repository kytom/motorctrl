#include <reg52.h>
#include "12864.h"







int main(void)
{


	LCD_init();
	time0_init();

	while (1) {
		k = keyscan();
		motor_init(k);
		while (EN) {
			motor_go();
			speed1 = speed_measure(MOTOR1);
			speed2 = speed_measure(MOTOR2);
			print_speed();
			speed_match(speed1, speed2);
			motor_go();
		}
	}
}

void speed_match(int speed1, int speed2)
{
	if (speed1 == speed2) {
		return;
	} else if (speed1 > speed2) {
		up(MOTOR1);
		down(MOTOR2);
	} else {
		up(MOTOR2);
		down(MOTOR1);
	}
}

int speed_measure(int motor)
{
	int speed = 0;
	return speed;
}
