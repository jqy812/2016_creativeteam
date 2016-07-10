#include "includes.h"
BYTE rev_ch;
WORD helm_use=0;
int16_t motor_use=0;
int direction;
BYTE haha;
BYTE high2;
BYTE low2;
BYTE high3;
BYTE low3;
BYTE device_number2;
BYTE data_number;
int supersonic_on_off=1;
int biaoji=0;
int jishu=0;
int zhangai=1;
int i;
int j;
extern int bz;
BYTE video_sender;

void Mode0_DebugCamera(void);
void Mode1_SendVideo(void);
void Mode2_GO(void);
void Mode3_Andriod(void);

void main(void)
{
	init_all_and_POST();

	if(mode==0)
		Mode0_DebugCamera();//图像显示屏显示，车速20，显示offset RoadType，舵机打角，wifi_car_action不激活
	else if(mode==1)
		Mode1_SendVideo();//推车录图像，仅摄像头图像发上位机
	else if(mode==2)
		Mode2_GO();//速度20，WIFI读卡循迹超声全开，图像不显示不发送
	else if(mode==3)
		Mode3_Andriod();//远程模式，上位机遥控车
}
void Mode0_DebugCamera(void)
{
//	set_speed_pwm(300); 
	EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断
//	LCD_write_english_string(96,0,"T");
//	LCD_write_english_string(96,2,"R");
	jishu=0;
#if 0
	while(bz==-1)
	{
		set_speed_pwm(-300); 
		control_car_action();
	}
#endif		
	bz=-1;
	for (;;)
	{
	//	LCD_Write_Num(105,4,bz,2);
		control_car_action();//ouyang
		if(WIFICHEKER==1)            // 有一个时间间隔为了 保证在没有收到的时候不会发疯一样发
		{
			WIFICHEKER=0;
			wifi_sender_checker();//每次检查一次是否收到回复  注意：子函数在被设计为发送完一定时间内不会工作，防止对方还没回答这里不停发
		}
		if (REMOTE_FRAME_STATE_OK == g_remote_frame_state)
		{
			g_remote_frame_state = REMOTE_FRAME_STATE_NOK;
					
			execute_remote_cmd(remote_frame_data+5);
		}
		if(fieldover==1&&Car_Stop==0&&zhangai==1)
		{
			car_default();
			fieldover=0; 
			set_speed_pwm(300); 
			FindBlackLine();              //寻迹处理                        jqy     
	    	//Display_Video();
			CenterLineWithVideo();        //摄像头数据处理              jqy     
	     	Video_Show();                 //显示屏显示                     jqy
	    // 	Send_CCD_Video();             //发送图像给wifi        jqy
	    //	supersonic();                 //超声                                jqy
#if 0
	     	for(video_sender=0;video_sender<70;video_sender++)
	     	{
	     		serial_port_0_TX(CenterLine[video_sender]);
	     	}
#endif
	     	
			if(target_offset<0)           //显示偏差值及赛道类型      jqy
				LCD_write_english_string(96,1,"-");
			else LCD_write_english_string(96,1,"+");
			LCD_Write_Num(105,1,ABS(target_offset),2);
			LCD_Write_Num(105,2,RoadType,2);
			LCD_Write_Num(105,6,bz,2);
	//		if(RoadType==12 || RoadType==13)
	//		{
	//			jishu++;
			//	set_speed_pwm(200);
	//		}
	//		else
			if(RoadType==66)      //道路类型66，表示单车道障碍    jqy
				jishu++;
			if(jishu>=6 && bz==1)   //bz位为1时，进行超车    jqy
			{
				zhangai=0;
			//	set_speed_pwm(0);
				jishu=0;
			}
		//	if(jishu==10 && bz==-1)
			if(jishu>=8 && bz==0)   //bz位为1时，进行避障停车   jqy
			{
				zhangai=0;
				jishu=0;
			}
			if(RoadType==88 && bz==2)    //道路类型88，表示双车道障碍，需掉头绕行    jqy
			{
			//	delay_ms(100);
				set_speed_pwm(0);
				delay_ms(2000);
				set_steer_helm_basement(data_steer_helm_basement.left_limit);
				set_speed_pwm(300);
				delay_ms(1600);/////
				set_speed_pwm(0);
				set_steer_helm_basement(data_steer_helm_basement.right_limit);
				delay_ms(1000);
				set_speed_pwm(-300);
				delay_ms(1200);/////
				set_speed_pwm(0);
				set_steer_helm_basement(data_steer_helm_basement.left_limit);
				delay_ms(1000);
				set_speed_pwm(300);
				delay_ms(1500);/////
			}
#if 0                                     //应用于之后直角转弯程序（初版，未经测试）                  jqy
			if(flag_Rightangle_l)	
			{ 
				set_speed_pwm(500);
				set_steer_helm_basement(data_steer_helm_basement.left_limit);   
				LCD_Write_Num(105,3,(int)flag_Rightangle_l,4);
			}
			else if(flag_Rightangle_r)		
			{ 
				set_speed_pwm(500);
				set_steer_helm_basement(data_steer_helm_basement.right_limit);   
				LCD_Write_Num(105,4,(int)flag_Rightangle_r,4);
			}
			else
#endif
				SteerControl();          //舵机控制              jqy

			EMIOS_0.CH[3].CSR.B.FLAG = 1;
			EMIOS_0.CH[3].CCR.B.FEN=1;
		}
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
}

void Mode1_SendVideo(void)
{
	//SetupCCD();	
	g_f_enable_speed_control = 0;
	EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断

	for (;;)
	{

		if(fieldover)
		{
			fieldover=0;
			FindBlackLine();
		//	Send_CCD_Video();
			
			EMIOS_0.CH[3].CSR.B.FLAG = 1;
			EMIOS_0.CH[3].CCR.B.FEN=1;
		}
		
	}

}

void Mode2_GO(void)
{
	//set_steer_helm_basement(3800);
	set_speed_pwm(240);
	EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断

#if 0
	for (;;)
	{
		trigger_supersonic_0();
        get_supersonic_time_0();
	    trigger_supersonic_2();
		get_supersonic_time_2();
#if 0
		if((ABS((WORD)(tmp_time.R))/100)<100)
		{
			trigger_supersonic_2();
			get_supersonic_time_2();
			LCD_Write_Num(96,6,(ABS((WORD)(tmp_time.R))/100),5);
			set_speed_pwm(0);
		}
#endif
		if((ABS((WORD)(tmp_time2.R))/100)<70 && (ABS((WORD)(tmp_time.R))/100)<100)
			jishu++;
		else if((ABS((WORD)(tmp_time2.R))/100)>70 && (ABS((WORD)(tmp_time.R))/100)>100)
			jishu=0;
		if((ABS((WORD)(tmp_time2.R))/100)<270 && (ABS((WORD)(tmp_time.R))/100)<270)
		{
			biaoji++;
			trigger_supersonic_0();
			get_supersonic_time_0();
			trigger_supersonic_2();
			get_supersonic_time_2();
			LCD_Write_Num(96,6,(ABS((WORD)(tmp_time2.R))/100),5);
			LCD_Write_Num(96,5,(ABS((WORD)(tmp_time.R))/100),5);
		}
		else if((ABS((WORD)(tmp_time2.R))/100)>270 && (ABS((WORD)(tmp_time.R))/100)>270)
		{
			biaoji=0;
			trigger_supersonic_0();
			get_supersonic_time_0();
			trigger_supersonic_2();
			get_supersonic_time_2();
			LCD_Write_Num(96,6,(ABS((WORD)(tmp_time2.R))/100),5);
			LCD_Write_Num(96,5,(ABS((WORD)(tmp_time.R))/100),5);
		}

	//	else if((ABS((WORD)(tmp_time.R))/100)<100)
	//	{
	//		set_speed_pwm(0);
	//		delay_ms(3000);
	//	}
		if(biaoji>=300)
		{
		//	trigger_supersonic_0();
		//	get_supersonic_time_0();
		//	trigger_supersonic_2();
		//	get_supersonic_time_2();
		//	LCD_Write_Num(96,6,(ABS((WORD)(tmp_time.R))/100),5);	
			//for (i = 0; i < 500; i++)
			//	{
				//	for (j = 0; j < 9; j++)
				///	{			
						set_steer_helm_basement(4800);
						set_speed_pwm(210);
						delay_ms(1000);
					//	trigger_supersonic_2();
					//	get_supersonic_time_2();
					//	LCD_Write_Num(96,6,(ABS((WORD)(tmp_time.R))/100),5);
					//}
			//	}
			//for (i = 0; i < 500; i++)
				//{
				//	for (j = 0; j < 9; j++)
				//	{			
						set_steer_helm_basement(2700);
						set_speed_pwm(320);
						delay_ms(1300);
					//	trigger_supersonic_2();
					//	get_supersonic_time_2();
					//	LCD_Write_Num(96,6,(ABS((WORD)(tmp_time.R))/100),5);
				//	}
				//}
			set_steer_helm_basement(STEER_HELM_CENTER);
			set_speed_pwm(240);
		//	delay_ms(2000);//////////////
			biaoji=0;
			jishu=0;
			//jishu++;
			//LCD_Write_Num(0,0,(ABS((WORD)(jishu))/100),5);
			//biaoji=0;
		//	set_speed_pwm(0); ////////////////
			//suicide();////////////
		}
		trigger_supersonic_0();
		get_supersonic_time_0();
		trigger_supersonic_2();
		get_supersonic_time_2();
		if(jishu>=100)
		{
			set_speed_pwm(0);
			suicide();
		}
	}
#endif

	for (;;)
	{
	    trigger_supersonic_2();
		get_supersonic_time_2();
		if((ABS((WORD)(tmp_time2.R))/100)<270)
		{
			biaoji++;
			trigger_supersonic_2();
			get_supersonic_time_2();
			LCD_Write_Num(96,6,(ABS((WORD)(tmp_time2.R))/100),5);
		}
		else if((ABS((WORD)(tmp_time2.R))/100)>270)
		{
			biaoji=0;
			trigger_supersonic_2();
			get_supersonic_time_2();
			LCD_Write_Num(96,6,(ABS((WORD)(tmp_time2.R))/100),5);
		}
		if(biaoji>=300)
		{	
			set_steer_helm_basement(4800);
			set_speed_pwm(210);
			delay_ms(1000);
			set_steer_helm_basement(2700);
			set_speed_pwm(210);
			delay_ms(1300);
			set_steer_helm_basement(STEER_HELM_CENTER);
			set_speed_pwm(240);
			biaoji=0;
		}
		EMIOS_0.CH[3].CSR.B.FLAG = 1;
		EMIOS_0.CH[3].CCR.B.FEN=1;
	}

	
#if 0
		if(biaoji==0)
		{
			while((jishu-jia)>0)
			{
				for (i = 0; i < 10; i++)
				{
					for (j = 0; j < 9; j++)
					{			
						set_steer_helm_basement(2802);
						set_speed_pwm(300);
						trigger_supersonic_2();
						get_supersonic_time_2();
						LCD_Write_Num(96,6,(ABS((WORD)(tmp_time.R))/100),5);
					}
				}
				jishu--;
			}
			biaoji=1;
			
		}
#endif
		LCD_Fill(0x00);
for (;;)
{
		//set_speed_pwm(300);
	//	set_steer_helm_basement(3875);
		/* 执行远程命令 */
		if (REMOTE_FRAME_STATE_OK == g_remote_frame_state)
		{
			g_remote_frame_state = REMOTE_FRAME_STATE_NOK;
			
			execute_remote_cmd(remote_frame_data+5);
		}

		/* 整车动作控制 */
		//control_car_action();
		//fieldover=1;//尝试
		if(fieldover)
		{
			fieldover=0;
			
			FindBlackLine();
		//	if(g_f_red==1&&g_f_stopline==1)
		//	{
				//set_speed_target(0);
			//	set_speed_pwm(0);
			//	D6=~D6;
			//}
			//else
				set_speed_pwm(300);
				//set_speed_target(20);
			Display_Video();



						if(target_offset<0)
							LCD_write_english_string(96,1,"-");
						else LCD_write_english_string(96,1,"+");
						LCD_Write_Num(105,1,ABS(target_offset),2);
						LCD_Write_Num(105,2,RoadType,2);
			SteerControl();
			
			EMIOS_0.CH[3].CSR.B.FLAG = 1;
			EMIOS_0.CH[3].CCR.B.FEN=1;
		}
}
}

void Mode3_Andriod(void)
{
	for(;;)
    {
		
#if 1
			LCD_PrintoutInt(0, 0, (int)Light_Status);
			
			high2=((WORD)(remote_frame_data[5])<<8);
			low2=(WORD)(remote_frame_data[6]);
			device_number2=(high2|low2);
			LCD_PrintoutInt(0, 4, (int)device_number2);
		
			high3=((WORD)(remote_frame_data[7])<<8);
			low3=(WORD)(remote_frame_data[8]);
			data_number=(high3|low3);
			LCD_PrintoutInt(0, 6, (int)data_number);
			
			
			if ( device_number2==1)
				{
					set_steer_helm_basement(data_number);
				}
			else if (device_number2==5)
				{
					set_speed_pwm(data_number);
				}
	   }
#endif
//		if(sending_test==1)          
//		{
//			sending_test=0;
//			sending_service_package(0x33,0x0001,0x000a);// BYTE 目标地址 WORD 命令字 WORD 数据字
//		}
//		
//		if(WIFICHEKER==1)            // 有一个时间间隔为了 保证在没有收到的时候不会发疯一样发
//		{
//			WIFICHEKER=0;
//			wifi_sender_checker();//每次检查一次是否收到回复  注意：子函数在被设计为发送完一定时间内不会工作，防止对方还没回答这里不停发
//		}
//    }
	
}
