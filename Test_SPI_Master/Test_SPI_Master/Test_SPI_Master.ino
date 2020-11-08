/*
 Name:		Test_SPI_Master.ino
 Created:	06.11.2020 20:39:06
 Author:	Vasiliy
*/
#include <SPI.h>
const byte size_servo = 18;
const byte size_data = size_servo * 2;	 // 18*2=36 байт данных 
byte data_size = 0;


byte buf[13];
int Data_Carent[6];   //6 значений датчиков тока
byte chek_sum_carent = 0;

// структура определяет все переменные и события вашего интерфейса управления 
struct MyStruct {

	// input variables
	uint8_t Position; // =0 если переключатель в положении A, =1 если в положении B, =2 если в положении C, ... 
	int8_t Servo_0; // =-100..100 положение слайдера 
	int8_t Servo_1; // =-100..100 положение слайдера 
	int8_t Servo_2; // =-100..100 положение слайдера 
	int8_t joystick_1_x; // =-100..100 координата x положения джойстика 
	int8_t joystick_1_y; // =-100..100 координата y положения джойстика 

	  // other variable
	uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;

MyStruct myRemoteXY; 

void ReadDataCommand_SPI (uint8_t CS_PIN, MyStruct* myRemoteXY)
{
	const int pausa = 20;
	byte buff_temp[128]; //Временный буфер на 128 байт
	byte chek_sum = 0;
	byte* p = (byte*)(myRemoteXY);
	digitalWrite(CS_PIN, LOW);

	SPI.transfer(0x1A);					   // служебные идентификационные байты ими начинается пакет передачи
	delayMicroseconds(pausa);
	SPI.transfer(0x1B);					   // служебные идентификационные байты ими начинается пакет передачи
	delayMicroseconds(pausa);
	SPI.transfer(0x1D);					   // Команда
	delayMicroseconds(pausa);

	for (byte i = 0; i < data_size+1; i++)  // +1 байт контрольной суммы
	{
		buff_temp[i] = SPI.transfer(0);
		if (i != data_size)
		{
		   chek_sum += buff_temp[i];
		}
		delayMicroseconds(pausa);
	}
	digitalWrite(CS_PIN, HIGH);
	SPI.endTransaction();

	if (buff_temp[data_size] == chek_sum)
	{
		//Serial.println("chek_sum Ok ");
		for (byte i = 0; i < data_size; i++)	//Копируем массив
		{
			p[i] = buff_temp[i];
		}
	}


	//Serial.print(p[0]); Serial.print(" ");
	//Serial.print(p[1]);	Serial.print(" ");
	//Serial.print(p[2]);	Serial.print(" ");
	//Serial.print(p[3]);	Serial.print(" ");
	//Serial.print(p[4]);	Serial.print(" ");
	//Serial.print(p[5]);	Serial.print(" ");
	//Serial.print(p[6]);	Serial.print(" ");

	//Serial.println(" ");
}

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

	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);


	data_size = sizeof(myRemoteXY);   // Запоминаем размер структуры
	Serial.print(" data_size= ");Serial.println(data_size);


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
	WriteReadBuffer_SPI(9, Data_Angle);
	ReadDataCommand_SPI(10, &myRemoteXY);
	long b = micros();
	Serial.print(" Send data... ");Serial.print(b-a);
	Serial.print(" millis... ");Serial.print(millis());

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
		Serial.print(" ");Serial.print(myRemoteXY.Servo_0);
		Serial.print(" ");Serial.print(myRemoteXY.Servo_1);
		Serial.print(" ");Serial.println(myRemoteXY.Servo_2);
	
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


		Serial.print(" Data_Angle0... ");Serial.println(Data_Angle[0]);
		Serial.print(" Data_Angle1... ");Serial.println(Data_Angle[1]);
		Serial.print(" Data_Angle2... ");Serial.println(Data_Angle[2]);

	
	delay(10);
}
