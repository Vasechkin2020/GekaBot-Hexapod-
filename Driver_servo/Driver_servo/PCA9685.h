
#define numServo 18		 //число серв 
#define SERVO_ANGLE 270   // Нас колько градусов поворачиваются моторы

#define SERVO_FREQ 100 // Analog servos run at ~50 Hz updates	 Digital up 300 Hz
#define SERVO_TAKT ((float)1000000/SERVO_FREQ/4096.0) // кол-во микросекунд которое требуется подавать сигнал на 1 имульс драйвера. Зависит от частоты сигнала и разрешающей способности платы управления
// или сколько микросекунд в одном такте платы . И если нам надо подать управляющий сигнал в 1500 микросекунд то надо 1500/	SERVO_TAKT 

#define PCA9685_LEFT_ADDRESS  0x43      /**< Default PCA9685 I2C Slave Address */
#define PCA9685_RIGHT_ADDRESS 0x41      /**<  PCA9685 I2C Slave Address */

#define PCA9685_MODE1 0x00      /**< Mode Register 1 */
#define MODE1_RESTART 0x80 /**< Restart enabled */
#define MODE1_SLEEP 0x10   /**< Low power mode. Oscillator off */
#define MODE1_AI 0x20      /**< Auto-Increment enabled */
#define FREQUENCY_OSCILLATOR 25000000 /**< Int. osc. frequency in datasheet */

#define PCA9685_PRESCALE 0xFE     /**< Prescaler for PWM output frequency */

#define PCA9685_ALLLED_ON_L 0xFA  /**< load all the LEDn_ON registers, 0 */
#define PCA9685_ALLLED_ON_H 0xFB  /**< load all the LEDn_ON registers, high */

#define PCA9685_SW_RESET   (byte)0x06          // Sent to address 0x00 to reset all devices on Wire line

//структра с данными по сервомотрам содержит угол и напрвление вращения
struct stru_Config_servo
{
	int angle;   	// угол уставновки сервы
	char napravl;  // Направление (Особенность) установки мотора подбирается по факту сборки и заноситься в начале при инициализации
	byte port;       // На каком порту платы находится мотор
};

struct stru_Config_lapa
{
	float current;    //Значение тока
	float BusVoltage;
	float BusPower;
	byte port;		  // Порт мультиплексора к которому подключеная лапа
};

stru_Config_servo Servo[numServo];  //Массив для хранения всех данных по серво  
stru_Config_lapa Lapa[COUNT_INA219];   //Массив для хранения  данных по току на лапе
//int Servo_angle[numServo];  //Массив для хранения углов сервомоторов 
//byte Servo_napravl[numServo];  //Массив для хранения направлений вращения сервомоторов 

//Перезагрузка в значения по умолчанию Отключаются субадреса
void Reset_All_9685()
{
	Wire.beginTransmission(0x00);
	Wire.write(PCA9685_SW_RESET);
	Wire.endTransmission();	
	delay(1);
}

void restart9685(uint8_t _addr)
{
	writeByte_i2c(_addr, PCA9685_MODE1, MODE1_RESTART);
	delay(10);
}

//Установка частоты передачи данных в сервомотры до 300Герц для цифровых и 50 для аналоговых
void setPWMFreq9685(uint8_t _addr, float _freq) 
{
	float prescaleval = ((FREQUENCY_OSCILLATOR / (_freq * 4096.0)) + 0.5) - 1;
	uint8_t prescale = (uint8_t)prescaleval;

	uint8_t oldmode = readByte_i2c(_addr, PCA9685_MODE1);
	uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
	writeByte_i2c(_addr, PCA9685_MODE1, newmode);                             // go to sleep
	writeByte_i2c(_addr, PCA9685_PRESCALE, prescale); // set the prescaler
	writeByte_i2c(_addr, PCA9685_MODE1, oldmode);
	delay(5);
	// This sets the MODE1 register to turn on auto increment.
	writeByte_i2c(_addr, PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);

	//Serial.print(" SERVO_FREQ= "); Serial.print(SERVO_FREQ);
	//Serial.print(" prescale= "); Serial.println(prescale);
}
//Начальная инициализация контроллеров
void Init_PCA9685(byte _adrr)
{
	Serial.print("Start Init PCA9685... 0x");
	Serial.print(_adrr, HEX);
	restart9685(_adrr);								   //Начальный сброс
	setPWMFreq9685(_adrr, SERVO_FREQ);				   //Устанавливаем частоту 50 для аналоговых или 100 герц для цифровых
	writeByte_i2c(_adrr, PCA9685_ALLLED_ON_L, 0);	   // Во все регистры начальные такта пишем ноль.  Чтобы потом не надо было каждый раз передавать 
	writeByte_i2c(_adrr, PCA9685_ALLLED_ON_H, 0);	   // Во все регистры начальные такта пишем ноль.
	Serial.println("   End Init PCA9685");
}
//Преобразуем угол в такты и пишем в регистры контроллера
void set_Angle_9685(byte _addr, byte _port, int _angle)	 
{
	byte on = 0;
	int pulse = map(_angle, 0, SERVO_ANGLE, 500, 2500) / SERVO_TAKT;
	//Serial.print("pulse= "); Serial.println(pulse);
	
	//Wire.setClock(1000000); // скорость передачи данных 1000 кБит/с.
	Wire.beginTransmission(_addr);
	Wire.write(0x08 + 4 * _port);  //Начинаем с регистра OFF и с шагом 4 байта на каждый порт
	//Wire.write(on);				//Начальные регистра не пишем, так как написали сразу при инициализации.
	//Wire.write(on >> 8);
	Wire.write(pulse);
	Wire.write(pulse >> 8);
	Wire.endTransmission();
	//Wire.setClock(400000); // скорость передачи данных 400 кБит/с.
}
void set_Led_ON_9685(byte _addr, byte _port)
{
	Wire.beginTransmission(_addr);
	Wire.write(0x06 + 4 * _port);  //Начинаем с регистра  и с шагом 4 байта на каждый порт
	Wire.write(0);
	Wire.write(0b10000);
	Wire.write(0);
	Wire.write(0);
	Wire.endTransmission();
	Serial.println("UP+");
}
void set_Led_OFF_9685(byte _addr, byte _port)
{
	Wire.beginTransmission(_addr);
	Wire.write(0x06 + 4 * _port);  //Начинаем с регистра  и с шагом 4 байта на каждый порт
	Wire.write(0);
	Wire.write(0);
	Wire.write(0);
	Wire.write(0b10000);
	Wire.endTransmission();
	Serial.println("DOWN+");
}

//Записываем в плату углы сервомоторов с 0 по 8 в левую с 9 по 17 в правую
void writeAngle_to_PCA9685()		
{
	for (byte i = 0; i < numServo; i++)
	{
		if (i >= 0 && i <= 8)
		{
			set_Angle_9685(PCA9685_LEFT_ADDRESS, Servo[i].port, Servo[i].angle);	 //Пишем в зависимости он того к какому порту мотор подключен и на какую плату
			//Serial.print("i= "); Serial.print(i);
			//Serial.print(" angle= "); Serial.println(Servo[i].angle);
		}
		if (i >= 9 && i <= 17)
		{
			set_Angle_9685(PCA9685_RIGHT_ADDRESS, Servo[i - 9].port, Servo[i].angle);//Пишем в зависимости он того к какому порту мотор подключен и на какую плату
			//Serial.print("ir= "); Serial.print(i-9);
			//Serial.print(" angle= "); Serial.println(Servo[i].angle);
		}
	}
}


void set_all_servo_angle(int _angle)
{
	for (byte i = 0; i < numServo; i++)
	{
		if (Servo[i].napravl == 1)
		{
			Servo[i].angle = _angle;
		}
		else
		{
			Servo[i].angle = 270 - _angle;
		}
	}
}
void set_group_servo_angle (byte _num, int _angle)
{
	for (byte i = _num; i < numServo; i = i+3)
	{
		if (Servo[i].napravl == 1)
		{
			Servo[i].angle = _angle;
		}
		else
		{
			Servo[i].angle = 270 - _angle;
		}
	}
}
void set_num_servo_angle(byte _num, int _angle)
{
		if (Servo[_num].napravl == 1)
		{
			Servo[_num].angle = _angle;
		}
		else
		{
			Servo[_num].angle = 270 - _angle;
		}
}

//Начальная установка напрвлений вращений моторов в зависимости как собрали макет
void Init_ServoMotor()
{
	// Из-за того что от датчиков тока я дорожки протянул не совсем правильно. Нужно сначала подключить средний и дальний моторы всх 3 ног и только потом три ближних мотора
	//Первая сторона
	Servo[0].napravl = -1;
	Servo[0].port = 6;

	Servo[1].napravl = -1;
	Servo[1].port = 0;

	Servo[2].napravl = 1;
	Servo[2].port = 1;
	
	Servo[3].napravl = -1;
	Servo[3].port = 7;
	
	Servo[4].napravl = -1;
	Servo[4].port = 2;

	Servo[5].napravl = 1;
	Servo[5].port = 3;

	Servo[6].napravl = -1;
	Servo[6].port = 8;

	Servo[7].napravl = -1;
	Servo[7].port = 4;

	Servo[8].napravl = 1;
	Servo[8].port = 5;

	//Вторая сторона

	Servo[9].napravl = -1;
	Servo[9].port = 6;

	Servo[10].napravl = -1;
	Servo[10].port = 0;

	Servo[11].napravl = 1;
	Servo[11].port = 1;

	Servo[12].napravl = -1;
	Servo[12].port = 7;

	Servo[13].napravl = -1;
	Servo[13].port = 2;

	Servo[14].napravl = 1;
	Servo[14].port = 3;

	Servo[15].napravl = -1;
	Servo[15].port = 8;

	Servo[16].napravl = -1;
	Servo[16].port = 4;

	Servo[17].napravl = 1;
	Servo[18].port = 5;
	
}
void Init_Lapa()		 // Тут определяем на каким портам мультиплексора соответствуют датчики INA219
{
	Lapa[0].port = 3;
	Lapa[1].port = 4;	
	Lapa[2].port = 5;

	Lapa[3].port = 2;
	Lapa[4].port = 0;
	Lapa[5].port = 1;
}

//Установка ног в начальную безопасную позицию
void Set_StartPositionServoMotor()
{
	//set_all_servo_angle(180);
	set_group_servo_angle(0, 135);
	set_group_servo_angle(1, 210);
	set_group_servo_angle(2, 10);
	writeAngle_to_PCA9685();          // Запись в  плату 
}
//Функция из лупа по тайменру каждые 10 милисекунд записывает данные в контроллеры					 
void loop_writeAngle_to_PCA9685()
{
	//long a = micros();
	writeAngle_to_PCA9685();
	//setAngle_to_ServoRight();
	//long b = micros();
	//Serial.println(b - a);
	//Serial.print("timer = ");
	//Serial.println(millis());
}

