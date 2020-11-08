/*
 Name:		Driver_Communication.ino
 Created:	08.11.2020 10:59:31
 Author:	Vasiliy
*/

// the setup function runs once when you press reset or power the board
#include "Remote_XY.h"
#include "slave_SPI.h"

void setup()
{
	Serial.begin(115200);	 //�������� ������� ����� �� �����
	Serial.println(" ");

	RemoteXY_Init();		 //�������� ���������� �� �������� ����� ������ ������ �� �����1
	Serial2.begin(115200);	 //�������� ���� 2 ��� ������ ����� ������ ������ �� ���������

	data_size = sizeof(myRemoteXY);   // ���������� ������ ���������
	Serial.print(" data_size= ");Serial.println(data_size);

	Init_SPI_slave();	 // ����������� SPI slave � �������� ����������



	// TODO you setup code

}

void loop()
{
	RemoteXY_Handler();				//���������� �����
	if (flag_sendData == false)    // ���� ������ �� ����������
	{
		noInterrupts();			//������ ���������� �� ����� �����������
		myRemoteXY = RemoteXY;        // ������ ����� ��������� ��� ��������
		interrupts();			// ���������� ����������
		//Serial.println(" myRemoteXY copy ");
		//myRemoteXY.connect_flag = 1;
		//myRemoteXY.Position = 122;
		//myRemoteXY.Servo_0 = 109;
		//myRemoteXY.Servo_1 = -66;
		//myRemoteXY.Servo_2 = -86;
	}

	//delay(3000);
	//PrintRemoteXY_variable();

	// TODO you loop code
	// ����������� ��������� RemoteXY ��� �������� ������
	// �� ����������� ������� delay() 


}