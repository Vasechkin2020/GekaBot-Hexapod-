//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// ����������� ������ ���������� � ����������� ���������� RemoteXY 
#define REMOTEXY_MODE__HARDSERIAL

#include <RemoteXY.h>

// ��������� ���������� 
#define REMOTEXY_SERIAL Serial2
#define REMOTEXY_SERIAL_SPEED 115200


// ������������ ����������  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
{ 255,6,0,0,0,44,0,10,186,1,
3,134,2,6,59,11,118,26,4,160,
3,82,58,13,235,26,4,32,2,23,
10,54,2,26,4,32,51,23,10,54,
2,26,5,32,16,32,32,32,136,26,
31 };

// ��������� ���������� ��� ���������� � ������� ������ ���������� ���������� 
struct MyStruct {

	// input variables
	uint8_t Position; // =0 ���� ������������� � ��������� A, =1 ���� � ��������� B, =2 ���� � ��������� C, ... 
	int8_t Servo_0; // =-100..100 ��������� �������� 
	int8_t Servo_1; // =-100..100 ��������� �������� 
	int8_t Servo_2; // =-100..100 ��������� �������� 
	int8_t joystick_1_x; // =-100..100 ���������� x ��������� ��������� 
	int8_t joystick_1_y; // =-100..100 ���������� y ��������� ��������� 

	  // other variable
	uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////



#include <SPI.h>

// ����������� ���� SPI ��� slave � �������� ��������� �� ���������� �����
void Init_SPI_slave()
{
	SPI.setDataMode(SPI_MODE3);
	SPI.setBitOrder(MSBFIRST);
	//SPI.setClockDivider(SPI_CLOCK_DIV4);  // ������ SPI_CLOCK_DIV2 �������� 2 � ������� ������� 16 = 8 �����

	// have to send on master in, *slave out*
	pinMode(MISO, OUTPUT);
	// have to send on master in, *slave out*
	pinMode(MOSI, INPUT_PULLUP);

	pinMode(SS, INPUT_PULLUP);  // ��� ������ �� ���� �������

	// turn on SPI in slave mode
	SPCR |= 1 << (SPE);
	// turn on interrupts
	SPCR |= 1 << (SPIE);
}

byte* MassivSend = 0;   // ����� ������ ������� ��� �������� ���������
MyStruct myRemoteXY;


volatile byte count = 0;
volatile byte data = 0;
volatile byte flag_data = 0;
volatile byte chek_sum = 0;   // ���� ����������� �����

byte buffer[32];	  //����� � ������� �� 32 �����
byte comanda = 0;       // ���������� ������� ������� ������
byte data_size = 0;


bool flag_sendData; // ���� �������� ������, ���� ���� ������� �� ���������� ������
bool flag_goodCommand; // ���� ��� ������ ���������� �������
bool flag_bedCommand; // ���� ��� ������ �� ���������� �������

// SPI interrupt routine ���������� �� ���������� �����
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
			if (data == 0x1A) //���� ������ ��������� ���� ��� ������� ��������
			{
				count++;                     // ����������� �������, ��� ������ ��� ������ ������ ����
				digitalWrite(46, 0);
				return;
			}
		}
		case 1:
		{
			if (data == 0x1B) // ���� ����� ��� ������� ������ ���� � ������ ������ ����������
			{
				count++;                     // ����������� �������, ��� ������ ��� ������ ������ ����
				digitalWrite(46, 0);
				return;
			}
			else
			{
				count = 0;                         // ������ ������ ���� �� ����������. ���������� ������� � �������� ��� �������
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
		case 2:			    //������ �������
		{
			comanda = data;   //���������� �������
			if (comanda == 0x1D)
			{
				flag_goodCommand = true;
			}
			myRemoteXY = RemoteXY;        // ������ ����� ��������� ��� ��������  �� � ��������
			MassivSend = (byte*)&myRemoteXY;  //���������� ����� ������ ��������� ��� ��������� �������� �� ������ ������� ����������� ������ �������, �������� ������ ��������� � ����� ������ � ������
		}
	}

	if (count > 1 && count < data_size + 2)		// �������� ���������
	{
		flag_sendData = true;   //������ �������� ������. ������ �� ��������� � ����
		SPDR = MassivSend[count - 2];				  // ���������� � ������ ������ ���� ��������� ����� ����� ������ 3 �����(1 ����� � �������) � �� ������ ������� ��� ���� ��������� ��� �������� 2 ����� � �������
		chek_sum += MassivSend[count - 2];  // ������� ����������� �����
		count++;
		digitalWrite(46, 0);
		return;
	}
	if (count == data_size + 2)		// �������� ����������� �����
	{
		SPDR = chek_sum;
		chek_sum = 0;
		count++;
		digitalWrite(46, 0);
		return;
	}
	if (count == data_size + 3)		// �� ��������� ����� ���������� ������ ����� �������� ����������� �����
	{
		count = 0;             // �������� ��� ������, �������� ����� ����� ���������� ����
		flag_sendData = false;   //������ �������� ������. ������ �� ��������� � ����
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