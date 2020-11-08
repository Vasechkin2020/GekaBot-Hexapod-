/*
 Name:		Test_SPI_Master.ino
 Created:	06.11.2020 20:39:06
 Author:	Vasiliy
*/
#include <SPI.h>
const byte size_servo = 18;
const byte size_data = size_servo * 2;	 // 18*2=36 байт данных 

byte buf[13];
int Data_Carent[6];   //6 значений датчиков тока
byte chek_sum_carent = 0;


void WriteReadBuffer_SPI(uint8_t CS_PIN, int* Data_Angle)  // Указываем на каком пине устройство и с какого регистра нужно прочитать данные
{
	const int pausa = 50;
	byte chek_sum = 0;

	byte* p = (byte*)(Data_Angle);
	//for (byte i = 0; i < size_servo*2; i++)
	//{
	//	Serial.print("pi= ");
	//	Serial.print(i);
	//	Serial.print(" ");

	//	Serial.println(p[i],HEX);
	//}

	//SPI.beginTransaction(settings16);       // записываем со скоростью 1 MHz делитель 16
	digitalWrite(CS_PIN, LOW);

	SPI.transfer(0x1A);					   // служебные идентификационные байты ими начинается пакет передачи
	delayMicroseconds(pausa);
	SPI.transfer(0x1B);					   // служебные идентификационные байты ими начинается пакет передачи
	delayMicroseconds(pausa);

	for (byte i = 0; i < size_data; i++)
	{
		if (i > 0 && i < 14)		//Принимаем данные по току начиная со 2 байта посылаемых данных поскольку ответ идет в обмен на 2 байт
		{
			if (i == 13)
			{
				buf[i-1] = SPI.transfer(*p);	  // для 13 байта контрольную сумму не считаем так как там передается сама контрольная сумма
			}
			else
			{
				buf[i - 1] = SPI.transfer(*p);
				chek_sum_carent += buf[i - 1];   //считаем контрольную сумму для пришедших данных
			}
		}
		else
		{
			SPI.transfer(*p);
		}
	
		chek_sum += *p;						//Считаем контрольную сумму
		//Serial.println(*bufferOut,HEX);
		p++;
		delayMicroseconds(pausa);
	}
	SPI.transfer(chek_sum);				   // Передаем последним байтом контрольную сумму

	digitalWrite(CS_PIN, HIGH);
	SPI.endTransaction();

	if (chek_sum_carent == buf[12])		 // Если чек суммы совпадают
	{
		int* buf2 = (int*)buf;
		for (byte i = 0; i < 6; i++)
		{
			Data_Carent[i] = buf2[i];
		}
	}


	//Serial.println("end");
}


int Data_Angle[size_servo];	  // Массив углов которые передаем

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);
	Serial.println(" ");
	Serial.println(" -------------------------------------------------");
	Serial.println(" START SetUp !!! ");
	Serial.println(" -------------------------------------------------");

	SPI.begin();
	//SPI.setDataMode(SPI_MODE3);
	//SPI.setBitOrder(MSBFIRST);
	//SPI.setClockDivider(SPI_CLOCK_DIV4);  // чтение SPI_CLOCK_DIV2 делитель 2 к частоте адруино 16 = 8 Мгерц

	pinMode(10, OUTPUT);

	//WriteByte_SPI(10, 0x1A, 0x1B);
	//Serial.println(" Send data... ");
	//
	//delay(99999);
	for (byte i = 0; i < size_servo; i++)
	{
		Data_Angle[i] = 180;
	}

	//Data_Angle[0] = 150;
	//Data_Angle[1] = 150;
	//Data_Angle[2] = 150;

	//Data_Angle[3] = 170;
	//Data_Angle[4] = 170;
	//Data_Angle[5] = 170;
	//
	//Data_Angle[6] = 200;
	//Data_Angle[7] = 200;
	//Data_Angle[8] = 200;
	////
	//Data_Angle[9] = 150;
	//Data_Angle[10] = 150;
	//Data_Angle[11] = 150;
	////
	//Data_Angle[12] = 125;
	//Data_Angle[13] = 125;
	//Data_Angle[14] = 125;
	//
	//Data_Angle[15] = 210;
	//Data_Angle[16] = 200;
	//Data_Angle[17] = 160;




}



// the loop function runs over and over again until power down or reset
void loop() 
{
	long a = micros();
	WriteReadBuffer_SPI(10, Data_Angle);
	long b = micros();
	Serial.print(" Send data... ");Serial.print(b-a);
	Serial.print(" micros... ");Serial.println(micros());

	for (byte i = 0; i < 12; i++)
	{
		//Serial.println(buf[i],HEX);
	}
	for (byte i = 0; i < 6; i++)
	{
		Serial.println(Data_Carent[i]);
	}
	
	delay(1000);
}
