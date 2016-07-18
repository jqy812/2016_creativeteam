#ifndef __RFIDCARD_H__
#define __RFIDCARD_H__

#define RFID_CMD_READ_CARD (0x20)
#define RFID_CMD_READ_DATA_BLOCK (0x21)
#define RFID_CMD_ENERGETIC_MODE (0x41)

#define RFID_CARD_ID
#define RFID_CARD_ID_PUNCTURED_BALLON_START (0x0A1E4748)
#define RFID_CARD_ID_PUNCTURED_BALLON_TURN_1 (0x4AF14448)
#define RFID_CARD_ID_PUNCTURED_BALLON_STOP (0x1250DD76)
#define RFID_CARD_ID_PUNCTURED_BALLON_CAR4_TURN1 (0xCA7C00BD)
#define RFID_CARD_ID_PUNCTURED_BALLON_CAR3_TURN1 (0xDA8220BD)
#define RFID_CARD_ID_PUNCTURED_BALLON_CAR4_TURN2 (0xDE913FC9)
#define RFID_CARD_ID_PUNCTURED_BALLON_CAR3_TURN2 (0x4AF14448)
#define RFID_CARD_ID_1 (0xDAA23548)
#define RFID_CARD_ID_2 (0xFA8320BD)

#define RFID_CARD_ID_1_1 (0x6AEEFEBC)
#define RFID_CARD_ID_1_2 (0x4A72FFBC)
#define RFID_CARD_ID_1_3 (0x4AF14448)
#define RFID_CARD_ID_1_4 (0xDA945248)
#define RFID_CARD_ID_2_1 (0xB49C2C52)
#define RFID_CARD_ID_2_2 (0x344F2952)
#define RFID_CARD_ID_2_3 (0xE44B2852)
#define RFID_CARD_ID_2_4 (0x1250DD76)  //big card
#define RFID_CARD_ID_3_1 (0x04E32852)
#define RFID_CARD_ID_3_2 (0x24572C52)
#define RFID_CARD_ID_3_3 (0xF4812652)
#define RFID_CARD_ID_4_1 (0x5A934B48)
#define RFID_CARD_ID_4_2 (0xCA7C00BD)
#define RFID_CARD_ID_4_3 (0xDA8220BD)
#define RFID_CARD_ID_5_1 (0xDAA23548)
#define RFID_CARD_ID_5_2 (0xA41C2F52)
#define RFID_CARD_ID_5_3 (0x4A4A00BD)
#define RFID_CARD_ID_5_4 (0x74642952)
#define RFID_CARD_ID_5_5 (0xF4A22952)
#define RFID_CARD_ID_6_1 (0x040E2752)
#define RFID_CARD_ID_6_2 (0x64E12952)
#define RFID_CARD_ID_6_3 (0x64282D52)
#define RFID_CARD_ID_6_4 (0x04172952)
#define RFID_CARD_ID_6_5 (0x00000000)
#define RFID_CARD_ID_6_6 (0x00000000)
#define RFID_CARD_ID_7_1 (0x74012852)
#define RFID_CARD_ID_7_2 (0xD4CB2952)
#define RFID_CARD_ID_7_3 (0x048D2952)

#define RFID_FRAME_LENGTH_MAX (256)
#define RFID_FRAME_STATE_OK (1)
#define RFID_FRAME_STATE_NOK (0)

#define RFID_MODUL_TYPE_UNKNOW (0)
#define RFID_MODUL_TYPE_OLD (1)
#define RFID_MODUL_TYPE_NEW (2)

//#define ROAD_NUM_1001 (0x3ABE1FBD)
//#define ROAD_NUM_1002 (0xCA7C00BD)
#define ROAD_NUM_1101 (0x7C349722)//红绿灯路口0x11类型
#define ROAD_NUM_1102 (0x8C139722)
#define ROAD_NUM_1103 (0xAC3D7422)
#define ROAD_NUM_1104 (0x3221EDD3)
#define ROAD_NUM_1105 (0x4C8E9222)
#define ROAD_NUM_1106 (0x0CF59622)
#define ROAD_NUM_1201 (0x12517BD6)
#define ROAD_NUM_1202 (0xE22576D6)

#define ROAD_NUM_2101 (0xDA3BFFBC)//左打死路口
#define ROAD_NUM_2102 (0xCA4900BD)
#define ROAD_NUM_2103 (0xFA8320BD)
#define ROAD_NUM_2201 (0xDA945248)//右打死路口
#define ROAD_NUM_2202 (0x526480D6)
#define ROAD_NUM_2203 (0x223E7AD6)
#define ROAD_NUM_2301 (0x1250DD76)//停车
#define ROAD_NUM_2401 (0x827958D6)//直行
#define ROAD_NUM_2402 (0x72147CD6)
#define ROAD_NUM_3001 (0xC269EAD3)//（桥）正常行驶
#define ROAD_NUM_3002 (0x9CC73424)
#define ROAD_NUM_3003 (0xD23487D6)
#define ROAD_NUM_3004 (0xC26F49D6)
#define ROAD_NUM_3101 (0x6C388622)//（桥）加速
#define ROAD_NUM_3102 (0x5CD07422)
#define ROAD_NUM_3201 (0x82AA86D6)//（桥）减速
#define ROAD_NUM_3202 (0x822875D6)
#define ROAD_NUM_4001 (0xDAA23548)//天少专车
#define ROAD_NUM_4002 (0x5A934B48)

extern const BYTE rfid_cmd_get_id[];
extern const BYTE rfid_cmd_get_data[];
extern const BYTE rfid_cmd_energetic_mode_enable[];
extern const BYTE rfid_cmd_energetic_mode_enable_new[];
extern const BYTE rfid_cmd_energetic_mode_disable[];
extern const BYTE rfid_cmd_energetic_mode_disable_new[];
//extern DWORD rfid_site;


/* 位置数据 */
#ifdef __CONTROL_C_
struct
{
	DWORD site;
	DWORD time;
	DWORD old_site;
	int is_new_site;
} RFID_site_data = { 0x00000000, 0x00000000, 0x00000000, 0 };
#else
extern struct
{
	DWORD site;
	DWORD time;
	DWORD old_site;
	WORD roadnum;
	int is_new_site;
} RFID_site_data;
#endif

extern int rev_RFID_frame(BYTE rev);
extern void explane_RFID_ret_data(const BYTE *data, WORD length);
extern void explane_RFID_ret_cardID(DWORD id);
extern int init_RFID_modul_type(void);
extern WORD RFID_Num_Exp(DWORD site);

#endif
