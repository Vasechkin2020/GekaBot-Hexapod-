/*
 Name:		Driver_Communication.ino
 Created:	08.11.2020 10:59:31
 Author:	Vasiliy
*/

// the setup function runs once when you press reset or power the board
#include "slave_SPI.h"

void setup()
{
	Serial.begin(115200);	 //�������� ������� ����� �� �����
	Serial.println(" ");

	RemoteXY_Init();		 //�������� ���������� �� �������� ����� ������ ������ �� �����1
	//Serial2.begin(115200);	 //�������� ���� 2 ��� ������ ����� ������ ������ �� ���������

	data_size = sizeof(myRemoteXY);   // ���������� ������ ���������
	Serial.print(" data_size= ");Serial.println(data_size);

	Init_SPI_slave();	 // ����������� SPI slave � �������� ����������
	pinMode(46, OUTPUT);
	pinMode(47, OUTPUT);

	data_size = sizeof(myRemoteXY);   // ���������� ������ ���������




	// TODO you setup code

}
long a = 0;

void loop()
{
	a = millis();

	while (true)
	{

		//digitalWrite(46, 1);

		RemoteXY_Handler();				//���������� �����
		//if (flag_sendData == false)    // ���� ������ �� ����������
		//{
		//	noInterrupts();			//������ ���������� �� ����� �����������
		//	myRemoteXY = RemoteXY;        // ������ ����� ��������� ��� ��������
		//	interrupts();			// ���������� ����������
			//Serial.println(" myRemoteXY copy ");
			//myRemoteXY.connect_flag = 1;
			//myRemoteXY.Position = 122;
			//myRemoteXY.Servo_0 = 109;
			//myRemoteXY.Servo_1 = -66;
			//myRemoteXY.Servo_2 = -86;
			//PrintRemoteXY_variable();

		//}
		//digitalWrite(46, 0);

		if (millis() - a > 200)
		{
			digitalWrite(47, 1);

			PrintRemoteXY_variable();
			a = millis();

			digitalWrite(47, 0);
		}

		if (flag_goodCommand == true)
		{
			Serial.print(" Ok ");
			flag_goodCommand = false;
		}
		if (flag_bedCommand == true)
		{
			Serial.print(" Error ");
			flag_bedCommand = false;
		}
		//delay(500);

		//Serial.println(millis());
		//delay(100);

	}
}