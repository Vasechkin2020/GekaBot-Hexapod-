/*
 Name:		Driver_servo.ino
 Created:	21.10.2020 22:33:32
 Author:	Vasiliy
*/

#include "Wire.h"
#include "Dop.h"
#include "PCA9685.h"
//#include "Ina219.h"


void setup() 
{
	Serial.begin(115200);
	Serial.println(" Start ");

	Wire.begin();
	//Wire.setClock(400000); // скорость передачи данных 400 кБит/с.
	Wire.setClock(1000000); // скорость передачи данных 1000 кБит/с.
	//I2C_test();

	Init_PCA9685(PCA9685_LEFT_ADDRESS);
	Init_PCA9685(PCA9685_RIGHT_ADDRESS);

	I2C_test();

	Init_ServoMotor();
	Set_StartPositionServoMotor();
	
	Init_SPI_slave();	 // Настраиваем SPI slave и включаем прерывание

	Data_Carent[0] = 80;
	Data_Carent[1] = 110;
	Data_Carent[2] = 90;
	Data_Carent[3] = 120;
	Data_Carent[4] = 100;
	Data_Carent[5] = 333;
	delay(5000);


	//Init_219(INA219_ADDRESS);


	//set_Angle_9685(PCA9685_LEFT_ADDRESS, 13, 0);
	//set_Angle_9685(PCA9685_LEFT_ADDRESS, 14, 2);
	//set_Angle_9685(PCA9685_LEFT_ADDRESS, 15, 0);

	//set_Angle_9685(PCA9685_RIGHT_ADDRESS, 13, 135);
	//set_Angle_9685(PCA9685_RIGHT_ADDRESS, 14, 200);
	//set_Angle_9685(PCA9685_RIGHT_ADDRESS, 15, 270);

	Timer1_Init();     // Таймер 1 на  милисекунд по совпадению А
	//delay(999992000);

}

long time_p1 = 0;
bool flag_napravlen = 0;
int gradus = 0;

// the loop function runs over and over again until power down or reset
void loop() 
{
	//По таймеру передаем данные в плату сервомоторов левую и правую 
	if (flag_9685 == true)	   
	{
		loop_writeAngle_to_PCA9685();
	}

	//По флагу выводим на печать данные для проверки
	if (flag_data == 1)
	{
		Serial.print("---");		Serial.println(millis());
		for (byte i = 0; i < size_servo; i++)
		{
			set_num_servo_angle(i, Data_Angle[i]);
		}

		for (byte i = 0; i < size_data/2; i++)
		{
			//Serial.println(buffer[i], HEX);
			//Serial.println(buffer2[i], HEX);
			//Serial.println(Data_Angle[i]);

		}
		for (byte i = 0; i < 12; i++)
		{
			//byte* p = (byte*)(Data_Carent);   // Запоминаем адрес начала массива. Используем для побайтной передачи
			//Serial.print("i= ");
			//Serial.print(i);
			//Serial.print(" ");
			//Serial.print(p[i], HEX);
			//Serial.println(">>");	

		}


		flag_data = 0;	  //закончили обработку данных
	}

}