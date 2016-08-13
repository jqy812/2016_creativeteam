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
	int is_new_site;
} RFID_site_data;
#endif

extern int rev_RFID_frame(BYTE rev);
extern void explane_RFID_ret_data(const BYTE *data, WORD length);
extern void explane_RFID_ret_cardID(DWORD id);
extern int init_RFID_modul_type(void);

#endif
