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
		case 2:			    //Пришла команда
		{
			comanda = data;   //Запоминаем команду
			MassivSend = (byte*)&myRemoteXY;  //Запоминаем адрес начала структуры для побайтной передачи на разные команды подсовываем разные массивы, смотреть размер структуры в таком случае и менять
			data_size = sizeof(myRemoteXY);   // Запоминаем размер структуры
			//data_size = 3;
			//count++;                     // Увеличиваем счетчик, что значит что пришел второй байт
			//return;

		}
	}

	if (count > 1 && count < data_size + 2)		// Передаем структуру
	{
		flag_sendData = true;   //начали передачу данных. запрет на изменение в лупе
		SPDR = MassivSend[count - 2];				  // Записываем в данные данные тока побайтово пишнм после приема 3 байта(1 байта с данными) и на другой стороне его надо принимать при отправле 2 байта с данными
		chek_sum += MassivSend[count - 2];  // считаем контрольную сумму
		count++;
		return;
	}
	if (count == data_size + 2)		// Передаем контрольную сумму
	{
		SPDR = chek_sum;
		chek_sum = 0;
		count++;
		return;
	}
	if (count == data_size + 3)		// На последнем байте передается только чтобы передать контрольную сумму
	{
		count = 0;             // Приняли все данные, начинаем ждать новый правильный байт
		flag_sendData = false;   //начали передачу данных. запрет на изменение в лупе
		return;
	}

}

