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
	//Wire.setClock(1000000); // скорость передачи данных 1000 кБит/с.
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
	//delay(1000);


	for (byte i = 0; i < size_data; i++)
	{
		buffer[i] = 0;
	}
	

	Init_All_INA219();
	Init_Lapa();

	pinMode(7, OUTPUT);
	digitalWrite(7, LOW);
	pinMode(6, OUTPUT);
	digitalWrite(6, LOW);
	pinMode(5, OUTPUT);
	digitalWrite(5, LOW);
	pinMode(4, OUTPUT);
	digitalWrite(4, LOW);


//	delay(999999999);

	//set_Angle2_9685(PCA9685_LEFT_ADDRESS, 13, 0);
	//set_Angle_9685(PCA9685_LEFT_ADDRESS, 14, 2);

		//set_Led_9685(PCA9685_LEFT_ADDRESS, 15, 1);
	//	for (byte i = 0; i < 10; i++)
	//{
	//		set_Led_ON_9685(PCA9685_LEFT_ADDRESS, 15);
	//		delay(100);
	//		set_Led_OFF_9685(PCA9685_LEFT_ADDRESS, 15);
	//		delay(100);
	//	}
	//delay(3000);
	//set_Led_9685(PCA9685_LEFT_ADDRESS, 15, 1);
	//Serial.println("UP");
	//delay(5000);


		//set_all_servo_angle(135);
	//set_Angle_9685(PCA9685_RIGHT_ADDRESS, 13, 135);
	//set_Angle_9685(PCA9685_RIGHT_ADDRESS, 14, 135);
	//set_Angle_9685(PCA9685_RIGHT_ADDRESS, 15, 135);													

	Timer1_Init();     // Таймер 1 на  милисекунд по совпадению А

	//set_group_servo_angle(0, 135);
	//set_group_servo_angle(1, 90);
	//set_group_servo_angle(2, 100);
	//writeAngle_to_PCA9685();          // Запись в  плату 


	//delay(7000);

}

long time_p1 = 0;
bool flag_napravlen = 0;
int gradus = 0;
long t_led_ok = millis();
long t_led_er = millis();

long time_data = millis();

// the loop function runs over and over again until power down or reset
void loop() 
{
	
	while (true)
	{

		//По флагу переписываем данные из буфера приема в массив данных, откуда потом их переаем в плату PCA9685
		if (flag_data == true)
		{

			//Serial.print("Data Ready...");		Serial.println(millis()- time_data);
			time_data = millis();
			for (byte i = 0; i < size_servo; i++)
			{
				set_num_servo_angle(i, Data_Angle[i]);
			}
			flag_data = false;	  //закончили обработку данных
		}
		//По таймеру передаем данные в плату сервомоторов левую и правую 
		if (flag_9685 == true)
		{
			loop_writeAngle_to_PCA9685();			//Тут закоментить и не будут работать сервы и мешать
			flag_9685 = false;
		}

		if (millis() - t_led_ok > 9)				  //Гасим светодиод если прошло 9 милисекунд с момента как включили
		{
			set_Led_OFF_9685(PCA9685_LEFT_ADDRESS, 13);
		}
		if (millis() - t_led_er > 9)				  //Гасим светодиод если прошло 9 милисекунд с момента как включили
		{
			set_Led_OFF_9685(PCA9685_LEFT_ADDRESS, 15);
		}

		if (flag_goodCommand == true)
		{
			//Serial.print(" OkC ");
			flag_goodCommand = false;
		}
		if (flag_bedCommand == true)
		{
			//Serial.print(" ErrorC ");
			flag_bedCommand = false;
		}
		if (flag_goodData == true)
		{
			//Serial.print(" OkD ");
			set_Led_ON_9685(PCA9685_LEFT_ADDRESS, 13);
			t_led_ok = millis();
			flag_goodData = false;
		}
		if (flag_bedData == true)
		{
			Serial.print(" ErrorD ");
			set_Led_ON_9685(PCA9685_LEFT_ADDRESS, 15);
			t_led_er = millis();

			flag_bedData = false;
		}



		if (flag_data == 1)
		{
			for (byte i = 0; i < size_data / 2; i++)
			{
				//Serial.println(buffer[i], HEX);
				//Serial.println(buffer2[i], HEX);
				//Serial.print("i= ");
				//Serial.print(i);
				//Serial.print(" ");
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
		/*
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
			   */
		if (millis() - time_p1 > 200)  // раз в секунду
		{
			//Serial.println(millis());
			if (flag_napravlen == 0)
			{
				//set_num_servo_angle(0, 90);
				//set_num_servo_angle(1, 90);
				//set_num_servo_angle(2, 90);

				//set_all_servo_angle(135);
				//set_group_servo_angle(0, 135);
				//set_group_servo_angle(1, 90);
				//set_group_servo_angle(2, 100);

				//flag_napravlen = 1;
			}
			else
			{
				//set_num_servo_angle(0, 150);
				//set_num_servo_angle(1, 150);
				//set_num_servo_angle(2, 150);
				//set_all_servo_angle(150);
				//set_group_servo_angle(0, 135);
				//set_group_servo_angle(1, 145);
				//set_group_servo_angle(2, 45);

				//flag_napravlen = 0;
			}
			Serial.print(Data_Angle[0]); Serial.print(" ");
			Serial.print(Data_Angle[1]); Serial.print(" ");
			Serial.print(Data_Angle[2]); Serial.print(" ");
			Serial.print(Data_Angle[3]); Serial.print(" ");
			Serial.print(Data_Angle[4]); Serial.print(" ");
			Serial.println(Data_Angle[5]);

			time_p1 = millis();
		}

	}
}