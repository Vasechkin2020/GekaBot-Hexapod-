//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// определение режима соединения и подключение библиотеки RemoteXY 
#define REMOTEXY_MODE__HARDSERIAL

#include <RemoteXY.h>

// настройки соединения 
#define REMOTEXY_SERIAL Serial2
#define REMOTEXY_SERIAL_SPEED 115200


// конфигурация интерфейса  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
{ 255,6,0,0,0,44,0,10,186,1,
3,134,2,6,59,11,118,26,4,160,
3,82,58,13,235,26,4,32,2,23,
10,54,2,26,4,32,51,23,10,54,
2,26,5,32,16,32,32,32,136,26,
31 };

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
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////



#include <SPI.h>

// Настраиваем шину SPI как slave и включаем прерывние по пришедшему байту
void Init_SPI_slave()
{
	SPI.setDataMode(SPI_MODE3);
	SPI.setBitOrder(MSBFIRST);
	//SPI.setClockDivider(SPI_CLOCK_DIV4);  // чтение SPI_CLOCK_DIV2 делитель 2 к частоте адруино 16 = 8 Мгерц

	// have to send on master in, *slave out*
	pinMode(MISO, OUTPUT);
	// have to send on master in, *slave out*
	pinMode(MOSI, INPUT_PULLUP);

	pinMode(SS, INPUT_PULLUP);  // Чип селект на вход сигнала

	// turn on SPI in slave mode
	SPCR |= 1 << (SPE);
	// turn on interrupts
	SPCR |= 1 << (SPIE);
}

byte* MassivSend = 0;   // Адрес начала массива для передачи структуры
MyStruct myRemoteXY;


volatile byte count = 0;
volatile byte data = 0;
volatile byte flag_data = 0;
volatile byte chek_sum = 0;   // Байт контрольной суммы

byte buffer[32];	  //Буфер с запасом на 32 байта
byte comanda = 0;       // запоминаем команду которая пришла
byte data_size = 0;


bool flag_sendData; // Флаг передачи данных, если флаг взведен то перадаются данные
bool flag_goodCommand; // Флаг Что пришла правильная команда
bool flag_bedCommand; // Флаг Что пришла НЕ правильная команда

// SPI interrupt routine Прерывание по пришедшему байту
ISR(SPI_STC_vect)
{
	digitalWrite(46, 1);
	data = SPDR;
	SPDR = 0b10011001;
	//Serial.print("0 ");
	//Serial.print(count);
	//Serial.print(" ");
	//Serial.println(data);
		//Serial.println(count);
	
	switch (count)
	{
		case 0:
		{
			if (data == 0x1A) //Ждем первый случебный байт при нулевом счетчике
			{
				count++;                     // Увеличиваем счетчик, что значит что пришел первый байт
				digitalWrite(46, 0);
				return;
			}
		}
		case 1:
		{
			if (data == 0x1B) // Сюда когда был приняьт первый байт и пришел второй правильный
			{
				count++;                     // Увеличиваем счетчик, что значит что пришел второй байт
				digitalWrite(46, 0);
				return;
			}
			else
			{
				count = 0;                         // Пришел второй байт не правильный. сбрасываем счетчик и начинаем все сначала
				//Serial.print(millis()); 
				//Serial.print(" ");
				//Serial.print("BED");
				//Serial.print(" ");
				//Serial.println(data, HEX);
				flag_bedCommand = true;
				digitalWrite(46, 0);
				return;
			}
		}
		case 2:			    //Пришла команда
		{
			comanda = data;   //Запоминаем команду
			if (comanda == 0x1D)
			{
				flag_goodCommand = true;
			}
			myRemoteXY = RemoteXY;        // Делаем копию структуры для передачи  ее и передаем
			MassivSend = (byte*)&myRemoteXY;  //Запоминаем адрес начала структуры для побайтной передачи на разные команды подсовываем разные массивы, смотреть размер структуры в таком случае и менять
		}
	}

	if (count > 1 && count < data_size + 2)		// Передаем структуру
	{
		flag_sendData = true;   //начали передачу данных. запрет на изменение в лупе
		SPDR = MassivSend[count - 2];				  // Записываем в данные данные тока побайтово пишнм после приема 3 байта(1 байта с данными) и на другой стороне его надо принимать при отправле 2 байта с данными
		chek_sum += MassivSend[count - 2];  // считаем контрольную сумму
		count++;
		digitalWrite(46, 0);
		return;
	}
	if (count == data_size + 2)		// Передаем контрольную сумму
	{
		SPDR = chek_sum;
		chek_sum = 0;
		count++;
		digitalWrite(46, 0);
		return;
	}
	if (count == data_size + 3)		// На последнем байте передается только чтобы передать контрольную сумму
	{
		count = 0;             // Передали все данные, начинаем ждать новый правильный байт
		flag_sendData = false;   //начали передачу данных. запрет на изменение в лупе
		digitalWrite(46, 0);
		return;
	}
	digitalWrite(46, 0);
}

void PrintRemoteXY_variable()
{
	//RemoteXY.level_1 = RemoteXY.slider_1;
	Serial.print(" P= ");Serial.print(RemoteXY.Position);
	Serial.print(" S_0= ");Serial.print(RemoteXY.Servo_0);
	Serial.print(" S_1= ");Serial.print(RemoteXY.Servo_1);
	Serial.print(" S_2= ");Serial.print(RemoteXY.Servo_2);

	//Serial.print(" 1_x = ");Serial.print(RemoteXY.joystick_1_x);
	//Serial.print(" 1_y = ");Serial.println(RemoteXY.joystick_1_y);

	Serial.print(" Connect = ");Serial.print(RemoteXY.connect_flag);
	Serial.println(" ");
	//Serial.println(millis());

	//Serial2.print(" P= ");Serial2.print(RemoteXY.Position);
	//Serial2.print(" S_0= ");Serial2.print(RemoteXY.Servo_0);
	//Serial2.print(" S_1= ");Serial2.print(RemoteXY.Servo_1);
	//Serial2.print(" S_2= ");Serial2.print(RemoteXY.Servo_2);
	//Serial2.println("");

	//delay(100);
}