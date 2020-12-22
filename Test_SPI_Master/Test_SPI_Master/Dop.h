
#define FREQ 1000 // частота прерываний, Герц  или 1 миллисекунда

const byte size_servo = 18;
const byte size_data = size_servo * 2;	 // 18*2=36 байт данных 
byte data_size = 0;

int16_t Data_Angle[size_servo];	  // Массив углов которые передаем

byte mode_work = 0;   //Режимы в которых может работать гексапод 0 -ничего не делает


byte buf[13];
int Data_Carent[6];   //6 значений датчиков тока
byte chek_sum_carent = 0;

// структура определяет все переменные и события вашего интерфейса управления 
struct struct_RemoteXY {

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

struct_RemoteXY myRemoteXY;

void ReadData_from_Communication(uint8_t CS_PIN, struct_RemoteXY* myRemoteXY)
{
	const int pausa = 25;
	byte buff_temp[128]; //Временный буфер на 128 байт
	byte chek_sum = 0;
	byte* p = (byte*)(myRemoteXY);
	digitalWrite(32, LOW);
	digitalWrite(CS_PIN, LOW);


	SPI.transfer(0x1A);					   // служебные идентификационные байты ими начинается пакет передачи
	delayMicroseconds(pausa);
	SPI.transfer(0x1B);					   // служебные идентификационные байты ими начинается пакет передачи
	delayMicroseconds(pausa);
	SPI.transfer(0x1D);					   // Команда
	delayMicroseconds(pausa);

	for (byte i = 0; i < data_size + 1; i++)  // +1 байт контрольной суммы
	{
		buff_temp[i] = SPI.transfer(0);
		if (i != data_size)
		{
			chek_sum += buff_temp[i];
		}
		delayMicroseconds(pausa);
	}
	digitalWrite(32, HIGH);
	digitalWrite(CS_PIN, HIGH);
	//SPI.endTransaction();

	if (buff_temp[data_size] == chek_sum)
	{
		//Serial.println("chek_sum Ok ");
		for (byte i = 0; i < data_size; i++)	//Копируем массив
		{
			p[i] = buff_temp[i];
		}
	}


	//SerialUSB.print(p[0]); SerialUSB.print(" ");
	//SerialUSB.print(p[1]);	SerialUSB.print(" ");
	//SerialUSB.print(p[2]);	SerialUSB.print(" ");
	//SerialUSB.print(p[3]);	SerialUSB.print(" ");
	//SerialUSB.print(p[4]);	SerialUSB.print(" ");
	//SerialUSB.print(p[5]);	SerialUSB.print(" ");
	//SerialUSB.print(p[6]);	SerialUSB.print(" ");

	//SerialUSB.println(" ");
}

void SendData_in_Driver(uint8_t CS_PIN, int16_t* Data_Angle)  // Указываем на каком пине устройство и с какого регистра нужно прочитать данные
{
	const int pausa = 25;
	byte chek_sum = 0;

	byte* p = (byte*)(Data_Angle);
	//for (byte i = 0; i < size_data; i++)
	//{
	//	SerialUSB.print("pi= ");
	//	SerialUSB.print(*p);
	//	SerialUSB.print(" ");
	//	p++;

	//	//Serial.println(p[i],HEX);
	//}
	//p = (byte*)(Data_Angle);

	//SPI.beginTransaction(settings16);       // записываем со скоростью 1 MHz делитель 16
	digitalWrite(33, LOW);
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
				buf[i - 1] = SPI.transfer(*p);	  // для 13 байта контрольную сумму не считаем так как там передается сама контрольная сумма
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
		//Serial.println(*bufferOut,HEX);SerialUSB
		//SerialUSB.println(*p);
		p++;
		delayMicroseconds(pausa);
	}
	SPI.transfer(chek_sum);				   // Передаем последним байтом контрольную сумму
	//SerialUSB.println(chek_sum);

	digitalWrite(33, HIGH);
	digitalWrite(CS_PIN, HIGH);
//	SPI.endTransaction();

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


void WriteByte_SPI(uint8_t CS_PIN, byte data)  // Указываем на каком пине устройство и с какого регистра нужно прочитать данные
{
	//SPI.beginTransaction();       // записываем со скоростью 1 MHz делитель 16
	digitalWrite(CS_PIN, LOW);
	SPI.transfer(data);
	digitalWrite(CS_PIN, HIGH);
	//SPI.endTransaction();
}

void Set_Start_Position()
{

	//int a_0 = 135;			// Положение для ходьбы
	//int a_1 = 180;
	//int a_2 = 90;

	int a_0 = 135;			// базовое положение, ноги в воздухе
	int a_1 = 210;
	int a_2 = 10;

	Data_Angle[0] = Body.Leg[0].angle_A = a_0;
	Data_Angle[1] = Body.Leg[0].angle_B = a_1;
	Data_Angle[2] = Body.Leg[0].angle_C = a_2;

	Data_Angle[3] = Body.Leg[1].angle_A = a_0;
	Data_Angle[4] = Body.Leg[1].angle_B = a_1;
	Data_Angle[5] = Body.Leg[1].angle_C = a_2;

	Data_Angle[6] = Body.Leg[2].angle_A = a_0;
	Data_Angle[7] = Body.Leg[2].angle_B = a_1;
	Data_Angle[8] = Body.Leg[2].angle_C = a_2;
	//
	Data_Angle[9] = Body.Leg[3].angle_A = a_0;
	Data_Angle[10] = Body.Leg[3].angle_B = a_1;
	Data_Angle[11] = Body.Leg[3].angle_C = a_2;
	//
	Data_Angle[12] = Body.Leg[4].angle_A = a_0;
	Data_Angle[13] = Body.Leg[4].angle_B = a_1;
	Data_Angle[14] = Body.Leg[4].angle_C = a_2;

	Data_Angle[15] = Body.Leg[5].angle_A = a_0;
	Data_Angle[16] = Body.Leg[5].angle_B = a_1;
	Data_Angle[17] = Body.Leg[5].angle_C = a_2;

}

void set_PIN()
{
	pinMode(8, OUTPUT);
	digitalWrite(8, HIGH);
	pinMode(9, OUTPUT);
	digitalWrite(9, HIGH);

	pinMode(10, OUTPUT);
	digitalWrite(10, HIGH);


	pinMode(32, OUTPUT);
	digitalWrite(32, HIGH);
	pinMode(33, OUTPUT);
	digitalWrite(33, HIGH);
	pinMode(34, OUTPUT);
	digitalWrite(34, HIGH);
	pinMode(35, OUTPUT);
	digitalWrite(35, LOW);

}

void Manual_control()		   // Преобразуем полученные данные из RwmoteXY в понятные углы  изадаем пределы движения для ручнойго управления проверки работоспособности
{
	Data_Angle[0] = map(myRemoteXY.Servo_0 + 100, 0, 200, 100, 170);
	Data_Angle[3] = map(myRemoteXY.Servo_0 + 100, 0, 200, 100, 170);
	Data_Angle[6] = map(myRemoteXY.Servo_0 + 100, 0, 200, 100, 170);
	Data_Angle[9] = map(myRemoteXY.Servo_0 + 100, 0, 200, 100, 170);
	Data_Angle[12] = map(myRemoteXY.Servo_0 + 100, 0, 200, 100, 170);
	Data_Angle[15] = map(myRemoteXY.Servo_0 + 100, 0, 200, 100, 1700);

	Data_Angle[1] = map(myRemoteXY.Servo_1 + 100, 0, 200, 180, 240);
	Data_Angle[4] = map(myRemoteXY.Servo_1 + 100, 0, 200, 180, 240);
	Data_Angle[7] = map(myRemoteXY.Servo_1 + 100, 0, 200, 180, 240);
	Data_Angle[10] = map(myRemoteXY.Servo_1 + 100, 0, 200, 180, 240);
	Data_Angle[13] = map(myRemoteXY.Servo_1 + 100, 0, 200, 180, 240);
	Data_Angle[16] = map(myRemoteXY.Servo_1 + 100, 0, 200, 180, 240);

	Data_Angle[2] = map(myRemoteXY.Servo_2 + 100, 0, 200, 10, 70);
	Data_Angle[5] = map(myRemoteXY.Servo_2 + 100, 0, 200, 10, 70);
	Data_Angle[8] = map(myRemoteXY.Servo_2 + 100, 0, 200, 10, 70);
	Data_Angle[11] = map(myRemoteXY.Servo_2 + 100, 0, 200, 10, 70);
	Data_Angle[14] = map(myRemoteXY.Servo_2 + 100, 0, 200, 10, 70);
	Data_Angle[17] = map(myRemoteXY.Servo_2 + 100, 0, 200, 10, 70);

}

void  setAngleForSend()              // переписываем углы из местного вида в массив для передачи
{
	Data_Angle[0] = Body.Leg[0].angle_A;
	Data_Angle[1] = Body.Leg[0].angle_B;
	Data_Angle[2] = Body.Leg[0].angle_C;

	Data_Angle[3] = Body.Leg[1].angle_A;
	Data_Angle[4] = Body.Leg[1].angle_B;
	Data_Angle[5] = Body.Leg[1].angle_C;

	Data_Angle[6] = Body.Leg[2].angle_A;
	Data_Angle[7] = Body.Leg[2].angle_B;
	Data_Angle[8] = Body.Leg[2].angle_C;

	Data_Angle[9]  = Body.Leg[3].angle_A;
	Data_Angle[10] = Body.Leg[3].angle_B;
	Data_Angle[11] = Body.Leg[3].angle_C;

	Data_Angle[12] = Body.Leg[4].angle_A;
	Data_Angle[13] = Body.Leg[4].angle_B;
	Data_Angle[14] = Body.Leg[4].angle_C;

	Data_Angle[15] = Body.Leg[5].angle_A;
	Data_Angle[16] = Body.Leg[5].angle_B;
	Data_Angle[17] = Body.Leg[5].angle_C;
}

void printDataRemoteXY()
{
	SerialUSB.print(" ");SerialUSB.print(myRemoteXY.connect_flag);
	SerialUSB.print(" ");SerialUSB.print(myRemoteXY.Position);
	SerialUSB.print(" ");SerialUSB.print(myRemoteXY.Servo_0);
	SerialUSB.print(" ");SerialUSB.print(myRemoteXY.Servo_1);
	SerialUSB.print(" ");SerialUSB.println(myRemoteXY.Servo_2);

}

void Timer_Init()     // Таймер 1 на 1 милисекунду по совпадению А
{
	SerialUSB.print("Timer TC0_C Init");

	REG_PMC_PCER0 |= 1 << 27; //включить тактирование "TC0"
	REG_TC0_CMR0 = 1 << 14;//CTC mode	с делителем /2 = 42МГц
	REG_TC0_RC0 = 42000000 / FREQ ; // рассчёт частоты
	REG_TC0_CCR0 = 5; //Обнулить счётчик и запустить.
	REG_TC0_IER0 = 1 << 4; // прерывание
	NVIC_EnableIRQ(TC0_IRQn);

	SerialUSB.println(" - Timer TC0_C START");


	//TCCR1A = 0;
	//TCCR1B = 0;
	//TCCR1B |= (1 << WGM12);                    // Режим CTC (сброс по совпадению) 
 //  // TCCR5B |= (1<<CS50);                       // Тактирование от CLK. 
	//														  // Если нужен предделитель : 
	//// TCCR5B |= (1<<CS51);                   // CLK/8 
	//TCCR1B |= (1 << CS10) | (1 << CS11); // CLK/64 //          Через 4 микросекунды считает 1/(16 000 000/64)
 //  // TCCR5B |= (1<<CS52);                   // CLK/256 
 //  // TCCR5B |= (1<<CS50)|(1<<CS52); // CLK/1024 
	//														// Верхняя граница счета. Диапазон от 0 до 65535. 
	//OCR1A = 249;    // Умножить на 4 микросекунды                         // Частота прерываний A будет = Fclk/(N*(1+OCR5A))  На 1 меньше так как считает от 0
 // //  OCR5B = 15624;                                // Частота прерываний B будет = Fclk/(N*(1+OCR5B)) 
	//														 // где N - коэф. предделителя (1, 8, 64, 256 или 1024) 
	//TIMSK1 = (1 << OCIE1A);                   // Разрешить прерывание по совпадению A 
 //  // TIMSK5 |= (1<<OCIE5B);                   // Разрешить прерывание по совпадению B 
 // //  TIMSK5 |= (1<<TOIE5);                     // Разрешить прерывание по переполнению 
}

bool flag_readData;
long count_readData;

bool flag_printData;
long count_printData;

void TC0_Handler(void) 			   //Обработччик таймера TC0_C а 1 миллисекунду
{
	if (REG_TC0_SR0&(1 << 4) == 0) { return; }

	count_readData++;
	count_printData++;

	if (count_readData >= FREQ/FREQ_WORK)	  // 1000/10 = 100 милисекунд или 10 Герц (10 раз за секунду)
	{
		flag_readData = true;
		count_readData = 0;
	}
	if (count_printData >= 200)
	{
		flag_printData = true;
		count_printData = 0;
	}
}

void sdsd()
{
	SerialUSB.println("FFFFFFF");
}