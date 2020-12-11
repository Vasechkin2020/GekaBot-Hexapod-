/*
 Name:		Driver_servo.ino
 Created:	21.10.2020 22:33:32
 Author:	Vasiliy
*/

#include "Wire.h"
#include "Dop.h"
#include "PCA9685.h"
#include "Ina219.h"


void setup() 
{
	Serial.begin(115200);
	Serial.println(" Start ");

	Wire.begin();
	//Wire.setClock(400000); // скорость передачи данных 400 кБит/с.
	Wire.setClock(1000000); // скорость передачи данных 1000 кБит/с.
	//I2C_test();

	Reset_All_9685();
	Init_PCA9685(PCA9685_LEFT_ADDRESS);
	Init_PCA9685(PCA9685_RIGHT_ADDRESS);

	I2C_test();
	delay(1000);

	Init_ServoMotor();
	Set_StartPositionServoMotor();
	
	Init_SPI_slave();	 // Настраиваем SPI slave и включаем прерывание

	//Data_Carent[0] = 80;
	//Data_Carent[1] = 110;
	//Data_Carent[2] = 90;
	//Data_Carent[3] = 120;
	//Data_Carent[4] = 100;
	//Data_Carent[5] = 333;
	delay(1000);

	

	Init_All_INA219();
	Init_Lapa();

//	delay(999999999);

	//set_Angle2_9685(PCA9685_LEFT_ADDRESS, 13, 0);
	//set_Angle_9685(PCA9685_LEFT_ADDRESS, 14, 2);

		//set_Led_9685(PCA9685_LEFT_ADDRESS, 15, 1);
		for (byte i = 0; i < 10; i++)
	{
			set_Led_ON_9685(PCA9685_LEFT_ADDRESS, 15);
			delay(100);
			set_Led_OFF_9685(PCA9685_LEFT_ADDRESS, 15);
			delay(100);
		}
	//delay(3000);
	//set_Led_9685(PCA9685_LEFT_ADDRESS, 15, 1);
	//Serial.println("UP");
	//delay(3000);



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
	//По флагу переписываем данные из буфера приема в массив данных, откуда потом их переаем в плату PCA9685
	if (flag_data == true)
	{
		Serial.print("Data Ready...");		Serial.println(millis());
		for (byte i = 0; i < size_servo; i++)
		{
			set_num_servo_angle(i, Data_Angle[i]);
		}
		flag_data = false;	  //закончили обработку данных
	}
	//По таймеру передаем данные в плату сервомоторов левую и правую 
	if (flag_9685 == true)	   
	{
		loop_writeAngle_to_PCA9685();
		flag_9685 = false;
	}


	if (flag_data == 1)
	{
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
	}

	if (flag_ina219 == true)	   //По таймеру опрашиваем датчики тока 
	{
		long a = micros();
		//Serial.println(millis());
		//Serial.print("readBusVoltage= ");
		//Serial.print(millis()); Serial.print(" ");

		//Serial.print(readBusVoltage(), 4); Serial.print(" ");

		////Serial.print("readShuntVoltage= ");
		////Serial.print(readShuntVoltage(), 4);  Serial.print(" ");

		//Serial.print("readShuntCurrent= ");
		for (byte i = 0; i < COUNT_INA219; i++)
		{
			set_TCA9548A(Lapa[i].port);
			Lapa[i].current = readShuntCurrent();
			Lapa[i].BusVoltage = readBusVoltage();
			Lapa[i].BusPower = readBusPower();
			if (Lapa[i].current < 4)
			{
				//Serial.print("I= ");
				Serial.print(Lapa[i].current, 2);  Serial.print(" ");
			}
		}

		//Serial.print("readBusPower= ");
		//Serial.println(readBusPower(), 4);


		//Serial.print("timer = ");
		long b = micros();

		Serial.println(b - a);
   		flag_ina219 = false;	

	}

	if (millis() - time_p1 > 2000)  // раз в секунду
	{
		if (flag_napravlen == 0)
		{
			//set_all_servo_angle(10);
			set_group_servo_angle(1, 180);
			set_group_servo_angle(2, 160);

			flag_napravlen = 1;
		}
		else
		{
			//set_all_servo_angle(260);
			set_group_servo_angle(1, 160);
			set_group_servo_angle(2, 200);

			flag_napravlen = 0;
		}
		time_p1 = millis();
	}


}