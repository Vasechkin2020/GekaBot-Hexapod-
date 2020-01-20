/*
 Name:		Test_SPI_Master.ino
 Created:	06.11.2020 20:39:06
 Author:	Vasiliy
*/
#include <SPI.h>
#include <Arduino.h>
#include "Kinematika.h"
#include "Dop.h"

#define SS_PIN_1 8
#define SS_PIN_2 9

void setup()
{
	Serial.begin(115200);
	SerialUSB.println(" ");
	SerialUSB.println(" -------------------------------------------------");
	SerialUSB.println(" START SetUp !!! ");
	SerialUSB.println(" -------------------------------------------------");

	Timer_Init();     // Таймер 1 на  милисекунд по совпадению А


	SPI.begin();
	SPI.setDataMode(SPI_MODE3);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV8);  // чтение SPI_CLOCK_DIV2 делитель 2 к частоте адруино 16 = 8 Мгерц

	set_PIN();   // Устанавливаем и обьявляем пины. для вывода анализатора и всего прочего

	data_size = sizeof(myRemoteXY);   // Запоминаем размер структуры
	SerialUSB.print(" data_size= ");
	SerialUSB.println(data_size);

	//WriteByte_SPI(10, 0x1A, 0x1B);
	//Serial.println(" Send data... ");
	//
	Set_Start_Position();	   // Ноги в начальную позицию
	mode_work = 2;

	//SerialUSB.print(" MAX_ANGLE= ");SerialUSB.println(MAX_ANGLE);


	delay(1000);
}
int8_t napr = 1;

void loop() 
{
	while (true)
	{
		if (flag_readData)		   //Каждые 10 милисекунд опрашиваем плату управления если надо и посылаем данные в драйвер
		{
			flag_readData = false;
			ReadData_from_Communication(SS_PIN_2, &myRemoteXY);	  //Отправляем запрос и получаем данные с платы коммуникации.
			//mode_work = myRemoteXY.Position;					  // Смотрим в каком режиме нужно работать
			delay(1);
			SerialUSB.print(" mode= ");SerialUSB.print(mode_work);
			switch (mode_work)
			{
			case 0:				  // 0 - установка в начальное положение
				{
					Set_Start_Position();	   // Ноги в начальную позицию
				}
				break;
			case 1:				  // 1 - ручное управление серомотрами для теста что все работает
				{
					Manual_control();		 // Преобразуем данные в углы и задаем пределы движения сервомоторов
				}
				break;
			case 2:				 // 2 - управление командами на движение
				{
				float speed_a = 0.05;  //Скорость движения
				float angle_a = 0;  // Угол отклонения от прямо  в градусах направление движения
				int num_leg = 1;
				Body.Leg[num_leg].angle_A += get_angle_for_speed(speed_a, angle_a) * napr;
				Body.Leg[0].angle_A = Body.Leg[num_leg].angle_A+45;
				Body.Leg[2].angle_A = Body.Leg[num_leg].angle_A-45;

				Body.Leg[3].angle_A = 270 - Body.Leg[0].angle_A;
				Body.Leg[4].angle_A = 270 - Body.Leg[num_leg].angle_A;
				Body.Leg[5].angle_A = 270 - Body.Leg[2].angle_A;
				//Body.Leg[num_leg].angle_C += get_angle_for_speed(speed_a, angle_a) * napr;
				//Body.Leg[4].angle_B += get_angle_for_speed(speed_a, angle_a) * napr;

				SerialUSB.print(" step_angl= "); SerialUSB.print(get_angle_for_speed(speed_a, angle_a));
				SerialUSB.print(" angl= ");SerialUSB.print(Body.Leg[4].angle_A);
				float max_angle = get_max_angle(angle_a);
				SerialUSB.print(" MAX angl= ");SerialUSB.println(max_angle);

				if (Body.Leg[num_leg].angle_A > (135 + max_angle))
					{
						napr = -1;
					}
					if (Body.Leg[num_leg].angle_A < (135 - max_angle))
					{
						napr = 1;
					}
					setAngleForSend();		 //  Переписываем углы в массив для передачи
				}
				break;
			}
			SendData_in_Driver(SS_PIN_1, Data_Angle);			 // Отправляем команды драйверу низкого уровня поставить сервомоторы в заданное положение
	}
		if (flag_printData)		 //ВЫводим на печать каждые 100 милисекунд
		{
			flag_printData = false;
			//printDataRemoteXY();
		}

	}
}
