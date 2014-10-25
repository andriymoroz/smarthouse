#ifndef NRF_MESSAGE_H
#define NRF_MESSAGE_H


#define MAX_NODE_NAME_LEN 20

typedef uint8_t	SensorId;

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
		char name[MAX_NODE_NAME_LEN+1];
	} id;
} NrfMessage;

typedef union nrf_messagev2
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
		char name[MAX_NODE_NAME_LEN+1];
	} id;
} SensorData;

typedef struct NrfMessageV2
{
	NrfMessageV2(uint8_t _id = 0) {id = _id;}
	SensorId	id;
	SensorData	data;
} NrfMessageV2;



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
/*
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
*/
#if 5/*sizeof(NrfMessage)*/ > 32
#error "NrfMessage too long!"
#endif

#endif // NRF_MESSAGE_H
