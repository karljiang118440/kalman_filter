/*
* bms.c
*
*  Created on: 2018年1月25日
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

	float qcc;//电池在电流为I，经过时间t所放电量
	float soc;
	float k, pc, soc0;



//********仿真使用，输入激励信号*****************


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



	//******不同放电倍率对容量补偿***************
	float Qcc_discharge_lut[2][10] = {
		{  0.95, 0.948, 0.92,   0.83,   0.75,   0.72,   0.3,   0.25,  0.2, 0.18},
		{ 0.1,   0.11,     0.3,    0.4,    0.5,    0.6,    1,    2,    3,  3.1 },
		//{ 1, 0.95, 0.9, 0.88, 0.4, 0.35, 0.3, 0.25 },
		//{ 0.1,0.2,  0.31,   0.5,    0.8,     1,      2,       3},
		//{ 0.9, 0.8, 0.6, 0.5, 0.4, 0.3, 0.3, 0.25}
	};

	//printf("BAT_Curr=%f\n", BAT_Curr);

	/*****不同充电倍率对容量补偿***************
	float Qcc_charge_lut[2][10] = {
	{ 0.1,0.5,1,1.5,2,2.5,5,8,10},
	{  1  ,0.9,0.8,0.7,0.6,0.5,0.4,0.3,0.2,0.1 }
	};*/



	//******不同温度条件下对容量补偿***************
	float Qcc_temp_lut[2][5] = {
		{ 0.6, 0.85, 0.9, 1, 1.2 },
		{ -20, -10, 0, 25, 60 }
	};

	/******电池自放电条件下对容量补偿***************
	float Qcc_temp_lut[2][5] = {
		{ 0.6, 0.85, 0.9, 1, 1.2 },
		{ -20, -10, 0, 25, 60 }
	};*/



	//*****电池内阻 和放电电流的关系**********
	/*float R_temp_lut[2][30] = {
		{ 100, 99, 97, 95, 92, 90, 87, 85, 80, 75, 70, 65, 60, 55, 50, 45, 42, 40, 35, 30, 25, 20, 15, 12, 10, 8, 5, 3, 1, 0 },
		{ 4.2, 4.15, 4.14, 4.12, 4.10, 4.08, 4.05, 4.03, 3.97, 3.90, 3.87, 3.84, 3.81, 3.79, 3.77, 3.76, 3.75, 3.74,
		3.73, 3.72, 3.71, 3.70, 3.69, 3.66, 3.65, 3.64, 3.63, 3.61, 3.59, 3.58 }
	};*/




///***********自定义过程参数量************
	for ( i = 0; i < 5; i++)
	{

	float t =vol_lut[1][i];//统计电池放电时间，单位秒

	float r = 0.35;//电池内阻

	float QCC = 3223;//电池电量mAH

	float V0 = 3.50;///截止电压

	float DischargeRate;

	float Cdt = 10;/////放电时间 s

	float coutbat = 1;	



	float voltage;//开路电压值，初值只能在没有负载情况下计算
	float BAT_Vtge = vol_lut[0][i]; //无法确定带负载的时候初值
	//float BAT_Vtge = 3.6; //无法确定带负载的时候初值
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

	//  容量补偿部分
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

	// 查表确定温度补偿
	for (j = 0; j <= 5; j++)
	{
		if (Qcc_temp_lut[1][j] <= temp && temp <= Qcc_temp_lut[1][j + 1])
			break;
		k = (Qcc_temp_lut[0][j + 1] - Qcc_temp_lut[0][j]) / (Qcc_temp_lut[1][j + 1] - Qcc_temp_lut[1][j]);
		Qcc_temp_k = Qcc_temp_lut[0][j + 1] + k*(temp - Qcc_temp_lut[1][j]);
		//soc = soc0 - pc*100;

	}

	// 不同放电倍率对容量补偿
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

	// 计算部分

	//先判断电流是否为0，电流为 0 不进行补偿。
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

	soc = socinital - pc * 100;//该 socinital 只能从外界输入

	soc_item[i] = soc;

	coutbat++;

	//return soc;

	//void Kalman_Filter(float SOC, float Uc)//SOC 为计算得出的值，OCV为开路电压值

	//{
		float SOC0;   //z最先估计的值

		//float dt = 1;  //积分时间，dt为滤波器采样时间 t；

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

		char C_0 = 1;  //H 矩阵的一个数

		//float soc_err = 0.1;//查表法和安时积分法得到的结果误差

		float K_0 = 0, K_1 = 0, t_0 = 0, t_1 = 0; //K是卡曼增益，t是中间变量

		float Pdot[4] = { 0, 0, 0, 0 }; //计算P矩阵的中间变量

		float PP[2][2] = { { 1, 0 }, { 0, 1 } }; //公式中P矩阵，x的协方差

		float A[2][2] = { { 1, 0 }, { 0, 1 } }; //A 矩阵系数

		float Q[2][2] = { { 0.01, 0.01 }, { 0.01, 0.01 } }; //Q 矩阵系数

		float B[2] = { 0, 0 }; //B 矩阵系数

		//SOC = SOC - dt*current / QCC;     // SOC 安时积分法计算

		// Uc = Uc*exp(-dt / (R2*C)) + BAT_Curr*R2*(1 - exp(-dt / (R2*C)); // 一阶电池模型；
		// Uc = Uc * exp (-dt / (R2 * C) + BAT_Curr * R2 *(1-exp(-dt / (R2 * C)) ;


		//  计算相关的计算结果***********************************

		//SOC_lut = 58.6;
		SOC0 = soc;
		soc_err = SOC_lut - SOC0;


		//  计算系数矩阵***********************************

		A[0][0] = 1;
		A[0][1] = 0;
		A[1][0] = 0;
		A[1][1] = exp(-dt / (R2*C));

		B[0] = -dt / QCC;
		//	B[1] = R2*(1 - exp(-dt / (R2*C));
		B[1] = R2 * (1 - exp(-dt / (R2 * C)));

		//  计算Q系数矩阵**********************************

		Q[0][0] = 0.01;
		Q[0][1] = 0.01;
		Q[1][0] = 0.01;
		Q[1][1] = 0.01;

		// Pdot为P的中间计算变量****************************

		Pdot[0] = PP[0][0];
		Pdot[1] = 0;
		Pdot[2] = 0;
		Pdot[3] = PP[1][1] * exp(-dt / (R2*C))*exp(-dt / (R2*C));


		// P[][]计算过程****************************

		PP[0][0] += Pdot[0] * dt + Q[0][0];
		PP[0][1] += Pdot[1] * dt + Q[0][1];
		PP[1][0] += Pdot[2] * dt + Q[1][0];
		PP[1][1] += Pdot[3] * dt + Q[1][1];

		// Kg校正过程****************************

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
	//float SOH=1;  //电池衰减率
	// float I=1;  //电池电流
	// float r=0.1;
	// float Es=4.1; // 电池输出电压
	//float u0=3.51 ; // 截止电压
	float P; //电池输出最大功率
	//float Qcc=3.2; //电池输出最大功率
	float dt = 1; //电池采样时间
	float voltage;
	float p;
	float Cdt = 1;  // 持续放电时间



	//**************第二部分计算公式参数********************

	//**********	固定电池容量下的峰值电流计算
	float soc = 0.2; //电池剩余电量
	float Es = 3.71; // 电池输出电压
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

	//**********	固定电压条件下的峰值电流计算

	return  P;

}
*/
