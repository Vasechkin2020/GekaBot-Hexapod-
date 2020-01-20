
#include <SPI.h>
//Задаем пин Slave select для SPI
#define SS_PIN 8

long time_start = 0;


void WriteByte_SPI(uint8_t CS_PIN, byte data)  // Указываем на каком пине устройство и с какого регистра нужно прочитать данные
{
	//SPI.beginTransaction();       // записываем со скоростью 1 MHz делитель 16
	digitalWrite(CS_PIN, LOW);
	SPI.transfer(data);
	digitalWrite(CS_PIN, HIGH);
	//SPI.endTransaction();
}


void setup() 
{
	Serial.begin(115200);
	SerialUSB.println("");

	SerialUSB.println("START PROGRAMM");


	SPI.begin();
	//SPI.setDataMode(SPI_MODE3);
	//SPI.setBitOrder(MSBFIRST);
	//SPI.setClockDivider(SPI_CLOCK_DIV16);  // скорость работы по шине  SPI_CLOCK_DIV2 делитель 2 к частоте адруино 16 = 8 Мгерц

  //определяем пин slave select как выход
	pinMode(SS_PIN, OUTPUT);
	digitalWrite(SS_PIN, HIGH);  // ensure SS stays high for now

	//delay(999);

	time_start = millis(); // Время старта программы
}


void loop() 
{
	if (millis()- time_start > 1000)
	{
		byte D = 16;
		SerialUSB.println(millis());
		WriteByte_SPI(SS_PIN, D);
		time_start = millis();
	}

}
