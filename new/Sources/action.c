#define __ACTION_C_
#include "includes.h"
int flag_5_2=0;
int flag_c_2_2=0;
int flag_c_4_1=0;
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
int bz=-1;
int time=1;;
extern int jishu;

/*------------------------------*/
/* 车灯控制程序    掉头                                         */  
/*------------------------------*/
void Car_UTurn(void)
{
	g_f_enable_mag_steer_control=0;
	set_speed_target(0);
	LeftL=1;
	set_steer_helm(data_steer_helm.left_limit);
	delay_ms(800);
	set_speed_target(10);
	LeftL=0;
	delay_ms(800);
	LeftL=1;
	delay_ms(800);
	LeftL=0;
	set_steer_helm(0);
	set_car_direction(UTURN);
	EMIOS_0.CH[3].CCR.B.FEN=0;
}

/*-----------------------------------------------------------------------*/
/* 设置车身方向        act为左右转、掉头                       */
/*-----------------------------------------------------------------------*/
void set_car_direction(SBYTE act)
{
	old_car_direction=car_direction;
	if(act==TURNRIGHT)
	{
		if(car_direction!=WEST)
			car_direction+=1;
		else
			car_direction=1;
	}
	else if(act==TURNLEFT)
	{
		if(car_direction!=NORTH)
			car_direction-=1;
		else
			car_direction=4;
	}
	else if(act==UTURN)
	{
		if(car_direction==NORTH||car_direction==EAST)
			car_direction+=2;
		else
			car_direction-=2;
	}
}
/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* RFID                                                                  */
/* 车1                                                                   */
/*-----------------------------------------------------------------------*/
void RFID_control_car_1_action(WORD site)
{
	if((site>>12)==0x1)//在红绿灯路口||(site>>12)==0x3
	{
		sending_service_package(0x33,0x00CD,site);
		if(Light_Status==0)
		{
			Car_Stop=1;
			LCD_Fill(0x00);
			set_speed_pwm(0);
		}
	}
	if((site>>8)==0x21 && (site>>0)!=0x2103)//在左打死路口         并起道路动作切换功能，日后需更改               jqy
	{
		LCD_Fill(0x00);
		if(bz==-1)
			bz=1;
		else if(bz==1)
			bz=-2;
		else if(bz==-2)
			bz=2;
		else if(bz==2)
			bz=-1;
	//	bz=-bz;
		jishu=0;////////标志位，防止在其他种类道路上行驶时计数累加            jqy
		set_speed_pwm(320);
	//	delay_ms(100);
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
	//	delay_ms(950);
		delay_ms(850);
	//	set_steer_helm_basement(data_steer_helm_basement.center);
	//	delay_ms(300);
	//	fieldover=1;
	}
	if((site>>8)==0x22)//在右打死路口                          仅起测试作用，可更改        jqy
	{
	//	LCD_Fill(0x00);
	//	set_speed_pwm(300);
	//	delay_ms(100);
	//	set_steer_helm_basement(data_steer_helm_basement.right_limit);
	//	delay_ms(700);
		bz=0;
		delay_ms(300);
		fieldover=1;
	}
#if 0   //仅停车
	if((site>>8)==0x23 && bz==-1)//停车     1号车停车参数       jqy
	{
		set_steer_helm_basement((data_steer_helm_basement.right_limit-data_steer_helm_basement.center)*0.1+data_steer_helm_basement.center);
		delay_ms(150);
		set_steer_helm_basement(data_steer_helm_basement.center);
		delay_ms(450);
		set_speed_pwm(0);
	//	Car_Stop=1;
		delay_ms(8000);
		bz=-2;
	//	LCD_Fill(0x00);
	//	fieldover=1;
	//	bz=-1;
	}
#endif
#if 1    //停车和开车配合版本，适用于1号车
	if((site>>8)==0x23 && time==1)//停车     1号车停车参数    
	{
		set_steer_helm_basement(data_steer_helm_basement.center-(data_steer_helm_basement.center-data_steer_helm_basement.left_limit)*0.2);
		delay_ms(100);
		set_steer_helm_basement((data_steer_helm_basement.right_limit-data_steer_helm_basement.center)*0.2+data_steer_helm_basement.center);
		delay_ms(100);
		set_steer_helm_basement(data_steer_helm_basement.center);
		delay_ms(300);
		set_speed_pwm(0);
	//	Car_Stop=1;
		delay_ms(4000);
		fieldover=0;
		set_steer_helm_basement((data_steer_helm_basement.right_limit-data_steer_helm_basement.center)*0.1+data_steer_helm_basement.center);
		set_speed_pwm(-320); 
		time++;
	}
	else if((site>>8)==0x23 && time==2)//停车     1号车停车参数
	{
		set_speed_pwm(1000);
		delay_ms(80);
		set_speed_pwm(0);
		delay_ms(1500);
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		set_speed_pwm(320);
		delay_ms(2300);
		set_steer_helm_basement(data_steer_helm_basement.center);
		delay_ms(500);
		fieldover=1;
		time++;
	}
	else if((site>>8)==0x23 && time==3)//停车     1号车停车参数
	{
		set_steer_helm_basement(data_steer_helm_basement.center-(data_steer_helm_basement.center-data_steer_helm_basement.left_limit)*0.96);
		set_speed_pwm(300);
		delay_ms(1700);
		set_speed_pwm(0);
		set_steer_helm_basement(data_steer_helm_basement.right_limit);
		delay_ms(1700);
		set_speed_pwm(-320);
		delay_ms(1700);
		set_speed_pwm(0);
		set_steer_helm_basement(data_steer_helm_basement.center);
		Car_Stop=1;
	}
#endif
#if 0
	else if((site>>8)==0x23 && bz==-2)//停车    2号车停车参数
	{
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		delay_ms(600);
		set_steer_helm_basement(data_steer_helm_basement.right_limit);
		delay_ms(650);
		set_steer_helm_basement(data_steer_helm_basement.center);
		set_speed_pwm(0);
		delay_ms(8000);
	//	Car_Stop=1;
	//	delay_ms(5000);
	//	LCD_Fill(0x00);
	//	fieldover=1;
		bz=-3;
	}
#endif
#if 0
	if((site>>8)==0x23)
//	else if((site>>8)==0x23 && bz==-3)//停车    3号车停车参数
	{
		set_steer_helm_basement(data_steer_helm_basement.center-(data_steer_helm_basement.center-data_steer_helm_basement.left_limit)*0.96);
		set_speed_pwm(300);
		delay_ms(2000);
		set_speed_pwm(0);
		set_steer_helm_basement(data_steer_helm_basement.right_limit);
		delay_ms(1700);
		set_speed_pwm(-300);
		delay_ms(1700);
		set_speed_pwm(0);
		set_steer_helm_basement(data_steer_helm_basement.center);
		Car_Stop=1;
	//	delay_ms(5000);
	//	LCD_Fill(0x00);
	//	fieldover=1;
	//	bz=-1;
	}
#endif
#if 0
	if((site>>8)==0x23)//停车    初始版本
	{
		set_speed_pwm(0);
		delay_ms(2000);
		set_steer_helm_basement(data_steer_helm_basement.right_limit);
		set_speed_pwm(-350);
		delay_ms(1500);
		set_speed_pwm(0);
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		delay_ms(1500);
		set_speed_pwm(350);
		delay_ms(1200);
		set_speed_pwm(0);
		set_steer_helm_basement(data_steer_helm_basement.center);
		delay_ms(800);
		set_speed_pwm(350);
		delay_ms(1000);
		set_speed_pwm(0);
		
		Car_Stop=1;
		//	LCD_Fill(0x00);
		fieldover=1;
	}
#endif
	if((site>>8)==0x31)
	{
		velocity=900;
		set_speed_pwm(velocity);
		fieldover=1;
	}
	if((site>>8)==0x32)
	{
		delay_ms(200);
		velocity=-500;
		set_speed_pwm(velocity);
	//	fieldover=1;
	}
	if((site>>0)==0x3002)
	{
		velocity=-900;
		set_speed_pwm(velocity);
		delay_ms(3000);
		set_speed_pwm(0);
		delay_ms(50);
		set_speed_pwm(-150);
		set_steer_helm_basement(data_steer_helm_basement.center-(data_steer_helm_basement.center-data_steer_helm_basement.left_limit)*0.1);
		delay_ms(300);
		set_steer_helm_basement(data_steer_helm_basement.right_limit);
		set_speed_pwm(250);
		delay_ms(1200);
		velocity=300;
		set_speed_pwm(velocity);
	//	fieldover=1;
	}
	if((site>>0)==0x3001)
	{
		delay_ms(300);
		velocity=300;
		set_speed_pwm(velocity);
		fieldover=1;
	}

}

/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* RFID                                                                  */
/* 车2                                                                   */
/*-----------------------------------------------------------------------*/
void RFID_control_car_2_action(DWORD site)
{
	if((site>>8)==0x11||(site>>8)==0x12)//在红绿灯路口
	{
		sending_service_package(0x33,0x00CD,site);
		if(Light_Status==0)
		{
			Car_Stop=1;
			LCD_Fill(0x00);
			set_speed_pwm(-100);
			delay_ms(50);
			set_speed_pwm(0);
		}
	}
	if((site>>8)==0x21)//在左打死路口         并起道路动作切换功能，日后需更改               jqy
	{
		LCD_Fill(0x00);
		if(bz==-1)
			bz=1;
		else if(bz==1)
			bz=-2;
		else if(bz==-2)
			bz=2;
		else if(bz==2)
			bz=-1;
	//	bz=-bz;
		jishu=0;////////标志位，防止在其他种类道路上行驶时计数累加            jqy
		set_speed_pwm(300);
	//	delay_ms(100);
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
	//	set_steer_helm_basement(data_steer_helm_basement.center);
	//	delay_ms(300);
	//	fieldover=1;
	}
	//rfid_ask_road(0x01, 0xFF, 0xCD, site);//CD=CARD
	//road_stop();
	if((site>>12)==0x1||(site>>12)==0x3)//在红绿灯路口
	{
		sending_service_package(0x33,0x00CD,site);
		if(Light_Status==0)
		{
			Car_Stop=1;
			LCD_Fill(0x00);
			set_speed_pwm(0);
		}
	}
	if((site>>0)==0x2103)//在左打死路口
	{
		delay_ms(200);
		set_speed_pwm(200);
		set_speed_pwm(270);
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		delay_ms(1200);
	}
	else if((site>>8)==0x21)//在左打死路口
	{
		LCD_Fill(0x00);
		bz=5;
		set_speed_pwm(200);
		set_speed_pwm(270);
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		delay_ms(1500);
	}
	if((site>>8)==0x22)//在右打死路口
	{
		LCD_Fill(0x00);
		delay_ms(500);
		set_speed_pwm(280);
		set_steer_helm_basement(data_steer_helm_basement.right_limit);
		delay_ms(1000);
		fieldover=1;
	}
	if((site>>8)==0x23)//停车
	{
		set_speed_pwm(300);
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		delay_ms(500);
		set_steer_helm_basement(data_steer_helm_basement.right_limit);
		delay_ms(550);
		set_steer_helm_basement(data_steer_helm_basement.center);
		set_speed_pwm(0);
		delay_ms(5000);
	}
	if((site>>8)==0x24)//三叉路口直行
	{
		set_steer_helm_basement(data_steer_helm_basement.center);
		delay_ms(1500);
	}
	if((site>>12)==0x04)//在接客区
	{
		sending_service_package(0x44,0x0020,site);
		Car_Stop=1;
		LCD_Fill(0x00);
		Car_Psg=1;
		set_speed_pwm(0);
	}
}
/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* RFID                                                                  */
/* 车3                                                                   */
/*-----------------------------------------------------------------------*/
void RFID_control_car_3_action(DWORD site)
{
	if((site>>8)==0x11||(site>>8)==0x12)//在红绿灯路口
	{
		sending_service_package(0x33,0x00CD,site);
		if(Light_Status==0)
		{
			Car_Stop=1;
			LCD_Fill(0x00);
			set_speed_pwm(-500);
			delay_ms(50);
			set_speed_pwm(0);
		}
	}
	if((site>>8)==0x21 && (site>>0)!=0x2103)//在左打死路口
	{
		LCD_Fill(0x00);
		bz=5;
		set_speed_pwm(450);
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		delay_ms(800);
	}
	if((site>>8)==0x22)//在右打死路口
	{
		LCD_Fill(0x00);
		set_speed_pwm(500);
		delay_ms(100);
		set_steer_helm_basement(data_steer_helm_basement.right_limit);
		delay_ms(1500);
		fieldover=1;
	}
	if((site>>8)==0x23)//停车
	{
	//	set_speed_pwm(0);
	//	Car_Stop=1;
	//	LCD_Fill(0x00);
		jishu=1;
	}
	if((site>>8)==0x40)//在接客区
	{
		sending_service_package(0x44,0x0020,site);
		Car_Stop=1;
		LCD_Fill(0x00);
		Car_Psg=1;
		set_speed_pwm(0);
	}
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
/* WiFi                                                                  */
/* 车1                                                                   */
/*-----------------------------------------------------------------------*/
void WiFi_control_car_1_action(WORD cmd)
{
	if (WIFI_CMD_NET_0_2 == cmd)//有障碍
	{
		Car_UTurn();
		set_speed_target(10);
	}
	if (WIFI_CMD_ROADBLOCK_unKNOW == cmd||WIFI_CMD_ROADBLOCK_NO == cmd)//无障碍
	{

		send_net_cmd(Road_num,WIFI_CMD_ASK_LIGHT);//请求节点发送路灯信息
	}
	if (WIFI_CMD_LIGHT_RED == cmd)//红灯不可通行
	{
		g_f_red=1;

	}
	if (WIFI_CMD_LIGHT_YELLOW == cmd||WIFI_CMD_LIGHT_GREEN==cmd)//绿灯可通行包括停车状态与通行状态
	{
		g_f_red=0;
		set_steer_helm(0);
		set_speed_target(10);
//		EMIOS_0.CH[3].CSR.B.FLAG = 1;//清场中断标志位
//		EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断
	}
	return;
}

/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* WiFi                                                                  */
/* 车2                                                                   */
/*-----------------------------------------------------------------------*/
void WiFi_control_car_2_action(WORD cmd)
{
	if (WIFI_CMD_NET_0_2 == cmd)//有障碍
	{
		Car_UTurn();
		set_speed_target(10);
	}
	if (WIFI_CMD_NET_3_3 == cmd)//无障碍
	{
		D1=~D1;
		D2=~D2;
		send_net_cmd(Road_num,WIFI_CMD_ASK_LIGHT);//请求节点发送路灯信息
	}
	if (WIFI_CMD_NET_0_1 == cmd)//红灯不可通行
	{
		g_f_red=1;

	}
	if (WIFI_CMD_NET_2_1 == cmd)//绿灯可通行包括停车状态与通行状态
	{
		g_f_red=0;
		set_steer_helm(0);
		set_speed_target(10);
//		EMIOS_0.CH[3].CSR.B.FLAG = 1;//清场中断标志位
//		EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断
	}

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
		if (RFID_site_data.is_new_site && RFID_site_data.old_site!=RFID_site_data.site)
		{
			RFID_site_data.old_site=0x00000000;
			RFID_site_data.site = 0x00000000;
			RFID_site_data.time = 0x00000000;
			RFID_site_data.is_new_site = 0;
			RFID_control_car_1_action(RFID_site_data.roadnum);
		}
		if(Car_Stop)
		{
			if(Light_Status==1)
			{
				Car_Stop=0;
			}
		}
	}
	if(WIFI_ADDRESS_CAR_2 == g_device_NO)
	{

		if (RFID_site_data.is_new_site && RFID_site_data.old_site!=RFID_site_data.site)
		{
			RFID_site_data.old_site=0x00000000;
			RFID_site_data.site = 0x00000000;
			RFID_site_data.time = 0x00000000;
			RFID_site_data.is_new_site = 0;
			RFID_control_car_2_action(RFID_site_data.roadnum);
		}
		if(Car_Stop)
		{
			if(Light_Status==1)
			{
				set_speed_pwm(350);
				delay_ms(1000);
				set_steer_helm_basement(data_steer_helm_basement.left_limit);
				delay_ms(1000);
				Car_Stop=0;
			}
		}
	}
	if(WIFI_ADDRESS_CAR_3 == g_device_NO)
	{
		if (RFID_site_data.is_new_site && RFID_site_data.old_site!=RFID_site_data.site)
		{
			RFID_site_data.is_new_site = 0;
			RFID_control_car_3_action(RFID_site_data.roadnum);
			RFID_site_data.old_site=0x0;
			RFID_site_data.site=0x0;
		}
		if(Car_Stop==1 && Car_Psg == 0 )
		{
			if(Light_Status==1)
			{
				Car_Stop=0;
				set_speed_pwm(500); 
			}
		}
		if(Car_Psg)
		{
			BMW_Taxi();
		}
	}
	if(WIFI_ADDRESS_CAR_2 == g_device_NO)
	{
		if (RFID_site_data.is_new_site && RFID_site_data.old_site!=RFID_site_data.site)
		{
			RFID_site_data.old_site=0x00000000;
			RFID_site_data.site = 0x00000000;
			RFID_site_data.time = 0x00000000;
			RFID_site_data.is_new_site = 0;
			RFID_control_car_2_action(RFID_site_data.roadnum);
		}
		if(Car_Stop)
		{
			if(Light_Status==1)
			{
				Car_Stop=0;
			}
		}
	}
#if 0
	if (WIFI_ADDRESS_CAR_2 == g_device_NO)
	{
		if (RFID_site_data.is_new_site)
		{
			RFID_site_data.is_new_site = 0;
			
			RFID_control_car_2_action(RFID_site_data.site);
		}
		if (g_net_control_data.is_new_cmd)
		{
			
			g_net_control_data.is_new_cmd = 0;
			
			WiFi_control_car_2_action(g_net_control_data.cmd);
		}
	}

	else if (WIFI_ADDRESS_CAR_4 == g_device_NO)
	{
		if (RFID_site_data.is_new_site)
		{
			RFID_site_data.is_new_site = 0;
			
			RFID_control_car_4_action(RFID_site_data.site);
		}
		if (g_net_control_data.is_new_cmd)
		{
			g_net_control_data.is_new_cmd = 0;
			
			WiFi_control_car_4_action(g_net_control_data.cmd);
		}
	}
	else if (WIFI_ADDRESS_CAR_3 == g_device_NO)
	{
		if (RFID_site_data.is_new_site)
		{
			RFID_site_data.is_new_site = 0;
			
			RFID_control_car_3_action(RFID_site_data.site);
		}
		if (g_net_control_data.is_new_cmd)
		{
			g_net_control_data.is_new_cmd = 0;
			
			WiFi_control_car_3_action(g_net_control_data.cmd);
		}
	}	
	else if (WIFI_ADDRESS_CAR_1 == g_device_NO)
	{
		if (RFID_site_data.is_new_site)
		{
			RFID_site_data.is_new_site = 0;
			
			RFID_control_car_1_action(RFID_site_data.site);
		}
		if (g_net_control_data.is_new_cmd)
		{
			g_net_control_data.is_new_cmd = 0;
			
			WiFi_control_car_1_action(g_net_control_data.cmd);
		}
	}
#endif
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
}
void car_default()//每次开车将宝马所有标志位置为默认状态，防止起冲突
{
	if(WIFI_ADDRESS_CAR_1 == g_device_NO)
	{
		have_responsed=1;
		waiting_for_response=0;
	}
	if(WIFI_ADDRESS_CAR_3 == g_device_NO)
	{
		if(Door_Close_Run)
			Door_Close_Run=0;
		if(Door_Delay)
			Door_Delay=0;
		set_door_pwm(0);
		if(Car_Psg)
			Car_Psg=0;
		if(Door_Open)
			Door_Open=0;
		if(Door_Close)
			Door_Close=0;
		if(Door_Status)
			Door_Status=0;
		if(Door_Stop)
			Door_Stop=0;
		if(DoorC!=0)
			DoorC=0;
	}
}
void BMW_Taxi()//宝马开关门开车控制
{
	if(Door_Open)
	{
		Door_Status=1;	
		set_door_pwm(600);
		Door_Open=0;
	}
	if(Door_Close)
	{
		Door_Status=1;
		set_door_pwm(-600);
		Door_Close=0;
	}
	if(Door_Stop)
	{
		set_door_pwm(0);
		Door_Stop=0;
		if(Door_Delay)
		{
			Car_Stop=0;
			Door_Delay=0;
		}
	}
	if(Door_Close_Run)
	{
		Door_Status=1;
		set_door_pwm(-600);
		Door_Close_Run=0;
		Door_Delay=1;
	}
}

void zhangai_run()
{
#if 1
		if(zhangai==0 && bz==1)          //超车处理参数，适用于1号车          jqy
		{
	//		delay_ms(200);
			set_steer_helm_basement(data_steer_helm_basement.left_limit);   
			delay_ms(420);
	//		delay_ms(650);
			set_steer_helm_basement(data_steer_helm_basement.right_limit);
			delay_ms(450);
	//		delay_ms(550);
			set_steer_helm_basement(data_steer_helm_basement.center);
			delay_ms(300);
	//		delay_ms(560);
			zhangai=1;
		}
#endif
#if 1
	//	if(zhangai==0 && bz==-1)
		if(zhangai==0 && bz==0)          //避障停车处理参数，适用于1号车         jqy
		{
			set_steer_helm_basement(data_steer_helm_basement.center);
			delay_ms(800);
			set_speed_pwm(0);
			jishu=0;
			delay_ms(1000);
		//	zhangai=1;
		}
		while(zhangai==0)                //前车未离开，本车不能前进            jqy
		{
			set_speed_pwm(0);
		    FindBlackLine(); 
	//		supersonic();
		    if(RoadType!=12 && RoadType!=13)
		    	if(RoadType!=66)
		    		jishu++;
		    LCD_Write_Num(105,4,jishu,2);
		    EMIOS_0.CH[3].CSR.B.FLAG = 1;
		    EMIOS_0.CH[3].CCR.B.FEN=1;
		    if(jishu>=50)
		    {
		    	zhangai=1;
		    	jishu=0;
		    	bz=-2;               ///////////////////////
		    	delay_ms(2000);
		    }
		}
#endif
}
