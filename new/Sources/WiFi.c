#define __WIFI_C_
#include "includes.h"


int g_remote_frame_state = REMOTE_FRAME_STATE_NOK;
int g_remote_frame_cnt = 0;
int g_start_all=0;
BYTE remote_frame_data[REMOTE_FRAME_LENGTH];
BYTE remote_frame_data_send[REMOTE_FRAME_LENGTH];
BYTE g_device_NO = WIFI_ADDRESS_CAR_1;	/* 设备号 即WiFi地址 */


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
		if (rev != g_device_NO && rev != 0xFF)
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
