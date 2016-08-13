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
extern int delay_count;
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
	if((site>>8)!=0x25 && (site>>8)!=0x23)
		sending_service_package(0x33,0x00CD,site);
	if((site>>8)==0x21 && (site>>0)!=0x2103)//在左打死路口         并起道路动作切换功能，日后需更改               jqy
	{
		bz=-1;
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		while(delay_count<2700)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
	}
	if((site>>0)==0x0A01 && bz==4)
	{
		RunL=1;		
		g_f_enable_speed_control=0;
		set_speed_pwm(0);
//		velocity=-9;
//		set_speed_target(-9);
//		fieldover=1;
//		bz=6;
    	delay_ms(3000);
    	set_steer_helm_basement(data_steer_helm_basement.center);
    	g_f_enable_speed_control=1;
		delay_count=0;	
		set_speed_target(-9);
		while(delay_count>-3900)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}	
		g_f_enable_speed_control=0;
		set_speed_pwm(0);
		bz=6;
		delay_ms(1000);
		Hang_Up();
		delay_ms(1000);
		bz=-6;
		fieldover=1;
		g_f_enable_speed_control=1;
		velocity=16;
	}
//	if((site>>0)==0x0A02 && bz==6)
//	{
//		delay_ms(1000);
//		Hang_Up();
//		delay_ms(1000);
//		bz=-6;
//		fieldover=1;
//		g_f_enable_speed_control=1;
//		velocity=16;
//	}
	if((site>>0)==0x8004)
	{
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		while(delay_count<2700)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.center);
		while(delay_count<550)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		set_speed_target(0);
		EMIOS_0.CH[10].CBDR.R = 2000;
		RunL=0;
		delay_ms(2000);
		delay_count=0;
		while(delay_count<500)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		g_f_enable_speed_control=0;
		set_speed_pwm(0);
		Car_Waitfororder=1;
	}
	if((site>>0)==0x8009)
	{
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		while(delay_count<1150)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.right_limit);
		while(delay_count<1000)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.center);
		while(delay_count<700)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		delay_count=0;
		set_steer_helm_basement(data_steer_helm_basement.right_limit);
		while(delay_count<400)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		bz=4;
	}
}

/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* WiFi                                                                  */
/* 车1                                                                   */
/*-----------------------------------------------------------------------*/
void WiFi_control_car_1_action(WORD cmd)
{
}

/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* WiFi                                                                  */
/* 车2                                                                   */
/*-----------------------------------------------------------------------*/
void WiFi_control_car_2_action(WORD cmd)
{
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
	if(WIFI_ADDRESS_CAR_4 == g_device_NO)
	{
		if (RFID_site_data.is_new_site == 1)
		{
			RFID_site_data.roadnum=RFID_Num_Exp(RFID_site_data.site);	
			RFID_site_data.is_new_site = 0;
			RFID_control_car_4_action(RFID_site_data.roadnum);
			RFID_site_data.is_new_site = 0;
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
	//		delay_ms(640);
			delay_ms(690);
			set_steer_helm_basement(data_steer_helm_basement.right_limit);
	//		delay_ms(670);
			delay_ms(720);
			set_steer_helm_basement(data_steer_helm_basement.center);
			delay_ms(350);
			zhangai=1;
			fieldover=1;
			bz=4;
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
	//	    LCD_Write_Num(105,4,jishu,2);
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
void Start_one()
{
	if(g_device_NO==4 && Car_Waitfororder==1)    //电压8.7V
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
		velocity=16;
		set_speed_target(velocity);
		set_steer_helm_basement(data_steer_helm_basement.center);
		delay_count=0;
		while(delay_count<1500)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
		set_steer_helm_basement(data_steer_helm_basement.left_limit);
		delay_count=0;
		while(delay_count<4500)
		{
			LCD_Write_Num(105,3,delay_count,4);
		}
	}
	
}
