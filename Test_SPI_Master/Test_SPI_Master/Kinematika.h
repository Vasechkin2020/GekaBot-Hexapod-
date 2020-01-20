
#define FREQ_WORK 10   // Частота основного цикла. Опрос данных и расчет координат


#define  FRONT 		   192.0		// Ширина между передними и задними момторами
#define  LENGTHS	   292.0		// Длинна между передними и заданими
#define  MIDDLE        224.0		// Ширина между средними

#define  leg_A         55.0		// Длинна плеча от 0 мотора до 1
#define  leg_B         74.0		// Длинна плеча от 1 мотора до 2
#define  leg_C         128.0		// Длинна плеча от 2 мотора до поверхности

#define  HEIGHT         1000.0	// Высота над поверхностью
#define  STEP_LONG      50		// Длинна шага ноги робота
#define  STEP         (STEP_LONG/2)		// Длинна половина шага ноги робота

//#define MAX_ANGLE    ( atan(STEP / leg_A) * RAD_TO_DEG )  // Находим максимальный угол отклонения от среднего положения для мотора 

float max_angle = 0;

#define MAX_SPEED   0.4			   // максимальная скорость движения робота	  Это для плеча 55 мм 55,49 градусов за 200 милисекунд

class LEG
{
	public:
		struct struct_xyz		   //Структура для координат
		{
			float x;
			float y;
			float z;
		};
		struct struct_leg
		{
			struct_xyz knot;        // Координаты узла крепления
			float angle_A;			// Угол для мотора 0
			float angle_B;			// Угол для мотора 1
			float angle_C;			// Угол для мотора 2
			struct_xyz coordinats;  //Массив для хранения координат точек опоры каждой ноги
		};

		struct_leg Leg[6];  
};

LEG Body;	  // Экземпляр класса для хранения текущих углов всех ног 6 штук


void start_measurement()	 	 //Начальные вычисления координат
{
	Body.Leg[0].knot.x = -LENGTHS / 2;
	Body.Leg[0].knot.y = -FRONT / 2;
	Body.Leg[0].knot.z = HEIGHT;
	Body.Leg[0].angle_A = 135;
	Body.Leg[0].angle_B = 210;
	Body.Leg[0].angle_C = 10;

	Body.Leg[1].knot.x = 0;
	Body.Leg[1].knot.y = -MIDDLE / 2;
	Body.Leg[1].knot.z = HEIGHT;
	Body.Leg[1].angle_A = 135;
	Body.Leg[1].angle_B = 210;
	Body.Leg[1].angle_C = 10;

	Body.Leg[2].knot.x = LENGTHS / 2;
	Body.Leg[2].knot.y = -FRONT / 2;
	Body.Leg[2].knot.z = HEIGHT;
	Body.Leg[2].angle_A = 135;
	Body.Leg[2].angle_B = 210;
	Body.Leg[2].angle_C = 10;
	
	Body.Leg[3].knot.x = -LENGTHS / 2;
	Body.Leg[3].knot.y = FRONT / 2;
	Body.Leg[3].knot.z = HEIGHT;
	Body.Leg[3].angle_A = 135;
	Body.Leg[3].angle_B = 210;
	Body.Leg[3].angle_C = 10;

	Body.Leg[4].knot.x = 0;
	Body.Leg[4].knot.y = MIDDLE / 2;
	Body.Leg[4].knot.z = HEIGHT;
	Body.Leg[4].angle_A = 135;
	Body.Leg[4].angle_B = 210;
	Body.Leg[4].angle_C = 10;

	Body.Leg[5].knot.x = LENGTHS / 2;
	Body.Leg[5].knot.y = FRONT / 2;
	Body.Leg[5].knot.z = HEIGHT;
	Body.Leg[5].angle_A = 135;
	Body.Leg[5].angle_B = 210;
	Body.Leg[5].angle_C = 10;
}

float get_angle_for_speed(float _speed, float _angle)	  // Возврашает угол на который нужно повернуть сервомотор 0 за одну итеррацию для заданной скорости и заданного угла поворота
{
	if (_speed > MAX_SPEED) _speed = MAX_SPEED; // максимальная скорость движения робота	

	float way = _speed * 1000;  //Переводим метры в секунду в милиметры в секунду, так как у нас все размеры в милиметрах. Получаем путь который нужно пройти за секунду
	float proekcia = way * cos(_angle * DEG_TO_RAD);  // Находим проекцию на ось Х нашего пути	градусы пеерводи в радианы. С++ все считает в радианах
	float step = proekcia / FREQ_WORK;  // Находим минимальный шаг для заданной частоты работы чтобы двигаться с такой скоростью
	float angl = atan(step / leg_A) * RAD_TO_DEG;  // Находим угол для 0 мотора для минимального шага 
	return angl;
}										

float get_max_angle(float _angle)
{
	float way = STEP * cos(_angle * DEG_TO_RAD);  // Находим проекцию на ось Х нашего максимального шага переводим в радианы. С++ все считает в радианах
	float max_angle = (atan(way / leg_A) * RAD_TO_DEG); //Находим на какой максимальный угол может отклониться мотор
	return max_angle;
}