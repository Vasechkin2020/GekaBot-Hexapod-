//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// определение режима соединения и подключение библиотеки RemoteXY 
#define REMOTEXY_MODE__HARDSERIAL

#include <RemoteXY.h>

// настройки соединения 
#define REMOTEXY_SERIAL Serial1
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
struct MyStruct  {

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

void PrintRemoteXY_variable()
{
	//RemoteXY.level_1 = RemoteXY.slider_1;
	Serial.print(" P= ");Serial.print(RemoteXY.Position);
	Serial.print(" S_0= ");Serial.print(RemoteXY.Servo_0);
	Serial.print(" S_1= ");Serial.print(RemoteXY.Servo_1);
	Serial.print(" S_2= ");Serial.print(RemoteXY.Servo_2);

	//Serial.print(" 1_x = ");Serial.print(RemoteXY.joystick_1_x);
	//Serial.print(" 1_y = ");Serial.println(RemoteXY.joystick_1_y);

	//Serial.print(" Connect = ");Serial.println(RemoteXY.connect_flag);
	Serial.println("");

	Serial2.print(" P= ");Serial2.print(RemoteXY.Position);
	Serial2.print(" S_0= ");Serial2.print(RemoteXY.Servo_0);
	Serial2.print(" S_1= ");Serial2.print(RemoteXY.Servo_1);
	Serial2.print(" S_2= ");Serial2.print(RemoteXY.Servo_2);
	Serial2.println("");

	delay(100);
}

