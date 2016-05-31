#define __ACTION_C_
#include "includes.h"
int flag_5_2=0;
int flag_c_2_2=0;
int flag_c_4_1=0;
int flag_c_4_2=0;
byte Road_num=0;
int g_f_red=0;//信号灯标志位
int car_direction=1;//车身绝对方向：1234北东南西
int old_car_direction=1;//车身上一次绝对方向：1234北东南西
BYTE waiting_for_response=0;

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
void RFID_control_car_1_action(WORD site)//欧阳修改
{
#if 0
	if (RFID_CARD_ID_2_4 == site&&RFID_site_data.old_site==RFID_CARD_ID_1_3)
	{
		Road_num=WIFI_ADDRESS_Road_Node_1;
		send_net_cmd(Road_num,WIFI_CMD_ASK_ROAD);//请求节点发送路况信息
	}
#endif
	if((site>>12)==0x1||(site>>12)==0x3)//在红绿灯路口
	{
		main_wifi_sender (site);
	}
}

/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* RFID                                                                  */
/* 车2                                                                   */
/*-----------------------------------------------------------------------*/
void RFID_control_car_2_action(DWORD site)
{
//	if (RFID_CARD_ID_1_3 == site)
//	{
//		set_steer_helm(0);
//	}
#if 0
	if (RFID_CARD_ID_2_4 == site&&RFID_site_data.old_site==RFID_CARD_ID_1_3)
	{
		Road_num=WIFI_ADDRESS_Road_Node_1;
		send_net_cmd(Road_num,WIFI_CMD_ASK_ROAD);//请求节点发送路况信息
	}
#endif
	//rfid_ask_road(0x01, 0xFF, 0xCD, site);//CD=CARD
	//road_stop();
}
/*-----------------------------------------------------------------------*/
/* 整车动作控制                                                          */
/* RFID                                                                  */
/* 车3                                                                   */
/*-----------------------------------------------------------------------*/
void RFID_control_car_3_action(DWORD site)
{
	return;
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
			RFID_site_data.is_new_site = 0;
			RFID_control_car_1_action(RFID_site_data.roadnum);
		}
		if(LightCWifi==1)            // 有一个时间间隔为了 保证在没有收到的时候不会发疯一样发
		{
			LightCWifi=0;
			wifi_sender_checker();//每次检查一次是否收到回复  注意：子函数在被设计为发送完一定时间内不会工作，防止对方还没回答这里不停发
			wifi_receive_checker();    // 收到数据回复我收到了 by xiong
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
void device_Num_change(void)
{
	if(g_device_NO==1)
		g_device_NO_Hex=0x01;
	if(g_device_NO==2)
		g_device_NO_Hex=0x02;
}

