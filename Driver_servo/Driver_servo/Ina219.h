/** default I2C address **/
#define INA219_ADDRESS (0x44) // Запаяно А1
 
#define INA219_REG_CONFIG (0x00)	/** config register address **/
#define INA219_REG_SHUNTVOLTAGE (0x01)	/** shunt voltage register **/
#define INA219_REG_BUSVOLTAGE (0x02)	 /** bus voltage register **/
#define INA219_REG_POWER (0x03)	  /** power register **/
#define INA219_REG_CURRENT (0x04)	 /** current register **/
#define INA219_REG_CALIBRATION (0x05)	/** calibration register **/

#define INA219_CONFIG_RESET (0x8000) // Reset Bit	 /** reset bit **/

float currentLSB = 0;
float powerLSB = 0;

/** bus voltage range values **/
enum {
	INA219_CONFIG_BVOLTAGERANGE_16V = (0x0000), // 0-16V Range
	INA219_CONFIG_BVOLTAGERANGE_32V = (0x2000), // 0-32V Range
};

/** values for gain bits **/
enum {
	INA219_CONFIG_GAIN_1_40MV = (0x0000),  // Gain 1, 40mV Range
	INA219_CONFIG_GAIN_2_80MV = (0x0800),  // Gain 2, 80mV Range
	INA219_CONFIG_GAIN_4_160MV = (0x1000), // Gain 4, 160mV Range
	INA219_CONFIG_GAIN_8_320MV = (0x1800), // Gain 8, 320mV Range
};

/** values for bus ADC resolution **/
enum {
	INA219_CONFIG_BADCRES_9BIT = (0x0000),  // 9-bit bus res = 0..511
	INA219_CONFIG_BADCRES_10BIT = (0x0080), // 10-bit bus res = 0..1023
	INA219_CONFIG_BADCRES_11BIT = (0x0100), // 11-bit bus res = 0..2047
	INA219_CONFIG_BADCRES_12BIT = (0x0180), // 12-bit bus res = 0..4097
	INA219_CONFIG_BADCRES_12BIT_2S_1060US =	(0x0480), // 2 x 12-bit bus samples averaged together
	INA219_CONFIG_BADCRES_12BIT_4S_2130US =	(0x0500), // 4 x 12-bit bus samples averaged together
	INA219_CONFIG_BADCRES_12BIT_8S_4260US =	(0x0580), // 8 x 12-bit bus samples averaged together
	INA219_CONFIG_BADCRES_12BIT_16S_8510US =(0x0600), // 16 x 12-bit bus samples averaged together
	INA219_CONFIG_BADCRES_12BIT_32S_17MS =	(0x0680), // 32 x 12-bit bus samples averaged together
	INA219_CONFIG_BADCRES_12BIT_64S_34MS =	(0x0700), // 64 x 12-bit bus samples averaged together
	INA219_CONFIG_BADCRES_12BIT_128S_69MS =	(0x0780), // 128 x 12-bit bus samples averaged together
	};

/** values for shunt ADC resolution **/
enum {
	INA219_CONFIG_SADCRES_9BIT_1S_84US = (0x0000),   // 1 x 9-bit shunt sample
	INA219_CONFIG_SADCRES_10BIT_1S_148US = (0x0008), // 1 x 10-bit shunt sample
	INA219_CONFIG_SADCRES_11BIT_1S_276US = (0x0010), // 1 x 11-bit shunt sample
	INA219_CONFIG_SADCRES_12BIT_1S_532US = (0x0018), // 1 x 12-bit shunt sample
	INA219_CONFIG_SADCRES_12BIT_2S_1060US =	(0x0048), // 2 x 12-bit shunt samples averaged together
	INA219_CONFIG_SADCRES_12BIT_4S_2130US =	(0x0050), // 4 x 12-bit shunt samples averaged together
	INA219_CONFIG_SADCRES_12BIT_8S_4260US =	(0x0058), // 8 x 12-bit shunt samples averaged together
	INA219_CONFIG_SADCRES_12BIT_16S_8510US = (0x0060), // 16 x 12-bit shunt samples averaged together
	INA219_CONFIG_SADCRES_12BIT_32S_17MS = (0x0068), // 32 x 12-bit shunt samples averaged together
	INA219_CONFIG_SADCRES_12BIT_64S_34MS = (0x0070), // 64 x 12-bit shunt samples averaged together
	INA219_CONFIG_SADCRES_12BIT_128S_69MS =	(0x0078), // 128 x 12-bit shunt samples averaged together
};

/** values for operating mode **/
enum {
	INA219_CONFIG_MODE_POWERDOWN = 0x00,       /**< power down */
	INA219_CONFIG_MODE_SVOLT_TRIGGERED = 0x01, /**< shunt voltage triggered */
	INA219_CONFIG_MODE_BVOLT_TRIGGERED = 0x02, /**< bus voltage triggered */
	INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED = 0x03,                         /**< shunt and bus voltage triggered */
	INA219_CONFIG_MODE_ADCOFF = 0x04, /**< ADC off */
	INA219_CONFIG_MODE_SVOLT_CONTINUOUS = 0x05, /**< shunt voltage continuous */
	INA219_CONFIG_MODE_BVOLT_CONTINUOUS = 0x06, /**< bus voltage continuous */
	INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS = 	0x07, /**< shunt and bus voltage continuous */
};

void reset219(uint8_t _addr)
{
	writeInt_i2c(_addr, INA219_REG_CONFIG, INA219_CONFIG_RESET);
	delay(10);
}
bool calibrate(float rShuntValue, float iMaxExpected)
{
	currentLSB = iMaxExpected / 32767.0;
	powerLSB = currentLSB * 20.0;

	uint16_t calibrationValue = (uint16_t)((0.04096) / (currentLSB * rShuntValue));

	Serial.print("calibrationValue= ");
	Serial.println(calibrationValue, 4);

	writeInt_i2c(INA219_ADDRESS, INA219_REG_CALIBRATION, calibrationValue);

	return true;
}

float readBusVoltage(void)		   // Возврвщвем значение в вольтах
{
	int16_t voltage = readInt_I2C(INA219_ADDRESS, INA219_REG_BUSVOLTAGE);
	voltage >>= 3;

	return (voltage * 0.004);
}
float readShuntVoltage(void)
{
	float voltage = readInt_I2C(INA219_ADDRESS, INA219_REG_SHUNTVOLTAGE);
	return (voltage * 0.00001);			   //Переводим в вольты
}

float readShuntCurrent(void)
{
	return (readInt_I2C(INA219_ADDRESS, INA219_REG_CURRENT) * currentLSB);
}

float readBusPower(void)
{
	return (readInt_I2C(INA219_ADDRESS, INA219_REG_POWER) * powerLSB);
}


void Init_219(byte _adrr)
{
	Serial.print("Start Init INA219... 0x");	Serial.println(_adrr,HEX);

	reset219(_adrr);								   //Начальный сброс
	Serial.print("Gonfig registr start "); Serial.println(readInt_I2C(INA219_ADDRESS, INA219_REG_CONFIG), BIN);

	uint16_t config = 0;
	config = (INA219_CONFIG_BVOLTAGERANGE_16V |		 //BRNG (bus voltage range) – диапазон измерения шины; 	16 или 32 вольта
		INA219_CONFIG_GAIN_8_320MV | 			//	   PG – усиление и диапазон PGA.	в каком диапазоне и с каким коэфициентом
		INA219_CONFIG_BADCRES_12BIT_4S_2130US |	   //Разрешение 9 бит для напряжения на шине	BADC (bus ADC resolution/averaging) – разрешающая способность АЦП шины;
		INA219_CONFIG_SADCRES_12BIT_4S_2130US |	  //Разрешение 9 бит для напряжения на шунте (падения напряжения)  SADC (shunt ADC resolution/averaging) – разрешающая способность АЦП шунта;
		INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS);	//MODE – режим; Режим постоянных измерений	 /**< shunt and bus voltage continuous */
	//Serial.print("config= "); Serial.println(config, BIN);
	writeInt_i2c(INA219_ADDRESS, INA219_REG_CONFIG, config);

	Serial.print("Gonfig registr new "); Serial.println(readInt_I2C(INA219_ADDRESS, INA219_REG_CONFIG), BIN);

	calibrate(0.1, 3.2);     // Колибровка со значениями шунта и максимального тока какой может быть	 0,1 Ом и 3 Ампера

	Serial.print("readBusVoltage= ");
	Serial.println(readBusVoltage(),4);

	Serial.print("readShuntVoltage= ");
	Serial.println(readShuntVoltage(),4);

	Serial.print("readShuntCurrent= ");
	Serial.println(readShuntCurrent(), 4);

	Serial.print("readBusPower= ");
	Serial.println(readBusPower(), 4);	
	
	
	Serial.println("End Init INA219");




}

