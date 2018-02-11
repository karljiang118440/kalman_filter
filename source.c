/*
* bms.c
*
*  Created on: 2018��1��25��
*      Author: karl.jiang
*/



#include <stdio.h>
#include<math.h>
#include<stdint.h>


//float   Battery_Soc(float AH_Data,/* float BAT_Curr,*/ float BAT_Vtge, float BAT_Tempp)
int main(void)

//int main()
{
	int i, j;

	float qcc;//����ڵ���ΪI������ʱ��t���ŵ���
	float soc;
	float k, pc, soc0;



//********����ʹ�ã����뼤���ź�*****************


	//float vol[5] = { 4.22, 3.861, 3.783, 3.685, 3.62 };
	float vol_lut[2][5] = {
		{ 4.22, 3.861, 3.783, 3.685, 3.62 },
		{ 0, 5100, 6900, 8700, 10200 }
	};

	float soc_kalman_item[5];
	float soc_item[5];

	/*float R2;
	float dt;
	float C;*/
	//float temp;
///////////////////////////////////////////////
	float Qcc_discharge_k;

	float Qcc_temp_k;


	float voltages[2][30] = {
		{ 100, 99, 97, 95, 92, 90, 87, 85, 80, 75, 70, 65, 60, 55, 50, 45, 42, 40, 35, 30, 25, 20, 15, 12, 10, 8, 5, 3, 1, 0 },
		{ 4.2, 4.15, 4.14, 4.12, 4.10, 4.08, 4.05, 4.03, 3.97, 3.90, 3.87, 3.84, 3.81, 3.79, 3.77, 3.76, 3.75, 3.74,
		3.73, 3.72, 3.71, 3.70, 3.69, 3.66, 3.65, 3.64, 3.63, 3.61, 3.59, 3.58 }
	};



	//******��ͬ�ŵ籶�ʶ���������***************
	float Qcc_discharge_lut[2][10] = {
		{  0.95, 0.948, 0.92,   0.83,   0.75,   0.72,   0.3,   0.25,  0.2, 0.18},
		{ 0.1,   0.11,     0.3,    0.4,    0.5,    0.6,    1,    2,    3,  3.1 },
		//{ 1, 0.95, 0.9, 0.88, 0.4, 0.35, 0.3, 0.25 },
		//{ 0.1,0.2,  0.31,   0.5,    0.8,     1,      2,       3},
		//{ 0.9, 0.8, 0.6, 0.5, 0.4, 0.3, 0.3, 0.25}
	};

	//printf("BAT_Curr=%f\n", BAT_Curr);

	/*****��ͬ��籶�ʶ���������***************
	float Qcc_charge_lut[2][10] = {
	{ 0.1,0.5,1,1.5,2,2.5,5,8,10},
	{  1  ,0.9,0.8,0.7,0.6,0.5,0.4,0.3,0.2,0.1 }
	};*/



	//******��ͬ�¶������¶���������***************
	float Qcc_temp_lut[2][5] = {
		{ 0.6, 0.85, 0.9, 1, 1.2 },
		{ -20, -10, 0, 25, 60 }
	};

	/******����Էŵ������¶���������***************
	float Qcc_temp_lut[2][5] = {
		{ 0.6, 0.85, 0.9, 1, 1.2 },
		{ -20, -10, 0, 25, 60 }
	};*/



	//*****������� �ͷŵ�����Ĺ�ϵ**********
	/*float R_temp_lut[2][30] = {
		{ 100, 99, 97, 95, 92, 90, 87, 85, 80, 75, 70, 65, 60, 55, 50, 45, 42, 40, 35, 30, 25, 20, 15, 12, 10, 8, 5, 3, 1, 0 },
		{ 4.2, 4.15, 4.14, 4.12, 4.10, 4.08, 4.05, 4.03, 3.97, 3.90, 3.87, 3.84, 3.81, 3.79, 3.77, 3.76, 3.75, 3.74,
		3.73, 3.72, 3.71, 3.70, 3.69, 3.66, 3.65, 3.64, 3.63, 3.61, 3.59, 3.58 }
	};*/




///***********�Զ�����̲�����************
	for ( i = 0; i < 5; i++)
	{

	float t =vol_lut[1][i];//ͳ�Ƶ�طŵ�ʱ�䣬��λ��

	float r = 0.35;//�������

	float QCC = 3223;//��ص���mAH

	float V0 = 3.50;///��ֹ��ѹ

	float DischargeRate;

	float Cdt = 10;/////�ŵ�ʱ�� s

	float coutbat = 1;	



	float voltage;//��·��ѹֵ����ֵֻ����û�и�������¼���
	float BAT_Vtge = vol_lut[0][i]; //�޷�ȷ�������ص�ʱ���ֵ
	//float BAT_Vtge = 3.6; //�޷�ȷ�������ص�ʱ���ֵ
	printf("vol_lut[0][i]=%f\n", vol_lut[0][i]);
	printf("t=%f\n", t);
	float temp = 20;
	float BAT_Curr = 0.5;
	float R2 = 0.1;
	float dt = 1;
	float C = 0.01;
	float socinital = 100;
	//float socinital;



	float SOC_lut;
	float k1;
	//float vol[30];
	//float vol[30] = {};




/////////////////////////////////////////

	QCC = 3223;

	DischargeRate = BAT_Curr*1000 / QCC;  //

	//printf("BAT_Curr=%f\n", BAT_Curr);


	voltage = BAT_Vtge + BAT_Curr *R2 + BAT_Curr*dt*C;
	//voltage = 4.04;
	printf("voltage=%f\n", voltage);

	//  ������������
/*	if (4.26 >= voltage && voltage >= 4.2)   soc0 = 100;

	else*/

	for (j = 30; j > 0; j--)
	{  

		//voltage[30] = {}
		if (voltages[1][j] >= voltage && voltage >= voltages[1][j + 1])
			break;
		k = (voltages[0][j + 1] - voltages[0][j]) / (voltages[1][j + 1] - voltages[1][j]);
		SOC_lut = voltages[0][j] + k*(voltage - voltages[1][j]);		
	}


/*if (4.3 >= voltage && voltage >=4.2)   soc0 =100;

if (4.2 > voltage && voltage >= 4.08)   soc0 = soc0 + 3;

else if (4.08 > voltage && voltage >= 4.03) soc0 = soc0 + 5;

else if (4.03 > voltage && voltage >= 3.97) soc0 = soc0 +5;

else if (3.87 >= voltage && voltage >= 3.81) soc0 = soc0 - 2.5;

else if (3.81 > voltage && voltage >= 3.77) soc0 = soc0 - 5;

else if (3.72 >= voltage && voltage >= 3.69) soc0 = soc0 - 4;

else if (3.69 > voltage && voltage >= 3.65) soc0 = soc0 + 2;

else if (3.65 > voltage && voltage >= 3.63) soc0 = soc0 - 4;*/



	printf("SOC_lut=%f\n", SOC_lut);

	// ���ȷ���¶Ȳ���
	for (j = 0; j <= 5; j++)
	{
		if (Qcc_temp_lut[1][j] <= temp && temp <= Qcc_temp_lut[1][j + 1])
			break;
		k = (Qcc_temp_lut[0][j + 1] - Qcc_temp_lut[0][j]) / (Qcc_temp_lut[1][j + 1] - Qcc_temp_lut[1][j]);
		Qcc_temp_k = Qcc_temp_lut[0][j + 1] + k*(temp - Qcc_temp_lut[1][j]);
		//soc = soc0 - pc*100;

	}

	// ��ͬ�ŵ籶�ʶ���������
	for (j = 0; j <= 10; j++)
	{
		if (Qcc_discharge_lut[1][j] <= DischargeRate && DischargeRate <= Qcc_discharge_lut[1][j+1])
			break;
		k1 = (Qcc_discharge_lut[0][j + 1] - Qcc_discharge_lut[0][j]) / (Qcc_discharge_lut[1][j + 1] - Qcc_discharge_lut[1][j]);
		Qcc_discharge_k = Qcc_discharge_lut[0][j] + k1*(DischargeRate - Qcc_discharge_lut[1][j]);
		
		//soc = soc0 - pc*100;

	}

	//printf("k1=%f\n", k1);
	//printf("k=%f\n", k);
	//printf("Qcc_discharge_lut[1][j]=%f\n", Qcc_discharge_lut[1][j]);

	// ���㲿��

	//���жϵ����Ƿ�Ϊ0������Ϊ 0 �����в�����
	if (BAT_Curr != 0)
	{

		QCC = QCC*Qcc_discharge_k*Qcc_temp_k;
	}

	else
	{
		QCC = QCC * Qcc_temp_k;
	}




	//qcc = 1000 * BAT_Curr*t * 10 / 3600;

	qcc = 1000 * BAT_Curr*t/3600;

	//printf("qcc=%f\n", qcc);

	pc = qcc / QCC;

	soc = socinital - pc * 100;//�� socinital ֻ�ܴ��������

	soc_item[i] = soc;

	coutbat++;

	//return soc;

	//void Kalman_Filter(float SOC, float Uc)//SOC Ϊ����ó���ֵ��OCVΪ��·��ѹֵ

	//{
		float SOC0;   //z���ȹ��Ƶ�ֵ

		//float dt = 1;  //����ʱ�䣬dtΪ�˲�������ʱ�� t��

		float PCt_0;
		//float current;
		
		float soc_err;
		float E;
		//float Qcc;
		//float Uc;
		//float R2 = 0.01;
		//float C = 1;
		//float i;
		float PCt_1;
		float R_angle=0.1;
		float soc_kalman;

		char C_0 = 1;  //H �����һ����

		//float soc_err = 0.1;//����Ͱ�ʱ���ַ��õ��Ľ�����

		float K_0 = 0, K_1 = 0, t_0 = 0, t_1 = 0; //K�ǿ������棬t���м����

		float Pdot[4] = { 0, 0, 0, 0 }; //����P������м����

		float PP[2][2] = { { 1, 0 }, { 0, 1 } }; //��ʽ��P����x��Э����

		float A[2][2] = { { 1, 0 }, { 0, 1 } }; //A ����ϵ��

		float Q[2][2] = { { 0.01, 0.01 }, { 0.01, 0.01 } }; //Q ����ϵ��

		float B[2] = { 0, 0 }; //B ����ϵ��

		//SOC = SOC - dt*current / QCC;     // SOC ��ʱ���ַ�����

		// Uc = Uc*exp(-dt / (R2*C)) + BAT_Curr*R2*(1 - exp(-dt / (R2*C)); // һ�׵��ģ�ͣ�
		// Uc = Uc * exp (-dt / (R2 * C) + BAT_Curr * R2 *(1-exp(-dt / (R2 * C)) ;


		//  ������صļ�����***********************************

		//SOC_lut = 58.6;
		SOC0 = soc;
		soc_err = SOC_lut - SOC0;


		//  ����ϵ������***********************************

		A[0][0] = 1;
		A[0][1] = 0;
		A[1][0] = 0;
		A[1][1] = exp(-dt / (R2*C));

		B[0] = -dt / QCC;
		//	B[1] = R2*(1 - exp(-dt / (R2*C));
		B[1] = R2 * (1 - exp(-dt / (R2 * C)));

		//  ����Qϵ������**********************************

		Q[0][0] = 0.01;
		Q[0][1] = 0.01;
		Q[1][0] = 0.01;
		Q[1][1] = 0.01;

		// PdotΪP���м�������****************************

		Pdot[0] = PP[0][0];
		Pdot[1] = 0;
		Pdot[2] = 0;
		Pdot[3] = PP[1][1] * exp(-dt / (R2*C))*exp(-dt / (R2*C));


		// P[][]�������****************************

		PP[0][0] += Pdot[0] * dt + Q[0][0];
		PP[0][1] += Pdot[1] * dt + Q[0][1];
		PP[1][0] += Pdot[2] * dt + Q[1][0];
		PP[1][1] += Pdot[3] * dt + Q[1][1];

		// KgУ������****************************

		PCt_0 = C_0*PP[0][0];
		PCt_1 = C_0*PP[1][1];
		E = R_angle + C_0 * PCt_0;
		K_0 = PCt_0 / E;
		K_1 = PCt_1 / E;

		t_0 = PCt_0,
		t_1 = C_0 * PP[0][1];

		PP[0][0] -= K_0 * t_0;
		PP[0][1] -= K_0 * t_1;
		PP[1][0] -= K_1 * t_0;
		PP[1][1] -= K_1 * t_1;


		
		soc_kalman = soc + K_0 * soc_err;
		
		soc_kalman_item[i] = soc_kalman;

		//return soc;
		printf("kalman=%f\n", soc_kalman_item[i]);
		//getchar();

		printf("Qcc_temp_k=%f\n", Qcc_temp_k);

		printf("Qcc=%f\n", QCC);
		//getchar();

		printf("DischargeRate=%f\n", DischargeRate);

		printf("Qcc_discharge_k=%f\n", Qcc_discharge_k);
		//getchar();

		printf("AH=%f\n", soc_item[i]);

		//getchar();
		//system("pause");

	}
	//printf("AH=%f\n", soc_item[i]);
	}
	
//}




/*float Battery_Soh(float BAT_Curr, float batcyc, float BAT_Tempp)
{

	// batcyc=50;
	// BAT_Tempp=300
	//BAT_Curr=2;//input T,I;


	float  Cr, A, B, C, K11, K12, K21, K22;


	float a = 2679.4698,
		b = -0.155,
		c = -2679.913,
		l = 785.46,
		m = 0.122,
		q = 1,
		f = -782.972,
		a1 = 0.734,
		b1 = 1,
		y = 1,
		p = -0.844;


	K11 = pow(2.178, a1 / (BAT_Tempp + 273));
	K12 = pow(BAT_Curr, y);
	K21 = pow(2.178, p / (BAT_Tempp + 273));
	K22 = pow(BAT_Curr, q);


	A = (a*K11 + b / K12 + c) / 8;
	B = (l*K21 + m*K22 + f) / 2;
	C = pow(batcyc, B);

	Cr = A*C;



	return Cr;


}


float Battery_Sop(float BAT_Vtge)
{
	//float SOH=1;  //���˥����
	// float I=1;  //��ص���
	// float r=0.1;
	// float Es=4.1; // ��������ѹ
	//float u0=3.51 ; // ��ֹ��ѹ
	float P; //�����������
	//float Qcc=3.2; //�����������
	float dt = 1; //��ز���ʱ��
	float voltage;
	float p;
	float Cdt = 1;  // �����ŵ�ʱ��



	//**************�ڶ����ּ��㹫ʽ����********************

	//**********	�̶���������µķ�ֵ��������
	float soc = 0.2; //���ʣ�����
	float Es = 3.71; // ��������ѹ
	float Ac = 0.5;
	float Bc = 0.5;
	float temp = 300;
	float R = 0.1;
	float Ea = 3.8;
	float SOCmin = 0.1;
	float I_soc;
	float Xc;
	float P_soc;

	//	Xc=Bc*exp(-Ea/(R*(temp+300))+Ac;
	Xc = Bc * exp(-Ea / (R * (temp + 273))) + Ac;

	I_soc = (soc - SOCmin) / Xc;

	P_soc = I_soc* Es;

	P = P_soc;

	//**********	�̶���ѹ�����µķ�ֵ��������

	return  P;

}
*/
