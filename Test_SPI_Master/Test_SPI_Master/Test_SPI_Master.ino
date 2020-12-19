/*
 Name:		Test_SPI_Master.ino
 Created:	06.11.2020 20:39:06
 Author:	Vasiliy
*/
#include <SPI.h>
#include <Arduino.h>
#include "Dop.h"

#define SS_PIN_1 8
#define SS_PIN_2 9


// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);
	SerialUSB.println(" ");
	SerialUSB.println(" -------------------------------------------------");
	SerialUSB.println(" START SetUp !!! ");
	SerialUSB.println(" -------------------------------------------------");

	SPI.begin();
	SPI.setDataMode(SPI_MODE3);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV8);  // чтение SPI_CLOCK_DIV2 делитель 2 к частоте адруино 16 = 8 Мгерц

	pinMode(8, OUTPUT);
	digitalWrite(8, HIGH);
	pinMode(9, OUTPUT);
	digitalWrite(9, HIGH);

	pinMode(10, OUTPUT);
	digitalWrite(10, HIGH);


	pinMode(32, OUTPUT);
	digitalWrite(32, HIGH);
	pinMode(33, OUTPUT);
	digitalWrite(33, HIGH);
	pinMode(34, OUTPUT);
	digitalWrite(34, HIGH);
	pinMode(35, OUTPUT);
	digitalWrite(35, LOW);


	data_size = sizeof(myRemoteXY);   // Запоминаем размер структуры
	SerialUSB.print(" data_size= ");
	SerialUSB.println(data_size);

	//WriteByte_SPI(10, 0x1A, 0x1B);
	//Serial.println(" Send data... ");
	//
	//delay(99999);
	for (byte i = 0; i < size_servo; i++)
	{
		Data_Angle[i] = 135;
	}

	Data_Angle[0] = 135;
	Data_Angle[1] = 210;
	Data_Angle[2] = 10;

	Data_Angle[3] = 135;
	Data_Angle[4] = 210;
	Data_Angle[5] = 10;
	
	Data_Angle[6] = 135;
	Data_Angle[7] = 210;
	Data_Angle[8] = 10;
	//
	Data_Angle[9] = 135;
	Data_Angle[10] = 210;
	Data_Angle[11] = 10;
	//
	Data_Angle[12] = 135;
	Data_Angle[13] = 210;
	Data_Angle[14] = 10;
	
	Data_Angle[15] = 135;
	Data_Angle[16] = 210;
	Data_Angle[17] = 10;
}


// the loop function runs over and over again until power down or reset

void loop() 
{
	while (true)
	{


		//SerialUSB.print(" millis... ");
		//SerialUSB.println(millis());

		long a = micros();
		SendData_in_Driver(SS_PIN_1, Data_Angle);
		delay(1);
		ReadData_from_Communication(SS_PIN_2, &myRemoteXY);
		//WriteByte_SPI(9, 77);
		long b = micros();
		//SerialUSB.print(" Send data... "); SerialUSB.print(millis());

		for (byte i = 0; i < 12; i++)
		{
			//Serial.println(buf[i],HEX);
		}
		for (byte i = 0; i < 6; i++)
		{
			//Serial.println(Data_Carent[i]);
		}
		//Serial.print(" "); Serial.print(myRemoteXY.connect_flag);
		//Serial.print(" ");Serial.print(myRemoteXY.Position);

		SerialUSB.print(" ");SerialUSB.print(myRemoteXY.Servo_0);
		SerialUSB.print(" ");SerialUSB.print(myRemoteXY.Servo_1);
		SerialUSB.print(" ");SerialUSB.println(myRemoteXY.Servo_2);

		Data_Angle[0] = map(myRemoteXY.Servo_0 + 100, 0, 200, 50, 220);
		Data_Angle[3] = map(myRemoteXY.Servo_0 + 100, 0, 200, 50, 220);
		Data_Angle[6] = map(myRemoteXY.Servo_0 + 100, 0, 200, 50, 220);
		Data_Angle[9] = map(myRemoteXY.Servo_0 + 100, 0, 200, 50, 220);
		Data_Angle[12] = map(myRemoteXY.Servo_0 + 100, 0, 200, 50, 220);
		Data_Angle[15] = map(myRemoteXY.Servo_0 + 100, 0, 200, 50, 220);

		Data_Angle[1] = map(myRemoteXY.Servo_1 + 100, 0, 200, 50, 220);
		Data_Angle[4] = map(myRemoteXY.Servo_1 + 100, 0, 200, 50, 220);
		Data_Angle[7] = map(myRemoteXY.Servo_1 + 100, 0, 200, 50, 220);
		Data_Angle[10] = map(myRemoteXY.Servo_1 + 100, 0, 200, 50, 220);
		Data_Angle[13] = map(myRemoteXY.Servo_1 + 100, 0, 200, 50, 220);
		Data_Angle[16] = map(myRemoteXY.Servo_1 + 100, 0, 200, 50, 220);

		Data_Angle[2] = map(myRemoteXY.Servo_2 + 100, 0, 200, 50, 220);
		Data_Angle[5] = map(myRemoteXY.Servo_2 + 100, 0, 200, 50, 220);
		Data_Angle[8] = map(myRemoteXY.Servo_2 + 100, 0, 200, 50, 220);
		Data_Angle[11] = map(myRemoteXY.Servo_2 + 100, 0, 200, 50, 220);
		Data_Angle[14] = map(myRemoteXY.Servo_2 + 100, 0, 200, 50, 220);
		Data_Angle[17] = map(myRemoteXY.Servo_2 + 100, 0, 200, 50, 220);


		//SerialUSB.print(" Data_Angle0... ");SerialUSB.println(Data_Angle[0]);
		//SerialUSB.print(" Data_Angle1... ");SerialUSB.println(Data_Angle[1]);
		//SerialUSB.print(" Data_Angle2... ");SerialUSB.println(Data_Angle[2]);


		delay(100);
	}
}
