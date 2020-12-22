
#define FREQ_WORK 10   // ������� ��������� �����. ����� ������ � ������ ���������


#define  FRONT 		   192.0		// ������ ����� ��������� � ������� ���������
#define  LENGTHS	   292.0		// ������ ����� ��������� � ��������
#define  MIDDLE        224.0		// ������ ����� ��������

#define  leg_A         55.0		// ������ ����� �� 0 ������ �� 1
#define  leg_B         74.0		// ������ ����� �� 1 ������ �� 2
#define  leg_C         128.0		// ������ ����� �� 2 ������ �� �����������


float hight = 1000.0;			// ��������� ������ ��� ������������, ����� ����� �������� � ����������
float hight_b = 20.0;			// ������ ������� ���� � ������ , ����� ����� �������� � ����������
float deltaX_dome = 0.0;	    // ��������� �� ����� ���������� �� ������ ������, ����� ����� �������� � ����������
float deltaY_dome = 50.0;	    // ��������� �� ����� ���������� �� ������ ������, ����� ����� �������� � ����������

#define  STEP_LONG      50		// ������ ���� ���� ������
#define  STEP         (STEP_LONG/2)		// ������ �������� ���� ���� ������	 ���������� "�" � ������

//#define MAX_ANGLE    ( atan(STEP / leg_A) * RAD_TO_DEG )  // ������� ������������ ���� ���������� �� �������� ��������� ��� ������ 

float max_angle = 0;

#define MAX_SPEED   0.4			   // ������������ �������� �������� ������	  ��� ��� ����� 55 �� 55,49 �������� �� 200 ����������
struct struct_xyz		   //��������� ��� ���������
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
			struct_xyz knot;        // ���������� ���� ���������
			struct_xyz dome;        // ���������� ������ ������	  ������������ ���� ������� ���������� ����� �����
			struct_xyz prop;        // ����� ���������� ����� ����� ������ ����
			float angle_A;			// ���� ��� ������ 0
			float angle_B;			// ���� ��� ������ 1
			float angle_C;			// ���� ��� ������ 2
			byte status;            // ������ ���� ������� ���������(0) ������� (1) ��� ��������� (2)
		};

		struct_leg Leg[6];  
};

struct struct_Dome
{
	struct_xyz point[6];        // ���������� ����� ����� ���������� ��� ������������ ����
};

LEG Body;	  // ��������� ������ ��� �������� ������� ����� ���� ��� 6 ����

struct_Dome virt_Leg;
byte napravl = 1;   //���������� �������� ������ 1 ��� ����� -1

 //������� ������� ���� 6 ����� ������������ ����. ���������� ����� ����� ����� ����� 2 ���������� 0 ���� ��� ������ � 1 ���� ����� �� ������� ������ � ����� ����������� ����� 2 � 3 �������
byte raschet_virt_leg(float delta_X_, float delta_Y_)    	//�� ���� ������ �������� �� ���� X � Y
{
	byte ret = 0;
	//-------------------------
	virt_Leg.point[0].x = 0;		//����� ����� 0 ��� ����� ������ �� ���������
	virt_Leg.point[0].y = 0;
	virt_Leg.point[0].z = 0;
	//-------------------------								 //����� ����� ���� ��� ����� ������ ������ �������� �� ������ ���� (������)
	virt_Leg.point[1].x = 0;
	virt_Leg.point[1].y = 0;
	virt_Leg.point[1].z = hight_b;
	//-------------------------
	virt_Leg.point[2].x += delta_X_ * napravl;	     // ����� ����� 2 ��� ����� ������� �������� �� ����������� �� ������� ���������  ���������� �� ���������� �������� ������ �(���) � ����� ��� 1 , ����� �(���) ����� ��� -1
	virt_Leg.point[2].y += delta_Y_ * napravl;
	virt_Leg.point[2].z = 0;

	if (abs(virt_Leg.point[2].x) >= abs(STEP))		// ���� ����� �� ������� ������, �� �� ���� �� ������� � �������� ��� ����� �� ������� � ������� ���� ���� ����������� ������� ���������� ����� 2 � 3
	{
		virt_Leg.point[2].x = STEP;
		ret = 1;
	}
	if (abs(virt_Leg.point[2].y) >= abs(STEP))		// ���� ����� �� ������� ������, �� �� ���� �� ������� � �������� ��� ����� �� ������� � ������� ���� ���� ����������� ������� ���������� ����� 2 � 3
	{
		virt_Leg.point[2].y = STEP;
		ret = 1;
	}
	//-------------------------
	virt_Leg.point[3].x = - virt_Leg.point[2].x;	     // ����� ����� 3 ��� ���������� ��������� ����� 2. ������ ������ ������� �� ������� �� ������� ��������� ����. � ���� ��� �����
	virt_Leg.point[3].y = - virt_Leg.point[2].y;

	// ��� � ������ �� ������� �������
	float x_diagonal = sqrt(pow(virt_Leg.point[2].x, 2) + pow(virt_Leg.point[2].y, 2));  // ��������� ��������� � �������������� �� �������� �� ������� ��������. ��� � ����� � ��� ������� ������ �� ������� ������
	double temp1 = (1 - (pow(x_diagonal, 2) / pow(STEP, 2))) * pow(hight_b,2); // 1 ����� � � �������� �������� �� � � ��������	 �������� �� b � ��������	   
	virt_Leg.point[3].z = sqrt(temp1);		// � ������ �� ����� ���������
	//-------------------------
											// ����� 4 ��� ����������� ������� ����� � �������� ����������� ������ ������ ������������ ���� ����� ������ ����������� ������ ������� ����  ��� ����� ����� �������� �� ��� � � Y	�� ������� ��������
	virt_Leg.point[4].x = sqrt(pow(STEP, 2) - pow(virt_Leg.point[2].y, 2));	   // ������ ��������� ��� �������� ����� ����
	virt_Leg.point[4].y = sqrt(pow(STEP, 2) - pow(virt_Leg.point[2].x, 2));
	virt_Leg.point[4].z = 0;
	//-------------------------
	virt_Leg.point[5].x = - virt_Leg.point[4].x;				// ����� 5 ��� ������� ����� 4 . 
	virt_Leg.point[5].y = - virt_Leg.point[4].y;
	virt_Leg.point[5].z = 0;
	//-------------------------
	return ret;
}

//������� ������� ������ ������ ����������. ������ ��� ����� � ��� ����, �� ������ �������� ���������� x,y,z ������ ���� �3 ��� ��������� ���� �������� �������� ������ ������������ ������� ������ �����. ��� �������� � ������ ������� �� �� ����� ����
struct_xyz praymaya_zadacha(float L1_, float A1_, float L2_, float A2_, float A3_)
{													// �������� �� 135 �������� ��� ��� ��� ����� ����� ������ ����������� ������������ ���� ����� 135 ��� �������� �� ���������
	struct_xyz ret;
	float _x = L1_ * cos(135 - A1_) + L2_ * cos(135 - A1_ + 135 - A2_);					   //x = XA + x' = L1*cos(Q1) + L2*cos(Q1+Q2)	// X- ������ � �����
	float _y = L1_ * sin(135 - A1_) + L2_ * sin(135 - A1_ + 135 - A2_);					   //y = YA + y' = L1*sin(Q1) + L2*sin(Q1+Q2)	// Y - ����� � ����

	float _x2 = _x * cos(A3_);
	float _z  = _x * sin(A3_);

	ret.z = _y;     //  ������� ��� ��� ����������� Y ��� Z � ������� ������
	ret.y = _x2;     //  ������� ��� ��� ����������� X ��� Y � ������� ������
	ret.x = _z;     //  ������� ��� ��� ����������� Z ��� X � ������� ������
}

void start_measurement()	 	 //��������� ��������� ���� � �������� �� �������  � ��������� ��������� ��������� � ������ ������
{
	//-------------------------
	Body.Leg[0].knot.x = -LENGTHS / 2;					 
	Body.Leg[0].knot.y = -FRONT / 2;
	Body.Leg[0].knot.z = hight;
	
	Body.Leg[0].dome.x = Body.Leg[0].knot.x - deltaX_dome;		 // ������ �������� ������������ ����� ��������� �������� ����� ��� ����. �������� ����������� ������ �� ���, ������ �����. ������ �����������.
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

void perviy_test()
{
	float speed_a = 0.05;  //�������� ��������
	float angle_a = 0;  // ���� ���������� �� �����  � �������� ����������� ��������
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
	setAngleForSend();		 //  ������������ ���� � ������ ��� ��������
}