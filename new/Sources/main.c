#include "includes.h"
BYTE rev_ch;
WORD helm_use=0;
int16_t motor_use=0;
int direction;
BYTE haha;
int supersonic_on_off=1;
int biaoji=0;
int jishu=0;
int jia=0;
int i;
int j;
int LichtZahl=0;
int Light_success=1;
int TestLight=0;
BYTE video_sender;

BYTE waiting_for_response=0;
int  lost_data=0;
BYTE prepared_data_for_ancillary;

void Mode0_DebugCamera(void);
void Lightcontrol(void);
void Light_wifi(BYTE LightCon);
void Mode1_SendVideo(void);
void Mode2_GO(void);
void Mode3_Andriod(void);

void main_wifi_sender (BYTE data_input);
void ancillary_wifi_sender (BYTE data_input2);
void wifi_sender_checker(void);
void wifi_receive_checker (void);
void main(void)
{
	init_all_and_POST();
	LCD_Fill(0x00);			
	EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断
	Lightcontrol();
}



void Lightcontrol(void)
{
	RedL_main=1;
	YellowL_main=0;
	GreenL_main=0;
	RedL_main2=1;
	YellowL_main2=0;
	GreenL_main2=0;
	for(;;)
	{
		if (REMOTE_FRAME_STATE_OK == g_remote_frame_state)
		{
			g_remote_frame_state = REMOTE_FRAME_STATE_NOK;
			Wifi_Ctrl();
			//rfid_ask_road(0x05, 0x33, 0x04, 0x00000000);//device_05
		}
		if(1)//Emergency_Mode==0 || mode==3
		{
			if(Light_Status==0)//红灯
			{
				if(GreenL_main==1)
				{
					YellowL_main=1;				
					RedL_main=0;				
					GreenL_main=0;
					LightCWifi=1;
				}
				if(LightCC==1)
				{
					LightCC=0;
					YellowL_main=0;
					GreenL_main=0;
					RedL_main=1;
					GreenL_main2=1;
					YellowL_main2=0;
					RedL_main2=0;
					LightCWifi2=1;
				}
			}
			if(LightCC2==1)
			{
				LightCC2=0;
				GreenL_main2=1;
				//YellowL_main2=1;
				RedL_main2=0;
			}
			if(Light_Status==1)//绿灯
			{
				GreenL_main=1;
				YellowL_main=0;
				RedL_main=0;
				GreenL_main2=0;
				YellowL_main2=0;
				RedL_main2=1;
			}
		}
//		if(Emergency_Mode==1 && mode==1)
//		{
//			RedL_main=1;
//			RedL_main2=1;
//			GreenL_main=0;
//			GreenL_main2=0;
//			YellowL_main=0;
//			YellowL_main2=0;
//		}
	}
}



//*********************************************************************************
//  主发送程序                 输入： 发送所需的数据      输出： 1 串口发送      2  waiting位     3 串口发送备份给备发送程序    4 发送丢失数
//*********************************************************************************
void main_wifi_sender (BYTE data_input)
{  
//	***********如果依然在等待回复，放弃上一个发送的等待，并且lostdata数加一***************
	if (waiting_for_response==1)
	{
	   lost_data++;
	   waiting_for_response=0;
	}
//	***********发送函数主体***************	                                    
	if(data_input==0x0A)				
		
//		    rfid_ask_road(0xDD, 0x33, 0x04, 0x00DD000A);
		rfid_ask_road(0x33, 0xEE, 0x04, 0x0001000A);

	if(data_input==0x0B)
//			rfid_ask_road(0xDD, 0x33, 0x04, 0x00DD000B);
		rfid_ask_road(0x33, 0xEE, 0x04, 0x0001000B);
//  ***********把发送数据交给辅助发送程序*************** 
	prepared_data_for_ancillary=data_input;
//  ***********等待回复位置1*************** 
	waiting_for_response=1;
	have_responsed=0;  
	sending_waiter=0;
}




//*********************************************************************************
//  辅助发送程序                 输入： 如果未应答，再发送数据      输出：  串口发送    
//*********************************************************************************
void ancillary_wifi_sender (BYTE data_input2)
{                                      
	if(data_input2==0x0A)				
		
	//	    rfid_ask_road(0xDD, 0x33, 0x04, 0x00DD000A);
		rfid_ask_road(0x33, 0xEE, 0x04, 0x0001000A);

	if(data_input2==0x0B)
			//rfid_ask_road(0xDD, 0x33, 0x04, 0x00DD000B);
		rfid_ask_road(0x33, 0xEE, 0x04, 0x0001000B);
}



//*********************************************************************************
//  应答检查程序               定时检查发送的数据是否得到了应答，若未，则使用辅助发送程序再次发送。 直到收到应答或有新的程序要发数据。
//*********************************************************************************
void wifi_sender_checker (void)
{ 
	if (sending_waiter<1)
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
				ancillary_wifi_sender (prepared_data_for_ancillary);
			}
		}
	}
}


//*********************************************************************************
//  收数据检查程序              如果受到了非应答的程序（即实打实的命令），回复收到
//*********************************************************************************
void wifi_receive_checker (void)
{
	if (order_received == 1)
	{
		order_received=0;
		rfid_ask_road(0xDD, 0x33, 0x04, 0x00000000);
	}
}
