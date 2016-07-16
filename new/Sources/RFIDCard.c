﻿#define __RFIDCARD_C_
#include "includes.h"


const BYTE rfid_cmd_get_id[] = { 0xAA, 0xBB, 0x02, 0x20, 0x22 };
const BYTE rfid_cmd_get_data[] = { 0xAA, 0xBB, 0x0A, 0x21, 0x00, 0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2F };
const BYTE rfid_cmd_energetic_mode_enable[] = { 0xAA, 0xBB, 0x03, 0x41, 0x15, 0x57 };
const BYTE rfid_cmd_energetic_mode_enable_new[] = { 0xAA, 0xBB, 0x05, 0x00, 0x01, 0x41, 0x15, 0x50 };
const BYTE rfid_cmd_energetic_mode_disable[] = { 0xAA, 0xBB, 0x03, 0x41, 0x00, 0x42 };
const BYTE rfid_cmd_energetic_mode_disable_new[] = { 0xAA, 0xBB, 0x05, 0x00, 0x01, 0x41, 0x00, 0x45 };
int g_rfid_frame_state = REMOTE_FRAME_STATE_NOK;
int g_rfid_frame_cnt = 0;
BYTE rfid_frame_data[RFID_FRAME_LENGTH_MAX];
BYTE rfid_frame_data_send[RFID_FRAME_LENGTH_MAX];
static int RFID_modul_type = RFID_MODUL_TYPE_UNKNOW;
//DWORD rfid_site = 0x00000000;


/*-----------------------------------------------------------------------*/
/* Send RFID cmd                                                         */
/*-----------------------------------------------------------------------*/
static void send_RFID_cmd(const BYTE cmd[])
{
	serial_port_2_TX_array(cmd, (WORD)(cmd[2]+3));
}


/*-----------------------------------------------------------------------*/
/* Receive RFID frame                                                    */
/*-----------------------------------------------------------------------*/
int rev_RFID_frame(BYTE rev)
{
	if (g_rfid_frame_cnt == 0)	//Receive frame head
	{
		if (rev == 0xAA)
		{
			rfid_frame_data[g_rfid_frame_cnt++] = 0xAA;
		}
	}
	else if (g_rfid_frame_cnt == 1)	//Receive frame head
	{
		if (rev == 0xBB)
		{
			rfid_frame_data[g_rfid_frame_cnt++] = 0xBB;
		}
		else
		{
			g_rfid_frame_cnt = 0;
		}
	}
	else if (g_rfid_frame_cnt == 2)	//Receive length
	{
		rfid_frame_data[g_rfid_frame_cnt++] = rev;
		if (rev+3>RFID_FRAME_LENGTH_MAX)	//Overflow or not
		{
			g_rfid_frame_cnt = 0;
		}
	}
	else if (g_rfid_frame_cnt>2 && g_rfid_frame_cnt<=rfid_frame_data[2]+1)	//Receive data area
	{
		rfid_frame_data[g_rfid_frame_cnt++] = rev;
	}
	else if (g_rfid_frame_cnt==rfid_frame_data[2]+2)	//Receive check BYTE
	{
		BYTE sum;
		
		rfid_frame_data[g_rfid_frame_cnt++] = rev;
		sum = check_sum((const BYTE *)(rfid_frame_data+2), (WORD)(rfid_frame_data[2]));
		if (sum != rfid_frame_data[rfid_frame_data[2]+2])
		{
			g_rfid_frame_cnt = 0;	//CheckSum Fail
		}
		else
		{
			g_rfid_frame_cnt = 0;
			g_rfid_frame_state = RFID_FRAME_STATE_OK;	//CheckSum Success
			explane_RFID_ret_data((const BYTE *)(rfid_frame_data+3), (WORD)(rfid_frame_data[2]-1));
		}
	}
	
	return g_rfid_frame_state;
}


/*-----------------------------------------------------------------------*/
/* Explane RFID return data aera                                         */
/*-----------------------------------------------------------------------*/
void explane_RFID_ret_data(const BYTE *data, WORD length)
{
	WORD cmd = 0;
	DWORD cardID = 0x00000000;

	if (RFID_MODUL_TYPE_UNKNOW == RFID_modul_type)	/* 检测卡的类型 */
	{
		if (4 == length && 0x00014115 == *((DWORD *)data))
		{
			//g_devices_init_status.RFIDCard_energetic_mode_enable_is_OK = 1;
			RFID_modul_type = RFID_MODUL_TYPE_NEW;
		}
		else if (2 == length && 0x4115 == *((WORD *)data))
		{
			//g_devices_init_status.RFIDCard_energetic_mode_enable_is_OK = 1;
			RFID_modul_type = RFID_MODUL_TYPE_OLD;
		}
		return;
	}
	else	/* 卡类型已知 执行相应命令 */
	{
		if (5 == length && RFID_CMD_ENERGETIC_MODE == data[0])
		{
			cardID = *(DWORD *)(data+1);
			//explane_RFID_ret_cardID(cardID);
			RFID_site_data.old_site=RFID_site_data.site;//余婧添加
			RFID_site_data.site = cardID;
			RFID_site_data.is_new_site = 1;
			RFID_site_data.time = g_time_basis_PIT;
			RFID_site_data.roadnum=RFID_Num_Exp(cardID);
		}
	}
}


/*-----------------------------------------------------------------------*/
/* Explane RFID card ID                                                  */
/*-----------------------------------------------------------------------*/
void explane_RFID_ret_cardID(DWORD id)
{
	RFID_site_data.old_site=RFID_site_data.site;//余婧添加
	RFID_site_data.site = id;
	RFID_site_data.is_new_site = 1;
	RFID_site_data.time = g_time_basis_PIT;
	//serial_port_1_TX_array((BYTE *)&id, sizeof(id));
}


/*-----------------------------------------------------------------------*/
/* 初始化模块                                                            */
/* 返回0成功                                                             */
/* 返回其他失败                                                          */
/*-----------------------------------------------------------------------*/
int init_RFID_modul_type(void)
{
	send_RFID_cmd(rfid_cmd_energetic_mode_enable);
	delay_ms(100);
	send_RFID_cmd(rfid_cmd_energetic_mode_enable_new);
	delay_ms(100);
	
	if (RFID_MODUL_TYPE_UNKNOW == RFID_modul_type)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*-----------------------------------------------------------------------*/
/* RFID卡对应路段  by 欧阳                                                          */
/*-----------------------------------------------------------------------*/
WORD RFID_Num_Exp(DWORD site)
{
	if(site== ROAD_NUM_1101)
		return 0x1101;
	if(site== ROAD_NUM_1102)
		return 0x1102;
	if(site== ROAD_NUM_1103)
		return 0x1103;
	if(site== ROAD_NUM_1104)
		return 0x1104;
	if(site== ROAD_NUM_1105)
		return 0x1105;
	if(site== ROAD_NUM_1106)
		return 0x1106;
	if(site== ROAD_NUM_1201)
		return 0x1201;
	if(site== ROAD_NUM_1202)
		return 0x1202;
	if(site== ROAD_NUM_2101)
		return 0x2101;
	if(site== ROAD_NUM_2102)
		return 0x2102;
	if(site== ROAD_NUM_2103)
		return 0x2103;
	if(site== ROAD_NUM_2201)
		return 0x2201;
	if(site== ROAD_NUM_2202)
			return 0x2202;
	if(site== ROAD_NUM_2203)
			return 0x2203;
	if(site== ROAD_NUM_2301)
		return 0x2301;
	if(site== ROAD_NUM_2401)
		return 0x2401;
	if(site== ROAD_NUM_2402)
		return 0x2402;
	if(site== ROAD_NUM_3001)
		return 0x3001;
	if(site== ROAD_NUM_3002)
		return 0x3002;
	if(site== ROAD_NUM_3003)
		return 0x3003;
	if(site== ROAD_NUM_3004)
		return 0x3004;
	if(site== ROAD_NUM_3101)
		return 0x3101;
	if(site== ROAD_NUM_3102)
		return 0x3102;
	if(site== ROAD_NUM_3201)
		return 0x3201;
	if(site== ROAD_NUM_3202)
		return 0x3202;
	if(site== ROAD_NUM_4001)
		return 0x4001;
	if(site== ROAD_NUM_4002)
		return 0x4002;
	if(site== ROAD_NUM_4101)
		return 0x4101;
	if(site== ROAD_NUM_4102)
		return 0x4202;
}
