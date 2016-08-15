﻿#define __WIFI_C_
#include "includes.h"


int g_remote_frame_state = REMOTE_FRAME_STATE_NOK;
int g_remote_frame_cnt = 0;
int g_start_all=0;
int have_responsed;
int order_received; 
WORD response_data=0x0;
int Car_Waitfororder=0;
int Light_Status=0;//默认红灯
int Traffic_Jam=0;//默认畅通
int End=0;
BYTE remote_frame_data[REMOTE_FRAME_LENGTH];
BYTE remote_frame_data_send[REMOTE_FRAME_LENGTH];
BYTE g_device_NO = WIFI_ADDRESS_CAR_1;
BYTE g_device_NO_Hex;/* 设备号 即WiFi地址 */
BYTE des;
WORD cmd_WIFI;
WORD sending_data;
BYTE waiting_for_response=0;
int  lost_data=0;
extern int place[4];
extern int used;
extern int Emergency;
extern int Hold_a;
/*-----------------------------------------------------------------------*/
/* 执行远程命令                                                          */
/*-----------------------------------------------------------------------*/
void execute_remote_cmd(const BYTE *data)
{
	WORD cmd = 0;
	
	cmd = ((WORD)(data[0])<<8) | ((WORD)(data[1]));
	switch (cmd)
	{
		/* 舵机调参 */
		case WIFI_CMD_SET_HELM_TARGET :
		set_steer_helm_basement(*((SWORD *)(&(data[2]))));
		break;
		case WIFI_CMD_SET_HELM_KP :
		break;
		case WIFI_CMD_SET_HELM_KI :
		break;
		case WIFI_CMD_SET_HELM_KD :
		break;
		case WIFI_CMD_SET_STEER_HELM_CENTER :
		set_steer_helm_basement_center(*((WORD *)(&(data[2]))));
		break;
		case WIFI_CMD_SET_STEER_HELM_LEFT :
		set_steer_helm_basement_left_limit(*((SWORD *)(&(data[2]))));
		break;
		case WIFI_CMD_SET_STEER_HELM_RIGHT :
		set_steer_helm_basement_right_limit(*((SWORD *)(&(data[2]))));
		break;
		case WIFI_CMD_WRITE_STEER_HELM_DATA_TO_TF :
		if (!update_steer_helm_basement_to_steer_helm())
		{
//			SWORD tmp_read_rad_xyz = (SWORD)read_rad_xyz;	/* 暂存陀螺仪读取标志位 */
//			
//			read_rad_xyz = 0;	/* 暂存禁用陀螺仪读取，防止干扰TF卡读写 */
			write_steer_helm_data_to_TF();	/* 大约60ms */
//			read_rad_xyz = tmp_read_rad_xyz;	/* 恢复陀螺仪读取标志位 */
		}
		else
		{
			//舵机数据不合理
		}
		break;
		case WIFI_CMD_SEND_STEER_HELM_DATA_FROM_TF :
		generate_remote_frame_2(g_device_NO, ANDROID_ADDRESS, WIFI_CMD_SEND_STEER_HELM_DATA_FROM_TF, sizeof(data_steer_helm_basement), (const BYTE *)&data_steer_helm_basement);
		break;
		
		
		/* 电机调参 */
		case WIFI_CMD_SET_MOTOR_TARGET :
		set_speed_target(*((SWORD *)(&(data[2]))));
		break;
		case WIFI_CMD_STOP_SPEED :
		set_speed_target((SWORD)0);
		break;
		case WIFI_CMD_SET_MOTOR_KP :
		set_speed_KP(*((SWORD *)(&(data[2]))));
		break;
		case WIFI_CMD_SET_MOTOR_KI :
		set_speed_KP(*((SWORD *)(&(data[2]))));
		break;
		case WIFI_CMD_SET_MOTOR_KD :
		set_speed_KP(*((SWORD *)(&(data[2]))));
		break;
	
		case WIFI_CMD_GET_SEEED_NOW :
		g_remote_control_flags.send_seppd_now = 1;
		break;
		case WIFI_CMD_UNGET_SPEED_NOW :
		g_remote_control_flags.send_seppd_now = 0;
		break;
		
		case WIFI_CMD_NET :
		D0=~D0;//测试用
		g_net_control_data.is_new_cmd = 1;
		g_net_control_data.cmd = *((WORD *)(&(data[2])));
		break;
		/*开始全场工作*/
		case WIFI_CMD_START_ACTIVE:
		g_start_all=1;
//		g_f_red=1;//测试用
		break;
	}
}

/*-----------------------------------------------------------------------*/
/* 接受远程数据帧                                                        */
/* 第二版                                                                */
/* 修改地址位定义:源地址(1B)+目的地址(1B)                                */
/* AA BB 源地址(1B) 目的地址(1B) 长度(1B) 数据体 校验字(1B)              */
/* 未改变原数据帧的大部分定义及长度                                      */
/*-----------------------------------------------------------------------*/
int rev_remote_frame_2(BYTE rev)
{
	if (g_remote_frame_cnt == 0)	//接收帧头
	{
		if (rev == 0xAA)
		{
			remote_frame_data[g_remote_frame_cnt++] = 0xAA;
		}
	}
	else if (g_remote_frame_cnt == 1)	//接收帧头
	{
		if (rev == 0xBB)
		{
			remote_frame_data[g_remote_frame_cnt++] = 0xBB;
		}
		else
		{
			g_remote_frame_cnt = 0;
		}
	}
	else if (g_remote_frame_cnt == 2)	//接收源地址
	{
		remote_frame_data[g_remote_frame_cnt++] = rev;
	}
	else if (g_remote_frame_cnt == 3)	//接收目的地址
	{
		remote_frame_data[g_remote_frame_cnt++] = rev;
		if (rev != g_device_NO && rev != 0xFF && rev != 0xEE)
		{
			g_remote_frame_cnt = 0;	//不是发给本机的
		}
	}
	else if (g_remote_frame_cnt == 4)	//接收长度
	{
		remote_frame_data[g_remote_frame_cnt++] = rev;
		if (rev+6>REMOTE_FRAME_LENGTH)	//判断是否会导致缓冲区溢出
		{
			g_remote_frame_cnt = 0;
		}
	}
	else if (g_remote_frame_cnt>4 && g_remote_frame_cnt<=remote_frame_data[4]+4)	//接收数据区
	{
		remote_frame_data[g_remote_frame_cnt++] = rev;
	}
	else if (g_remote_frame_cnt==remote_frame_data[4]+4+1)	//接收校验字节	
	{
		BYTE sum;
		remote_frame_data[g_remote_frame_cnt++] = rev;
		sum = check_sum((const BYTE *)(remote_frame_data+2), (WORD)(remote_frame_data[4]+3));
		if (sum != remote_frame_data[remote_frame_data[4]+5])
		{
			g_remote_frame_cnt = 0;	//CheckSum Fail
		}
		else
		{
			g_remote_frame_cnt = 0;
			g_remote_frame_state = REMOTE_FRAME_STATE_OK;	//CheckSum Success
		}
	}
	return g_remote_frame_state;
}


/*-----------------------------------------------------------------------*/
/* 生成并发送远程命令                                                    */
/* 第二版                                                                */
/* 修改地址位定义:源地址(1B)+目的地址(1B)                                */
/* AA BB 源地址(1B) 目的地址(1B) 长度(1B) 数据体 校验字(1B)              */
/* 未改变原数据帧的大部分定义及长度                                      */
/* 参数 : scr 源地址                                                     */
/*        des 目标地址                                                   */
/*        cmd WiFi命令字                                                 */
/*        length data长度                                                */
/*        data发出的数据体，接在cmd后                                    */
/*-----------------------------------------------------------------------*/
void generate_remote_frame_2(BYTE scr, BYTE des, WORD cmd, BYTE length, const BYTE data[])
{
	WORD i = 0, j = 0;
	remote_frame_data_send[i++] = 0xAA;
	remote_frame_data_send[i++] = 0xBB;
	remote_frame_data_send[i++] = scr;
	remote_frame_data_send[i++] = des;
	remote_frame_data_send[i++] = length+2;
	remote_frame_data_send[i++] = (BYTE)(cmd>>8);
	remote_frame_data_send[i++] = (BYTE)cmd;
	for (j = 0; j < length; j++)
	{
		remote_frame_data_send[i++] = data[j];
	}
	remote_frame_data_send[i++] = check_sum(remote_frame_data_send+2, i-2);
	for (; i < REMOTE_FRAME_LENGTH; i++)	/* 清空未使用区域 */
	{
		remote_frame_data_send[i] = 0x00;
	}
	serial_port_0_TX_array(remote_frame_data_send, REMOTE_FRAME_LENGTH);
}


/*-----------------------------------------------------------------------*/
/* 发送赛场网络控制命令                                                          */
/* 依赖generate_remote_frame_2()                                          */
/*-----------------------------------------------------------------------*/
void send_net_cmd(BYTE des, WORD net_cmd)
{
	generate_remote_frame_2(g_device_NO, des, WIFI_CMD_NET, sizeof(net_cmd), (const BYTE *)(&net_cmd));
}


/*-----------------------------------------------------------------------*/
/* 异或校验                                                                            */
/*-----------------------------------------------------------------------*/
BYTE check_sum(const BYTE *data, WORD length)
{
	int i;
	BYTE res = 0;
	
	for (i=0; i<length; i++)
	{
		res ^= data[i];
	}
	
	return res;
}


/*-----------------------------------------------------------------------*/
/* 报告在线                                                                            */
/*-----------------------------------------------------------------------*/
void report_online(void)
{
	WORD online = WIFI_CMD_NET_ONLINE;
	
	if (200 < diff_time_basis_PIT(g_time_basis_PIT, g_net_control_data.last_report_online_time))	/* 2秒报告一次 */
	{
		generate_remote_frame_2(g_device_NO, WIFI_ADDRESS_BROADCAST, WIFI_CMD_NET, sizeof(online), (const BYTE *)(&online));
		g_net_control_data.last_report_online_time = g_time_basis_PIT;
	}
	
}


/*-----------------------------------------------------------------------*/
/* 发送远程请求的数据                                                             */
/*-----------------------------------------------------------------------*/
void send_remote_request_data(void)
{
	/* 发送当前速度 */
	if (g_remote_control_flags.send_seppd_now)
	{
		SWORD speed_now_tmp;
		
		if (data_encoder.is_forward)
		{
			speed_now_tmp = (SWORD)(data_encoder.speed_now);
		}
		else
		{
			speed_now_tmp = (SWORD)0 - (SWORD)(data_encoder.speed_now);
		}
		generate_remote_frame_2(g_device_NO, WIFI_ADDRESS_ANDRIUD_ZHOU, WIFI_CMD_GET_SEEED_NOW, sizeof(speed_now_tmp), (const BYTE *)&speed_now_tmp);
	}
	
	/* 发送陀螺仪积分值 */
	/*
	if (g_remote_control_flags.send_gyro_integral)
	{
		generate_remote_frame_2(g_device_NO, WIFI_ADDRESS_ANDRIUD_ZHOU, WIFI_CMD_GET_GYRO_INTEGRAL_DATA, sizeof(rad), (const BYTE *)&rad);
	}
	*/
}
#if 0
void rfid_ask_road(BYTE scr, BYTE des, BYTE length,	WORD cmd ,WORD RFID_Num)
{ 
	WORD i = 0,j = 0;
    byte num_1=0x00,num_2=0x00,num_3=0x00, num_4=0x00;//ou
	byte check;
	int num_temp=0x00000000;
	Temp_Send_Data.scr=scr;
	Temp_Send_Data.des=des;
	Temp_Send_Data.length=length;
	Temp_Send_Data.cmd=cmd;
	Temp_Send_Data.RFID_Num=RFID_Num;
	remote_frame_data_send[i++] = 0xAA;
	remote_frame_data_send[i++] = 0xBB;
	remote_frame_data_send[i++] = scr;
	remote_frame_data_send[i++] = des;
	remote_frame_data_send[i++] = length;
	num_temp=0xFF;
	num_1=(byte)((cmd>>8)&num_temp);
	remote_frame_data_send[i++] = num_1 ;
	num_2=(byte)(cmd&num_temp);
	remote_frame_data_send[i++] = num_2 ;
	num_3=(byte)((RFID_Num>>8)&num_temp);
	remote_frame_data_send[i++] = num_3 ;
	num_4=(byte)(RFID_Num&num_temp);
	remote_frame_data_send[i++] = num_4;
	check=remote_frame_data_send[2];
	for(j=3;j<9;j++)
	{
		check^=remote_frame_data_send[j];
	}
	remote_frame_data_send[i++] = check;
	serial_port_0_TX_array(remote_frame_data_send, 10);//ouyang
}
#endif 
// 以下部分非必须程序，用于2016赛季应答机制//
//*********************************************************************************
//  打包程序 ，用来使程序可读性更好
//*********************************************************************************
void sending_service_package (BYTE toward, WORD cmd, WORD data)
{  
	des=toward;
	cmd_WIFI=cmd;
	sending_data=data;
	main_wifi_sender();
}

//*********************************************************************************
//  主发送程序                 输入： 发送所需的数据      输出： 1 串口发送      2  waiting位     3 串口发送备份给备发送程序    4 发送丢失数
//*********************************************************************************
void main_wifi_sender (void)
{  

//	***********如果依然在等待回复，放弃上一个发送的等待，并且lostdata数加一***************
	if (waiting_for_response==1)
	{
	   lost_data++;
	   waiting_for_response=0;
	}
//	***********发送函数主体***************	                                    
	generate_remote_frame_2(g_device_NO_Hex, des, cmd_WIFI, 2, (const BYTE *)(&sending_data));
//  ***********等待回复位置1*************** 
	waiting_for_response=1;
	have_responsed=0;  
	sending_waiter=0;
}

//*********************************************************************************
//  应答检查程序               定时检查发送的数据是否得到了应答，若未，则使用辅助发送程序再次发送。 直到收到应答或有新的程序要发数据。
//*********************************************************************************
void wifi_sender_checker (void)
{ 
	if (sending_waiter<5)
	{
		return;
	}
	else
	{
		if (waiting_for_response==1)
		{
			if (have_responsed==1)
			{
				waiting_for_response=0;
			}
			else if (have_responsed==0)
			{
				ancillary_wifi_sender ();
			}
		}
	}
}
//*********************************************************************************
//  辅助发送程序                 输入： 如果未应答，再发送数据      输出：  串口发送    
//*********************************************************************************
void ancillary_wifi_sender (void)
{                                      		    
	generate_remote_frame_2(g_device_NO_Hex, des, cmd_WIFI, 2, (const BYTE *)(&sending_data));
}
void Wifi_Ctrl()
{
	if(remote_frame_data[2] == 0x33 )//熊老板上位机
	{
		if(remote_frame_data[5]==0x00 && remote_frame_data[6]==0x66)
		{
			Car_Waitfororder=0;
		}
		if (remote_frame_data[3] == g_device_NO_Hex && remote_frame_data[5]==0x00 && remote_frame_data[6]==0x00)   
		{	
			have_responsed=1;	
		}// 检查是否得到应答 
		if (remote_frame_data[3] == 0x03 && remote_frame_data[5]==0x99 && remote_frame_data[6]==0x01)   
		{	
			End=1;	
		}// 第一幕终止 
		if (remote_frame_data[3] == 0xEE && remote_frame_data[5]==0x00 && remote_frame_data[6]==0x01)   
		{
			if(remote_frame_data[8]==0x0A)
				Light_Status=0;	
			if(remote_frame_data[8]==0x0B)
				Light_Status=1;	
		}// 红绿灯状态
		if (remote_frame_data[3] == 0xEE && remote_frame_data[5]==0x00 && remote_frame_data[6]==0x88 && remote_frame_data[8]==0x88)   
		{
			order_received =1;
			Traffic_Jam=1;
		}// 拥堵状态	
		if (remote_frame_data[3] == 0xEE && remote_frame_data[5]==0x00 && remote_frame_data[6]==0x2A && remote_frame_data[8]==0x2A)   
		{
			order_received =1;
			Emergency=1;
		}// 紧急状态	
		if (remote_frame_data[3] == 0xEE && remote_frame_data[5]==0x00 && remote_frame_data[6]==0x2B && remote_frame_data[8]==0x2B)   
		{
			order_received =1;
			Emergency=0;
		}// 紧急状态	解除
	}
	if(remote_frame_data[2] == 0x44 && remote_frame_data[3] == g_device_NO_Hex)//天少发过来
	{
		if ( remote_frame_data[5]==0x00 && remote_frame_data[6]==0x00)
				have_responsed=1;	// 检查天少是否回答 
		if(g_device_NO == 3)
		{

			if(remote_frame_data[5]==0x00 && remote_frame_data[6]==0x66)
			{
				used=0;
				if(remote_frame_data[7]==0x01)
					place[1]=1;
				if(remote_frame_data[7]==0x02)
					place[2]=1;
				if(remote_frame_data[7]==0x03)
					place[3]=1;
				if(remote_frame_data[8]==0x01)
					place[1]=2;
				if(remote_frame_data[8]==0x02)
					place[2]=2;
				if(remote_frame_data[8]==0x03)
					place[3]=2;
				Car_Waitfororder=0;
				sending_service_package(0x44,0x0000,0xAAAA);
				delay_ms(200);
				sending_service_package(0x44,0x0000,0xAAAA);
			}
			if(remote_frame_data[5]==0x00 && remote_frame_data[6]==0x0B)
				Door_Open=1;	//天少远程开门
			if(remote_frame_data[5]==0x00 && remote_frame_data[6]==0xCB)
			{
				if(!Door_Status)
					Door_Close=1;
			}
			if (remote_frame_data[5]==0x00 && remote_frame_data[6]==0x01) 
			{
				if(!Door_Status)
					Car_Stop=0;//天少开车
				sending_service_package(0xEE,0xBBBB,0x0);
				delay_ms(100);
				sending_service_package(0xEE,0xBBBB,0x0);
			}
			if (remote_frame_data[5]==0x00 && remote_frame_data[6]==0xCC) 
				Door_Close_Run=1;//关门并开车
			if (remote_frame_data[5]==0x00 && remote_frame_data[6]==0x3B) 
			{
				if (remote_frame_data[8]==0x01)
				{
					RFID_site_data.is_new_site = 1 ;
					RFID_site_data.old_site=RFID_site_data.site;
					RFID_site_data.site=ROAD_NUM_2501;
				}
				if (remote_frame_data[8]==0x02)
				{
					RFID_site_data.is_new_site = 1 ;
					RFID_site_data.old_site=RFID_site_data.site;
					RFID_site_data.site=ROAD_NUM_1101;
				}
				if (remote_frame_data[8]==0x03)
				{
					RFID_site_data.is_new_site = 1 ;
					RFID_site_data.old_site=RFID_site_data.site;
					if(place[1]==1)
						RFID_site_data.site=ROAD_NUM_4001;
					else if(place[2]==1)
						RFID_site_data.site=ROAD_NUM_4002;
					else if(place[3]==1)
						RFID_site_data.site=ROAD_NUM_4003;
				}
				if (remote_frame_data[8]==0x04)
				{
					RFID_site_data.is_new_site = 1 ;
					RFID_site_data.old_site=RFID_site_data.site;
					if(place[1]==2)
						RFID_site_data.site=ROAD_NUM_4001;
					else if(place[2]==2)
						RFID_site_data.site=ROAD_NUM_4002;
					else if(place[3]==2)
						RFID_site_data.site=ROAD_NUM_4003;
				}					
			}
		}
	}
}


