// ����������� ���� SPI ��� slave � �������� ��������� �� ���������� �����
void Init_SPI_slave()
{
	// have to send on master in, *slave out*
	pinMode(MISO, OUTPUT);
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

// SPI interrupt routine ���������� �� ���������� �����
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
			if (data == 0x1A) //���� ������ ��������� ���� ��� ������� ��������
			{
				count++;                     // ����������� �������, ��� ������ ��� ������ ������ ����
				return;
			}
		}
		case 1:
		{
			if (data == 0x1B) // ���� ����� ��� ������� ������ ���� � ������ ������ ����������
			{
				count++;                     // ����������� �������, ��� ������ ��� ������ ������ ����
				return;
			}
			else
			{
				count = 0;                         // ������ ������ ���� �� ����������. ���������� ������� � �������� ��� �������
				Serial.println("Error");
				return;
			}
		}
		case 2:			    //������ �������
		{
			comanda = data;   //���������� �������
			MassivSend = (byte*)&myRemoteXY;  //���������� ����� ������ ��������� ��� ��������� �������� �� ������ ������� ����������� ������ �������, �������� ������ ��������� � ����� ������ � ������
			data_size = sizeof(myRemoteXY);   // ���������� ������ ���������
			//data_size = 3;
			//count++;                     // ����������� �������, ��� ������ ��� ������ ������ ����
			//return;

		}
	}

	if (count > 1 && count < data_size + 2)		// �������� ���������
	{
		flag_sendData = true;   //������ �������� ������. ������ �� ��������� � ����
		SPDR = MassivSend[count - 2];				  // ���������� � ������ ������ ���� ��������� ����� ����� ������ 3 �����(1 ����� � �������) � �� ������ ������� ��� ���� ��������� ��� �������� 2 ����� � �������
		chek_sum += MassivSend[count - 2];  // ������� ����������� �����
		count++;
		return;
	}
	if (count == data_size + 2)		// �������� ����������� �����
	{
		SPDR = chek_sum;
		chek_sum = 0;
		count++;
		return;
	}
	if (count == data_size + 3)		// �� ��������� ����� ���������� ������ ����� �������� ����������� �����
	{
		count = 0;             // ������� ��� ������, �������� ����� ����� ���������� ����
		flag_sendData = false;   //������ �������� ������. ������ �� ��������� � ����
		return;
	}

}

