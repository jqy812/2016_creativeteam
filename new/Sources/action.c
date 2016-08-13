#define __ACTION_C_
#include "includes.h"
int flag_5_2=0;
int flag_c_2_2=0;
int flag_c_4_1=0;
int Road_No=1;
int flag_c_4_2=0;
byte Road_num=0;
int Car_Stop=0;
int Door_Status=0;
int Door_Stop=0;
int Car_Psg=0;
int Door_Open=0;
int Light_2_Ctrl=0;
int Door_Delay=0;
int Door_Close=0;
int Door_Close_Run=0;
int g_f_red=0;//信号灯标志位
int car_direction=1;//车身绝对方向：1234北东南西
int old_car_direction=1;//车身上一次绝对方向：1234北东南西

//********************2016赛季参数******************************************
extern int velocity;
extern int zhangai;
int bz=-1;//关键标志位  有问题问jqy
int time=1;
extern int jishu;
int right=0;//改变2号车PID参数标志位
int place[4]={0,0,0,0};//不同接客区标志   0：未启用   1：起点   2：终点   3：已使用
int current_place=0;
int used=0;//标志已经过起点
int Leftturn=0;//左转位
int Rightturn=0;//右转位
int Had_turn=0;//转过后不再转
int up=0;
extern int Traffic_Jam;
extern int End;
int Game_over=0;
int Emergency=0;
int Polizei=0;
extern int delay_count;
int Hold_a=0;
int Hold_b=0;
/*------------------------------*/
/* 车灯控制程序    掉头                                         */  
/*------------------------------*/
void Car_UTurn(void)
{
}

/*-----------------------------------------------------------------------*/
/* 设置车身方向        act为左右转、掉头                       */
/*-----------------------------------------------------------------------*/
void set_car_direction(SBYTE act)
{
}
/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* RFID                                                                  */
/* 车1                                                                   */
/*-----------------------------------------------------------------------*/
void RFID_control_car_1_action(WORD site)
{
}

/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* RFID                                                                  */
/* 车2                                                                   */
/*-----------------------------------------------------------------------*/
void RFID_control_car_2_action(DWORD site)
{

}
/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* RFID                                                                  */
/* 车3                                                                   */
/*-----------------------------------------------------------------------*/
void RFID_control_car_3_action(DWORD site)
{

}
/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* RFID                                                                  */
/* 车4                                                                   */
/*-----------------------------------------------------------------------*/
void RFID_control_car_4_action(DWORD site)
{
	return;
}
/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* RFID                                                                  */
/* 车7                                                                   */
/*-----------------------------------------------------------------------*/
void RFID_control_car_7_action(WORD site)
{
	if((site>>8)!=0x25 && (site>>8)!=0x23)
		sending_service_package(0x33,0x00CD,site);
//	if((site>>0)==0x1105 && Polizei==1)//在红绿灯路口1
//	{
//		Light_Mode=0;
//		set_steer_helm_basement(data_steer_helm_basement.center+70);
//		delay_count=0;
//		while(delay_count<7000)
//		{
//			LCD_Write_Num(105,3,delay_count,4);
//		}
//		set_steer_helm_basement(data_steer_helm_basement.left_limit);
//		delay_count=0;
//		while(delay_count<6000)
//		{
//			LCD_Write_Num(105,3,delay_count,4);
//		}
//		Polizei=0;
//		velocity=100;
//	}
	if((site>>0)==0x8014 && Polizei==1)//在红绿灯路口1
	{
		Light_Mode=0;
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		while(delay_count<7000)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		Polizei=0;
		velocity=90;
	}
	if((site>>0)==0x1102)
	{
		bz=-2;
	}
	if((site>>8)==0x12)//在红绿灯路口2
	{
		delay_count=0;
		while(delay_count<14000)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		g_f_enable_speed_control=0;
		set_speed_pwm(0); 
		sending_service_package(0x33,0x0088,0x00);
		delay_ms(1000);
		sending_service_package(0x33,0x0088,0x00);
		delay_ms(1000);
		sending_service_package(0x33,0x0088,0x00);
		velocity=60;
		while(bz!=711)
		{
			if (REMOTE_FRAME_STATE_OK == g_remote_frame_state)
			{
				g_remote_frame_state = REMOTE_FRAME_STATE_NOK;
				Wifi_Ctrl();
			}
		}
		RightL=1;
		Light_Mode=1;
		set_speed_pwm(-300);
		delay_ms(800);
		g_f_enable_speed_control=1;
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		velocity=60;
		set_speed_target(velocity);
		delay_count=0;
		while(delay_count<6000)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
	}
	if((site>>8)==0x21 && (site>>0)!=0x2103)//在左打死路口         并起道路动作切换功能，日后需更改               jqy
	{
	//	LCD_Fill(0x00);
		jishu=0;////////标志位，防止在其他种类道路上行驶时计数累加            jqy
//		delay_ms(350);
//		set_steer_helm_basement(data_steer_helm_basement.left_limit);
//		delay_ms(1000);
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.left_limit-150);
		while(delay_count<10000)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
	}
	if(site==0x8009)
	{
		Light_Mode=0;
		bz=1;
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		while(delay_count<4000)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		set_steer_helm_basement(data_steer_helm_basement.right_limit);
		delay_count=0;
		while(delay_count<4100)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		set_steer_helm_basement(data_steer_helm_basement.center);
		delay_count=0;
		while(delay_count<3000)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		bz=-1;
	}
	if(site==0x4002)
	{
		Light_Mode=1;
		bz=-1;
	}
	if(site==0x4003)
	{
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		while(delay_count<7200)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.center);
		while(delay_count<1800)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		StopL=0;
		RunL=0;
		Car_Waitfororder=1;
		g_f_enable_speed_control=0;
		set_speed_pwm(0); 
		suicide();
	}
//	if((site>>0)==0x8001)
//	{
//		bz=2;
//	}
//	if((site>>0)==0x1105)
//	{
//		bz=-1;
//	}
}

/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* RFID                                                                  */
/* 车8                                                                   */
/*-----------------------------------------------------------------------*/
void RFID_control_car_8_action(WORD site)
{
	if((site>>8)!=0x25 && (site>>8)!=0x23)
		sending_service_package(0x33,0x00CD,site);
	if((site>>8)==0x11)//在红绿灯路口1
	{
		Road_No=1;
		if(Light_Status==0)
		{
			g_f_enable_speed_control=0;
			set_speed_pwm(-300);
			delay_ms(50);
			set_speed_pwm(0);
			Car_Stop=1;
			LCD_Fill(0x00);
		}
	}
	if(((site>>0)==0x1101) && (Polizei==1))//在红绿灯路口1
	{
		g_f_enable_speed_control=0;
		delay_ms(100);
		set_speed_pwm(-300);
		delay_ms(50);
		set_speed_pwm(0);
		suicide();
	}
	if((site>>8)==0x12)//在红绿灯路口2
	{
		Road_No=2;
		if(Light_Status==1)
		{
			set_speed_target(0); 
			Car_Stop=1;
			LCD_Fill(0x00);
		}
	}
	if((site>>8)==0x21 && (site>>0)!=0x2103)//在左打死路口         并起道路动作切换功能，日后需更改               jqy
	{
		LCD_Fill(0x00);
		bz=-1;
		jishu=0;////////标志位，防止在其他种类道路上行驶时计数累加            jqy
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.left_limit-610);
		while(delay_count<2200)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
	}
	if((site>>0)==0x8005)
	{
		Hold_b=1;
	}
	if((site>>0)==0x4003)
	{
		Hold_b=0;
	}
}
/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* WiFi                                                                  */
/* 车1                                                                   */
/*-----------------------------------------------------------------------*/
void WiFi_control_car_1_action(WORD cmd)
{
	return;
}

/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* WiFi                                                                  */
/* 车2                                                                   */
/*-----------------------------------------------------------------------*/
void WiFi_control_car_2_action(WORD cmd)
{
	return;
}
/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* WiFi                                                                  */
/* 车3                                                                   */
/*-----------------------------------------------------------------------*/
void WiFi_control_car_3_action(WORD cmd)
{
	return;
}
/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* WiFi                                                                  */
/* 车4                                                                   */
/*-----------------------------------------------------------------------*/
void WiFi_control_car_4_action(WORD cmd)
{
	return;
}


/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* 负责分配                                                              */
/*-----------------------------------------------------------------------*/
void control_car_action(void)
{
	if(WIFI_ADDRESS_CAR_1 == g_device_NO)
	{
		if (RFID_site_data.is_new_site == 1)
		{
			RFID_site_data.is_new_site = 0;
			RFID_site_data.roadnum=RFID_Num_Exp(RFID_site_data.site);
			RFID_control_car_1_action(RFID_site_data.roadnum);
			RFID_site_data.is_new_site = 0;
		}
		if(Car_Stop==1 &&  RoadType!=88 && RoadType!=2)
		{
			if(Light_Status==1 && Road_No==1)
			{
				if(bz==2 || (RFID_site_data.roadnum==0x1102))
				{
					delay_ms(1500);
					LCD_Write_Num(105,6,'he',2);
				}
				delay_ms(1000);
				fieldover=1;
				Car_Stop=0;
			}
			if(Light_Status==0 && Road_No==2)
			{
				delay_ms(1000);
				fieldover=1;
				Car_Stop=0;
			}
		}
	}
	if(WIFI_ADDRESS_CAR_7 == g_device_NO)
		{
			if (RFID_site_data.is_new_site == 1 && RFID_site_data.site!=RFID_site_data.old_site)
			{
				RFID_site_data.is_new_site = 0;
				RFID_site_data.roadnum=RFID_Num_Exp(RFID_site_data.site);
				RFID_control_car_7_action(RFID_site_data.roadnum);
				RFID_site_data.is_new_site = 0;
			}
		}
	if(WIFI_ADDRESS_CAR_8 == g_device_NO)
		{
			if (RFID_site_data.is_new_site == 1 && RFID_site_data.site!=RFID_site_data.old_site)
			{
				RFID_site_data.is_new_site = 0;
				RFID_site_data.roadnum=RFID_Num_Exp(RFID_site_data.site);
				RFID_control_car_8_action(RFID_site_data.roadnum);
				RFID_site_data.is_new_site = 0;
			}
			if(Car_Stop==1 &&  RoadType!=88 && RoadType!=2)
			{
				if(Light_Status==1 && Road_No==1)
				{
//					if(bz==2 || (RFID_site_data.roadnum==0x1102))
//					{
//						LCD_Write_Num(105,6,'he',2);
//					}
					g_f_enable_speed_control=1;
					fieldover=1;
					Car_Stop=0;
				}
				if(Light_Status==0 && Road_No==2)
				{
					delay_ms(1000);
					fieldover=1;
					Car_Stop=0;
				}
			}
		}
}

void device_Num_change(void)//把设备号换成16进制（好像没啥用）
{
	if(g_device_NO==1)
		g_device_NO_Hex=0x01;
	if(g_device_NO==2)
		g_device_NO_Hex=0x02;
	if(g_device_NO==3)
		g_device_NO_Hex=0x03;
	if(g_device_NO==4)
		g_device_NO_Hex=0x04;
	if(g_device_NO==7)
		g_device_NO_Hex=0x07;
	if(g_device_NO==8)
		g_device_NO_Hex=0x08;
}


void zhangai_run()
{
	if(zhangai==0 && bz==1 && g_device_NO==7)          //停车处理参数，适用于7号车          jqy
	{
		bz=-2;
		zhangai=1;
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		while(delay_count<4000)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		set_steer_helm_basement(data_steer_helm_basement.right_limit);
		delay_count=0;
		while(delay_count<4100)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		set_steer_helm_basement(data_steer_helm_basement.center);
		delay_count=0;
		while(delay_count<2400)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
	}
}

void Start_one()
{
	if(g_device_NO==7 && Car_Waitfororder==1)    //电压8.7V
	{
		while(Car_Waitfororder==1)
		{
			if (REMOTE_FRAME_STATE_OK == g_remote_frame_state)
			{
				g_remote_frame_state = REMOTE_FRAME_STATE_NOK;
				Wifi_Ctrl();
			}
//			if(order_received==1)
//			{
//				order_received=0;
//				generate_remote_frame_2(g_device_NO_Hex, 0x33, 0x0000, 2, (const BYTE *)(&response_data));
//			}
		}
		RightL=0;
		velocity=60;
		set_speed_target(velocity);
		set_steer_helm_basement(data_steer_helm_basement.center);
		delay_count=0;
		while(delay_count<4000)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		delay_count=0;
		while(delay_count<5500)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
	}
	
}
