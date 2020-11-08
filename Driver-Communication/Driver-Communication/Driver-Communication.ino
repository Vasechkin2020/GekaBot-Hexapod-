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
	Serial.begin(115200);	 //Включаем обычный вывод на экран
	Serial.println(" ");

	RemoteXY_Init();		 //Включаем управление по телефону через модуль блютуз на порту1
	Serial2.begin(115200);	 //Включаем порт 2 для вывода через второй блютуз на компьютер

	data_size = sizeof(myRemoteXY);   // Запоминаем размер структуры
	Serial.print(" data_size= ");Serial.println(data_size);

	Init_SPI_slave();	 // Настраиваем SPI slave и включаем прерывание



	// TODO you setup code

}

void loop()
{
	RemoteXY_Handler();				//обработчик связи
	if (flag_sendData == false)    // если данные не передаются
	{
		noInterrupts();			//запрет прерываний на время копирования
		myRemoteXY = RemoteXY;        // Делаем копию структуры для передачи
		interrupts();			// рахрешение прерывания
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
	// используйте структуру RemoteXY для передачи данных
	// не используйте функцию delay() 


}