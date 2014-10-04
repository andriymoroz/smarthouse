#ifndef NRF_MESSAGE_H
#define NRF_MESSAGE_H

typedef union nrf_message
{
	uint32_t ping;
	uint32_t address;
	uint32_t status;
	struct {
		float temperature;
		float humidity;
	} dht;
	struct {
		float temperature;
	} ds;
	struct {
		char name[10];
	} id;
} NrfMessage;


typedef enum message_type_e
{
	MsgType_Void = 0,
	MsgType_Ping,
	MsgType_ReqAddress,
	MsgType_SetAddress,
	MsgType_ReqStatus,
	MsgType_Status,
	MsgType_TempHum,
	MsgType_Temp,
	MsgType_ID,


	MSG_TYPE_NUM
} MsgType;

typedef enum network_message_subtype_e
{
	NetMsgSubtype_Void = 0,
	NetMsgSubtype_Ping,
	NetMsgSubtype_ReqAddress,
	NetMsgSubtype_SetAddress,
	//NetMsgSubtype_

	NET_MSG_SUBTYPE_NUM
} NetMsgSubtype;

typedef enum data_message_subtype_e
{
	DataMsgSubtype_Void = 0,
	DataMsgSubtype_Temp,
	DataMsgSubtype_State,
	DataMsgSubtype_Humidity,
	DataMsgSubtype_Status,
	DataMsgSubtype_ReqData,

	DATA_MSG_SUBTYPE_NUM
} DataMsgSubtype;

//typedef struct {
//	MsgType msgtype;
//	int msgsubtype;
//	uint8_t data[4];
//} NrfMessage;

#if 5/*sizeof(NrfMessage)*/ > 32
#error "NrfMessage too long!"
#endif

#endif // NRF_MESSAGE_H
