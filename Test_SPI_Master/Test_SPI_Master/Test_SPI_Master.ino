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
			SerialUSB.print(" mode= "); SerialUSB.print(mode_work);
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
					//perviy_test();
					byte granica_kupola = raschet_virt_leg(delta_X, delta_X);       // Расчитываем положение 6 точек виртуальной ноги
					  // Перебираем все 6 ног и в зависимости от статуса ноги присваиваем точке опоры ноги нужную точку виртуальной ноги
					for (byte i = 0; i < 6; i++)             
					{
						if (Body.Leg[i].status == 0)      //Если Базовое положение то присваиваем точку 0
						{
							Body.Leg[i].prop.x = virt_Leg.point[0].x;
							Body.Leg[i].prop.y = virt_Leg.point[0].y;
							Body.Leg[i].prop.z = virt_Leg.point[0].z;
						}
						if (Body.Leg[i].status == 1)      //Если нога опорная то присваиваем точку 2
						{
							Body.Leg[i].prop.x = virt_Leg.point[2].x;
							Body.Leg[i].prop.y = virt_Leg.point[2].y;
							Body.Leg[i].prop.z = virt_Leg.point[2].z;
						}
						if (Body.Leg[i].status == 0)      //Если нога воздушная то присваиваем точку 3
						{
							Body.Leg[i].prop.x = virt_Leg.point[3].x;
							Body.Leg[i].prop.y = virt_Leg.point[3].y;
							Body.Leg[i].prop.z = virt_Leg.point[3].z;
						}
					}
					if (granica_kupola == 1)				// Если достигли границы купола то меняем статус ног с опорных на воздушние и наоборот и меняем точки 2 и 3 местами	 (фактически зеркалим точку 2 а точка 3 сама изменить в функции при следующем пересчете
					{
						virt_Leg.point[2].x = -virt_Leg.point[2].x;
						virt_Leg.point[2].y = -virt_Leg.point[2].y;
						virt_Leg.point[2].z = -virt_Leg.point[2].z;
					}





				}
				break;
			SendData_in_Driver(SS_PIN_1, Data_Angle);			 // Отправляем команды драйверу низкого уровня поставить сервомоторы в заданное положение
	}
		if (flag_printData)		 //ВЫводим на печать каждые 100 милисекунд
		{
			flag_printData = false;
			//printDataRemoteXY();
		}

	}
}
