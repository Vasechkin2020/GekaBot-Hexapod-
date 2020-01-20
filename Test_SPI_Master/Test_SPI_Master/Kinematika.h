
#define FREQ_WORK 10   // ������� ��������� �����. ����� ������ � ������ ���������


#define  FRONT 		   192.0		// ������ ����� ��������� � ������� ���������
#define  LENGTHS	   292.0		// ������ ����� ��������� � ��������
#define  MIDDLE        224.0		// ������ ����� ��������

#define  leg_A         55.0		// ������ ����� �� 0 ������ �� 1
#define  leg_B         74.0		// ������ ����� �� 1 ������ �� 2
#define  leg_C         128.0		// ������ ����� �� 2 ������ �� �����������

#define  HEIGHT         1000.0	// ������ ��� ������������
#define  STEP_LONG      50		// ������ ���� ���� ������
#define  STEP         (STEP_LONG/2)		// ������ �������� ���� ���� ������

//#define MAX_ANGLE    ( atan(STEP / leg_A) * RAD_TO_DEG )  // ������� ������������ ���� ���������� �� �������� ��������� ��� ������ 

float max_angle = 0;

#define MAX_SPEED   0.4			   // ������������ �������� �������� ������	  ��� ��� ����� 55 �� 55,49 �������� �� 200 ����������

class LEG
{
	public:
		struct struct_xyz		   //��������� ��� ���������
		{
			float x;
			float y;
			float z;
		};
		struct struct_leg
		{
			struct_xyz knot;        // ���������� ���� ���������
			float angle_A;			// ���� ��� ������ 0
			float angle_B;			// ���� ��� ������ 1
			float angle_C;			// ���� ��� ������ 2
			struct_xyz coordinats;  //������ ��� �������� ��������� ����� ����� ������ ����
		};

		struct_leg Leg[6];  
};

LEG Body;	  // ��������� ������ ��� �������� ������� ����� ���� ��� 6 ����


void start_measurement()	 	 //��������� ���������� ���������
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

float get_angle_for_speed(float _speed, float _angle)	  // ���������� ���� �� ������� ����� ��������� ���������� 0 �� ���� ��������� ��� �������� �������� � ��������� ���� ��������
{
	if (_speed > MAX_SPEED) _speed = MAX_SPEED; // ������������ �������� �������� ������	

	float way = _speed * 1000;  //��������� ����� � ������� � ��������� � �������, ��� ��� � ��� ��� ������� � ����������. �������� ���� ������� ����� ������ �� �������
	float proekcia = way * cos(_angle * DEG_TO_RAD);  // ������� �������� �� ��� � ������ ����	������� �������� � �������. �++ ��� ������� � ��������
	float step = proekcia / FREQ_WORK;  // ������� ����������� ��� ��� �������� ������� ������ ����� ��������� � ����� ���������
	float angl = atan(step / leg_A) * RAD_TO_DEG;  // ������� ���� ��� 0 ������ ��� ������������ ���� 
	return angl;
}										

float get_max_angle(float _angle)
{
	float way = STEP * cos(_angle * DEG_TO_RAD);  // ������� �������� �� ��� � ������ ������������� ���� ��������� � �������. �++ ��� ������� � ��������
	float max_angle = (atan(way / leg_A) * RAD_TO_DEG); //������� �� ����� ������������ ���� ����� ����������� �����
	return max_angle;
}