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
BYTE video_sender;

void Mode0_DebugCamera(void);
void Mode1_SendVideo(void);
void Mode2_GO(void);
void Mode3_Andriod(void);

void main(void)


{
	init_all_and_POST();
//	if(mode==0)
		Mode0_DebugCamera();//图像显示屏显示，车速20，显示offset RoadType，舵机打角，wifi_car_action不激活
//	else if(mode==1)
//		Mode1_SendVideo();//推车录图像，仅摄像头图像发上位机
//	else if(mode==2)
//		Mode2_GO();//速度20，WIFI读卡循迹超声全开，图像不显示不发送
//	else if(mode==3)
//		Mode3_Andriod();//远程模式，上位机遥控车
}
void Mode0_DebugCamera(void)
{
//	set_speed_target(20);
	EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断
	LCD_write_english_string(96,0,"T");
	LCD_write_english_string(96,2,"R");
	
	for (;;)
	{
		//control_car_action();//rfid_ou

		if(fieldover)
		{
			fieldover=0;                                              
			set_speed_pwm(800); 
			FindBlackLine();
	    //	Display_Video();
			CenterLineWithVideo();
	     	Video_Show();
	    //	supersonic();
#if 1
	     	for(video_sender=0;video_sender<70;video_sender++)
	     	{
	     		serial_port_0_TX(CenterLine[video_sender]);
	     	}
#endif
	     	
			if(target_offset<0)
				LCD_write_english_string(96,1,"-");
			else LCD_write_english_string(96,1,"+");
			LCD_Write_Num(105,1,ABS(target_offset),2);
			LCD_Write_Num(105,2,RoadType,2);
#if 0
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
				SteerControl();

			EMIOS_0.CH[3].CSR.B.FLAG = 1;
			EMIOS_0.CH[3].CCR.B.FEN=1;
		}

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
	 LCD_Fill(0x00);
	for(;;)
    {
		   // LCD_PrintoutInt(48, 0, (int)haha);
		  // LCD_PrintoutInt(48, 0, (int)shuzi);
		   // LCD_PrintoutInt(48, 0, (int)rev_ch);
		    LCD_PrintoutInt(48, 0, (SWORD)motor_use);
		  
		// ---------------------交给控制--------------------	
		   if(haha==1)
	        {
			   if (direction==1)
			
		    	{
		    		set_steer_helm_basement(helm_use); //舵机调参
		    	}
		        else if (direction==5)
		    	{
		    		set_speed_pwm(motor_use);
		    	}
		        else if (direction==11)
				{
		        	supersonic_on_off=~supersonic_on_off;
				}
		     haha=0;
	        } 
		   if (supersonic_on_off)
		   {
			   trigger_supersonic_0();
			   get_supersonic_time_0();
			   trigger_supersonic_1();
			   get_supersonic_time_1();
			   while(((ABS((WORD)(tmp_time.R))/100)<200)  && (motor_use>0))
			   {
					trigger_supersonic_0();
					get_supersonic_time_0();
					trigger_supersonic_1();
					get_supersonic_time_1();
					LCD_Write_Num(96,6,(ABS((WORD)(tmp_time.R))/100),5);
					set_speed_pwm(0);
				}	
				LCD_Fill(0x00);
				set_speed_pwm(motor_use);
		   }
	}
	
}
