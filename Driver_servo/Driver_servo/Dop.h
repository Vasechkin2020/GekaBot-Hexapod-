

void I2C_test()
{
	byte error, address;
	int nDevices;

	Serial.print("Scanning...");Serial.println(millis());

	nDevices = 0;
	for (address = 8; address < 127; address++) {
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0) {
			Serial.print(" Address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.print(address, HEX);
			Serial.println(" !");

			nDevices++;
		}
		else if (error == 4) {
			Serial.print("Unknow error at address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.println(address, HEX);
		}
	}
	if (nDevices == 0)
		Serial.println("No I2C ааdevices found\n");
	else
	{
		Serial.print("done = ");
		Serial.println(nDevices);
	}

	//delay(1000);
}

byte readByte_i2c(uint8_t _addr, uint8_t _registr)
{
	Wire.beginTransmission(_addr);
	Wire.write(_registr);
	Wire.endTransmission();

	Wire.requestFrom((uint8_t)_addr, (uint8_t)1);
	return Wire.read();
}
uint16_t  readInt_I2C(uint8_t address, int8_t registr)
{
	Wire.beginTransmission(address);
	Wire.write(registr);
	Wire.endTransmission();
	Wire.requestFrom(address, (uint8_t)2);
	uint8_t Hbyte = Wire.read();             //read High byte of data
	uint8_t Lbyte = Wire.read();             //read Low byte of data
	return ((int16_t)Hbyte << 8) | Lbyte;;             // Сдвигаем старший байт влево и добавляем младший байт
}
int16_t readRegister16(uint8_t reg)
{
	int16_t value;

	Wire.beginTransmission(0x44);
	Wire.write(reg);
	Wire.endTransmission();

	delay(1);

	Wire.beginTransmission(0x44);
	Wire.requestFrom(0x44, 2);
	while (!Wire.available()) {};
	uint8_t vha = Wire.read();
	uint8_t vla = Wire.read();

	Wire.endTransmission();

	value = vha << 8 | vla;

	return value;
}

void writeByte_i2c(uint8_t _addr, uint8_t _registr, uint8_t _data)
{
	Wire.beginTransmission(_addr);
	Wire.write(_registr);
	Wire.write(_data);
	int reza = Wire.endTransmission();
	if (reza != 0)
	{
		Serial.print(" writeByte_i2c Mistake reza= ");Serial.println(reza);
	}
}
void writeInt_i2c(uint8_t _addr, uint8_t _registr, int16_t _data)
{
	Wire.beginTransmission(_addr);
	Wire.write(_registr);
	Wire.write(_data >> 8);
	Wire.write(_data & 0xFF);
	int reza = Wire.endTransmission();
	if (reza != 0)
	{
		Serial.print(" writeInt_i2c Mistake reza= ");Serial.println(reza);
	}
}

static void Timer1_Init()     // Таймер 1 на 1 милисекунду по совпадению А
{
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B |= (1 << WGM12);                    // Режим CTC (сброс по совпадению) 
   // TCCR5B |= (1<<CS50);                       // Тактирование от CLK. 
															  // Если нужен предделитель : 
	// TCCR5B |= (1<<CS51);                   // CLK/8 
	TCCR1B |= (1 << CS10) | (1 << CS11); // CLK/64 //          Через 4 микросекунды считает 1/(16 000 000/64)
   // TCCR5B |= (1<<CS52);                   // CLK/256 
   // TCCR5B |= (1<<CS50)|(1<<CS52); // CLK/1024 
															// Верхняя граница счета. Диапазон от 0 до 65535. 
	OCR1A = 249;    // Умножить на 4 микросекунды                         // Частота прерываний A будет = Fclk/(N*(1+OCR5A))  На 1 меньше так как считает от 0
  //  OCR5B = 15624;                                // Частота прерываний B будет = Fclk/(N*(1+OCR5B)) 
															 // где N - коэф. предделителя (1, 8, 64, 256 или 1024) 
	TIMSK1 = (1 << OCIE1A);                   // Разрешить прерывание по совпадению A 
   // TIMSK5 |= (1<<OCIE5B);                   // Разрешить прерывание по совпадению B 
  //  TIMSK5 |= (1<<TOIE5);                     // Разрешить прерывание по переполнению 
}

bool flag_9685 = false;   // Флаг нужно ли отправлять новые данные в плату сервомоторов
bool flag_ina219 = false;   // Флаг нужно ли отправлять новые данные в плату сервомоторов
bool flag_gradus = false;

int count_9685 = 0;
int count_219 = 0;
int count_gradus = 135;


ISR(TIMER1_COMPA_vect)         // Обработчик прерывания таймера 4 по совпадению A 	1 раз в 1 милисекунд
{
	count_9685++;
	count_219++;
	count_gradus++;

	if (count_9685 >= 10)
	{
 		flag_9685 = true;
		count_9685 = 0;
	}
	if (count_219 >= 5)
	{
		flag_ina219 = true;
		count_219 = 0;
	}
	if (count_gradus >= 10)
	{
		flag_gradus = true;
		count_gradus = 0;
	}

}

// Настраиваем шину SPI как slave и включаем прерывние по пришедшему байту
void Init_SPI_slave()
{
	// have to send on master in, *slave out*
	pinMode(MISO, OUTPUT);
	// turn on SPI in slave mode
	SPCR |= 1 << (SPE);
	// turn on interrupts
	SPCR |= 1 << (SPIE);
}

long time_start = 0;
const byte size_servo = 18;
const byte size_data = size_servo * 2;	 // 18*2=36 байт данных и один контрольная сумма
volatile byte buffer[size_data];
volatile byte buffer2[size_data];
int Data_Carent[6];         // Значения датчика тока
byte chek_sum_carent = 0;


int* Data_Angle = 0; //Обявляем переменную в которой будем хранить адрес начала массива данных типа Int  данных с углами полученный с верхнего уровня
volatile byte count = 0;
volatile byte data = 0;
volatile byte flag_data = 0;
volatile byte chek_sum = 0;   // Байт контрольной суммы


// SPI interrupt routine Прерывание по пришедшему байту
ISR(SPI_STC_vect)
{
	data = SPDR;
	//Serial.print("0 ");
	//Serial.print(count);
	//Serial.print(" ");
	//Serial.println(data, HEX);
		//Serial.println(count);
	switch (count)
	{
		case 0:
		{
			if (data == 0x1A) //Ждем первый случебный байт при нулевом счетчике
			{
				count++;                     // Увеличиваем счетчик, что значит что пришел первый байт
				//SPDR = flag_data;        // Записываем в регистр для передачи статус обработки . Успели ли обработать предыдущую партию
				return;
			}
		}
		case 1:
		{
			if (data == 0x1B) // Сюда когда был приняьт первый байт и пришел второй правильный
			{
				count++;                     // Увеличиваем счетчик, что значит что пришел второй байт
				return;
			}
			else
			{
				count = 0;                         // Пришел второй байт не правильный. сбрасываем счетчик и начинаем все сначала
				Serial.println("Error");
				return;
			}
		}
	}
	buffer[count - 2] = data;
	if (count - 2 < size_data)	  //для всех кроме последнего и пропустили первые два служебных байта
	{
		chek_sum += data;			 // Суммирем все байты данных при перевышении остается младший байт его и сравниваем
	}
	else
	{
		if (chek_sum == data)		// Если наша чек сумма совпадает с последним байтом где чексума переданных данных 
		{
			//Serial.println("chek_sum Ok");

			if (flag_data == 0)    //Если уже успели обработали предыдущие данные в лупе
			{
				for (byte i = 0; i < size_data; i++)
				{
					buffer2[i] = buffer[i];
				}
				Data_Angle = (int*)(&buffer2);// Берем адрес массива(1 байт) и преврщаем его в адрес int (2 байта) и записываем его в переменную с адресом начала массива
				flag_data = 1;				//Включаем обработку в лупе	Если данные хорошие и прошлые уже обработаны
			}
		}
		else
		{
			Serial.println("chek_sum BED");
		}

		chek_sum = 0;
		count = 0;             // Приняли все данные, начинаем ждать новый правильный байт
		return;
	}
	if (count > 1 && count < 14)		// Передаем 12 байт
	{
		byte* p = (byte*)(Data_Carent);   // Запоминаем адрес начала массива. Используем для побайтной передачи
		SPDR = p[count-2];				  // Записываем в данные данные тока побайтово пишнм после приема 3 байта(1 байта с данными) и на другой стороне его надо принимать при отправле 2 байта с данными
		chek_sum_carent += p[count - 2];  // считаем контрольную сумму
	}
	if (count == 14)		// Передаем контрольную сумму
	{
		SPDR = chek_sum_carent;
		chek_sum_carent = 0;
	}	
	count++;
}



//if (flag_ina219 == true)	   //По таймеру опрашиваем датчики тока 
//{
//	long a = micros();
//	flag_ina219 = false;	
//	//Serial.println(millis());
//	//Serial.print("readBusVoltage= ");
//	//Serial.print(millis()); Serial.print(" ");

//	//Serial.print(readBusVoltage(), 4); Serial.print(" ");

//	////Serial.print("readShuntVoltage= ");
//	////Serial.print(readShuntVoltage(), 4);  Serial.print(" ");

//	//Serial.print("readShuntCurrent= ");
//	float Current = readShuntCurrent();
//	if (Current < 4)
//	{
//		Serial.print(Current, 4);  Serial.println(" ");
//	}

//	//Serial.print("readBusPower= ");
//	//Serial.println(readBusPower(), 4);


//	//Serial.print("timer = ");
//	long b = micros();

//	//Serial.println(b - a);

//}
/*
if (flag_gradus == true)
{
	flag_gradus = false;
	if (flag_napravlen == 0)
	{
		set_all_servo_angle(gradus);
		gradus += 4;
		if (gradus > 270)
		{
			flag_napravlen = 1;
			gradus = 269;
		}
	}
	else
	{
		set_all_servo_angle(gradus);
		gradus -= 4;
		if (gradus < 0)
		{
			flag_napravlen = 0;
			gradus = 1;
		}
	}
}
  */




  ////set_all_servo_angle(90);
  //if (millis() - time_p1 > 2000)  // раз в секунду
  //{
  //	if (flag_napravlen == 0)
  //	{
  //		set_all_servo_angle(10);
  //		flag_napravlen = 1;
  //	}
  //	else
  //	{
  //		set_all_servo_angle(260);
  //		flag_napravlen = 0;
  //	}
  //	time_p1 = millis();
  //	//Serial.print("flag_napravlen = ");
  //	//Serial.println(flag_napravlen);

