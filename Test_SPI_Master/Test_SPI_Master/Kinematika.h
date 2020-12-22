
#define FREQ_WORK 10   // Частота основного цикла. Опрос данных и расчет координат


#define  FRONT 		   192.0		// Ширина между передними и задними момторами
#define  LENGTHS	   292.0		// Длинна между передними и заданими
#define  MIDDLE        224.0		// Ширина между средними

#define  leg_A         55.0		// Длинна плеча от 0 мотора до 1
#define  leg_B         74.0		// Длинна плеча от 1 мотора до 2
#define  leg_C         128.0		// Длинна плеча от 2 мотора до поверхности


float hight = 1000.0;			// Стартовая Высота над поверхностью, потом может меняться в переменной
float hight_b = 20.0;			// Высота подьема ноги в элипсе , потом может меняться в переменной
float deltaX_dome = 0.0;	    // Растояние от точки крепеления до центра купола, потом может меняться в переменной
float deltaY_dome = 50.0;	    // Растояние от точки крепеления до центра купола, потом может меняться в переменной

#define  STEP_LONG      50		// Длинна шага ноги робота
#define  STEP         (STEP_LONG/2)		// Длинна половина шага ноги робота	 переменная "а" в элипсе

//#define MAX_ANGLE    ( atan(STEP / leg_A) * RAD_TO_DEG )  // Находим максимальный угол отклонения от среднего положения для мотора 

float max_angle = 0;

#define MAX_SPEED   0.4			   // максимальная скорость движения робота	  Это для плеча 55 мм 55,49 градусов за 200 милисекунд
struct struct_xyz		   //Структура для координат
	{
		float x;
		float y;
		float z;
	};

class LEG
{
	public:
		struct struct_leg
		{
			struct_xyz knot;        // Координаты узла крепления
			struct_xyz dome;        // Координаты центра купола	  Относительно него считаем координаты почки опоры
			struct_xyz prop;        // Опора координаты точки опоры каждой ноги
			float angle_A;			// Угол для мотора 0
			float angle_B;			// Угол для мотора 1
			float angle_C;			// Угол для мотора 2
			byte status;            // Статус ноги Базовое положение(0) Опорная (1) или Воздушная (2)
		};

		struct_leg Leg[6];  
};

struct struct_Dome
{
	struct_xyz point[6];        // Координаты Шести точек поситанные для виртукальной ноги
};

LEG Body;	  // Экземпляр класса для хранения текущих углов всех ног 6 штук

struct_Dome virt_Leg;
byte napravl = 1;   //Напрвление движения вперед 1 или назад -1

 //Функция расчета всех 6 точек виартуальной ноги. координаты точки опоры точка номер 2 Возвращаем 0 если все хорошо и 1 нсли дошли до границы купола и нужно переставить точки 2 и 3 местами
byte raschet_virt_leg(float delta_X_, float delta_Y_)    	//На вход подаем смещение по осям X и Y
{
	byte ret = 0;
	//-------------------------
	virt_Leg.point[0].x = 0;		//ТОчка номер 0 это центр купола по основанию
	virt_Leg.point[0].y = 0;
	virt_Leg.point[0].z = 0;
	//-------------------------								 //Точка номер один это точка центра купола поднятая на высоту ноги (элипса)
	virt_Leg.point[1].x = 0;
	virt_Leg.point[1].y = 0;
	virt_Leg.point[1].z = hight_b;
	//-------------------------
	virt_Leg.point[2].x += delta_X_ * napravl;	     // Точка номер 2 это точка которая движется по поверхности на которую опираемся  умноженная на напрвление движения вперед и(или) в право это 1 , назад и(или) влево это -1
	virt_Leg.point[2].y += delta_Y_ * napravl;
	virt_Leg.point[2].z = 0;

	if (abs(virt_Leg.point[2].x) >= abs(STEP))		// Если дошли до границы купола, то за него не заходим а сообщаем что дошли до границы и уровнем выше надо переставить местами ккординаты точек 2 и 3
	{
		virt_Leg.point[2].x = STEP;
		ret = 1;
	}
	if (abs(virt_Leg.point[2].y) >= abs(STEP))		// Если дошли до границы купола, то за него не заходим а сообщаем что дошли до границы и уровнем выше надо переставить местами ккординаты точек 2 и 3
	{
		virt_Leg.point[2].y = STEP;
		ret = 1;
	}
	//-------------------------
	virt_Leg.point[3].x = - virt_Leg.point[2].x;	     // Точка номер 3 это зеркальное положение точки 2. только высоту считаем по функции по которой переносим ногу. у меня это Элипс
	virt_Leg.point[3].y = - virt_Leg.point[2].y;

	// ТУТ Я СЧИТАЮ ПО ФУНКЦИИ ЭЛЛИПСА
	float x_diagonal = sqrt(pow(virt_Leg.point[2].x, 2) + pow(virt_Leg.point[2].y, 2));  // вычисляем диагональ в прямоугольнике по сторонам по теореме Пифагора. Это и будет Х для расчета высоты по функции Элипса
	double temp1 = (1 - (pow(x_diagonal, 2) / pow(STEP, 2))) * pow(hight_b,2); // 1 минус х в квадрате деленная на а в квадрате	 умножить на b в квадрате	   
	virt_Leg.point[3].z = sqrt(temp1);		// и корень из этого выражения
	//-------------------------
											// Точка 4 это максимально дальняя точка в заданном направление внутри купола используется если нужно встать максимально широко разведя лапы  Тут нужно найти проекции на оси Х и Y	по теореме Пифагора
	virt_Leg.point[4].x = sqrt(pow(STEP, 2) - pow(virt_Leg.point[2].y, 2));	   // длинна Диагонали тут половина длины шага
	virt_Leg.point[4].y = sqrt(pow(STEP, 2) - pow(virt_Leg.point[2].x, 2));
	virt_Leg.point[4].z = 0;
	//-------------------------
	virt_Leg.point[5].x = - virt_Leg.point[4].x;				// Точка 5 это зеркало точки 4 . 
	virt_Leg.point[5].y = - virt_Leg.point[4].y;
	virt_Leg.point[5].z = 0;
	//-------------------------
	return ret;
}

//Функция решения прямой задачи кинематики. задаем два плеча и два угла, на выходе получаем координаты x,y,z третий угол А3 это возможный угол поворота нулевого мотора относительно расчета строго враво. Для передний и задних моторов он не равен нулю
struct_xyz praymaya_zadacha(float L1_, float A1_, float L2_, float A2_, float A3_)
{													// Отнимаем по 135 градусов так как под таким углом моторы установлены относительно друг друга 135 это середина их диапазона
	struct_xyz ret;
	float _x = L1_ * cos(135 - A1_) + L2_ * cos(135 - A1_ + 135 - A2_);					   //x = XA + x' = L1*cos(Q1) + L2*cos(Q1+Q2)	// X- вправо и влево
	float _y = L1_ * sin(135 - A1_) + L2_ * sin(135 - A1_ + 135 - A2_);					   //y = YA + y' = L1*sin(Q1) + L2*sin(Q1+Q2)	// Y - вверх и вниз

	float _x2 = _x * cos(A3_);
	float _z  = _x * sin(A3_);

	ret.z = _y;     //  смотрим как оси расположены Y это Z в системе робота
	ret.y = _x2;     //  смотрим как оси расположены X это Y в системе робота
	ret.x = _z;     //  смотрим как оси расположены Z это X в системе робота
}

void start_measurement()	 	 //Начальные Установки ноги в градусах на моторах  и установка координат крепления и центра купола
{
	//-------------------------
	Body.Leg[0].knot.x = -LENGTHS / 2;					 
	Body.Leg[0].knot.y = -FRONT / 2;
	Body.Leg[0].knot.z = hight;
	
	Body.Leg[0].dome.x = Body.Leg[0].knot.x - deltaX_dome;		 // Задаем смещение относительно точки крепления учитывая какая где нога. Передние отодвигаюся вперед от оси, задние назад. Смотри внимательно.
	Body.Leg[0].dome.y = Body.Leg[0].knot.y - deltaY_dome;
	Body.Leg[0].dome.z = 0;

	Body.Leg[0].angle_A = 135;
	Body.Leg[0].angle_B = 210;
	Body.Leg[0].angle_C = 10;
	Body.Leg[0].status = 0;
	//-------------------------
	Body.Leg[1].knot.x = 0;
	Body.Leg[1].knot.y = -MIDDLE / 2;
	Body.Leg[1].knot.z = hight;

	Body.Leg[1].dome.x = Body.Leg[1].knot.x;
	Body.Leg[1].dome.y = Body.Leg[1].knot.y - deltaY_dome;
	Body.Leg[1].dome.z = 0;

	Body.Leg[1].angle_B = 210;
	Body.Leg[1].angle_C = 10;
	Body.Leg[1].status = 0;
	//-------------------------
	Body.Leg[2].knot.x = LENGTHS / 2;
	Body.Leg[2].knot.y = -FRONT / 2;
	Body.Leg[2].knot.z = hight;

	Body.Leg[2].dome.x = Body.Leg[2].knot.x + deltaX_dome;
	Body.Leg[2].dome.y = Body.Leg[2].knot.y - deltaY_dome;
	Body.Leg[2].dome.z = 0;

	Body.Leg[2].angle_A = 135;
	Body.Leg[2].angle_B = 210;
	Body.Leg[2].angle_C = 10;
	Body.Leg[2].status = 0;
	//-------------------------
	Body.Leg[3].knot.x = -LENGTHS / 2;
	Body.Leg[3].knot.y = FRONT / 2;
	Body.Leg[3].knot.z = hight;

	Body.Leg[3].dome.x = Body.Leg[3].knot.x - deltaX_dome;
	Body.Leg[3].dome.y = Body.Leg[3].knot.y + deltaY_dome;
	Body.Leg[3].dome.z = 0;

	Body.Leg[3].angle_A = 135;
	Body.Leg[3].angle_B = 210;
	Body.Leg[3].angle_C = 10;
	Body.Leg[3].status = 0;
	//-------------------------
	Body.Leg[4].knot.x = 0;
	Body.Leg[4].knot.y = MIDDLE / 2;
	Body.Leg[4].knot.z = hight;

	Body.Leg[4].dome.x = Body.Leg[4].knot.x;
	Body.Leg[4].dome.y = Body.Leg[4].knot.y + deltaY_dome;
	Body.Leg[4].dome.z = 0;

	Body.Leg[4].angle_A = 135;
	Body.Leg[4].angle_B = 210;
	Body.Leg[4].angle_C = 10;
	Body.Leg[4].status = 0;
	//-------------------------
	Body.Leg[5].knot.x = LENGTHS / 2;
	Body.Leg[5].knot.y = FRONT / 2;
	Body.Leg[5].knot.z = hight;

	Body.Leg[5].dome.x = Body.Leg[5].knot.x + deltaX_dome;
	Body.Leg[5].dome.y = Body.Leg[5].knot.y + deltaY_dome;
	Body.Leg[5].dome.z = 0;

	Body.Leg[5].angle_A = 135;
	Body.Leg[5].angle_B = 210;
	Body.Leg[5].angle_C = 10;
	Body.Leg[5].status = 0;
	//-------------------------
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

void perviy_test()
{
	float speed_a = 0.05;  //Скорость движения
	float angle_a = 0;  // Угол отклонения от прямо  в градусах направление движения
	int num_leg = 1;
	Body.Leg[num_leg].angle_A += get_angle_for_speed(speed_a, angle_a) * napr;
	Body.Leg[0].angle_A = Body.Leg[num_leg].angle_A + 45;
	Body.Leg[2].angle_A = Body.Leg[num_leg].angle_A - 45;

	Body.Leg[3].angle_A = 270 - Body.Leg[0].angle_A;
	Body.Leg[4].angle_A = 270 - Body.Leg[num_leg].angle_A;
	Body.Leg[5].angle_A = 270 - Body.Leg[2].angle_A;
	//Body.Leg[num_leg].angle_C += get_angle_for_speed(speed_a, angle_a) * napr;
	//Body.Leg[4].angle_B += get_angle_for_speed(speed_a, angle_a) * napr;

	SerialUSB.print(" step_angl= "); SerialUSB.print(get_angle_for_speed(speed_a, angle_a));
	SerialUSB.print(" angl= ");SerialUSB.print(Body.Leg[4].angle_A);
	float max_angle = get_max_angle(angle_a);
	SerialUSB.print(" MAX angl= ");SerialUSB.println(max_angle);

	if (Body.Leg[num_leg].angle_A > (135 + max_angle))
	{
		napr = -1;
	}
	if (Body.Leg[num_leg].angle_A < (135 - max_angle))
	{
		napr = 1;
	}
	setAngleForSend();		 //  Переписываем углы в массив для передачи
}