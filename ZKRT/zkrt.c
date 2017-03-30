#include "zkrt.h"

uint8_t zkrt_tx_seq[DEVICE_NUMBER];               		//字节6，帧序列号，为了区分，每个操作需要1个专门的序列号
uint8_t now_uav_type = 0;
uint16_t now_uav_num = 0;

uint8_t cunzai[5]={0xFE,0xFE,0xFE,0xFE,0xFE};
extern uint8_t addrdata[5];
extern uint16_t sen_val_buff[4];

/**
 * @brief Accumulate the X.25 CRC by adding one char at a time.
 *
 * The checksum function adds the hash of one char at a time to the
 * 16 bit checksum (uint16_t).
 *
 * @param data new char to hash
 * @param crcAccum the already accumulated checksum

 **/
void crc_accumulate(uint8_t data, uint16_t *crcAccum)
{
	/*Accumulate one byte of data into the CRC*/
	uint8_t tmp;

	tmp = data ^ (uint8_t)(*crcAccum &0xff);
	tmp ^= (tmp<<4);
	*crcAccum = (*crcAccum>>8) ^ (tmp<<8) ^ (tmp <<3) ^ (tmp>>4);
}


/**
 * @brief Accumulate the X.25 CRC by adding an array of bytes
 *
 * The checksum function adds the hash of one char at a time to the
 * 16 bit checksum (uint16_t).
 *
 * @param data new bytes to hash
 * @param crcAccum the already accumulated checksum
 **/
void crc_accumulate_buffer(uint16_t *crcAccum, const char *pBuffer, uint16_t length)
{
	const uint8_t *p = (const uint8_t *)pBuffer;
	
	while (length--)
	{
		crc_accumulate(*p++, crcAccum);
	}
}



/**
 * @brief Calculates the X.25 checksum on a byte buffer
 *
 * @param  pBuffer buffer containing the byte array to hash
 * @param  length  length of the byte array
 * @return the checksum over the buffer bytes
 **/
uint16_t crc_calculate(const uint8_t* pBuffer, uint16_t length)
{
	uint16_t crcTmp = 0XFFFF;
        
	while (length--) 
	{
		crc_accumulate(*pBuffer++, &crcTmp);
	}
	return crcTmp;
}


void zkrt_final_encode(zkrt_packet_t *packet)
{
	uint8_t i;
	
	packet->start_code = _START_CODE;                             //字节0，帧起始码，0XEB
	packet->ver = _VERSION;                                       //字节1，协议版本
	packet->session_ack = 0X00;                                   //字节2，无需应答，数据帧
	packet->padding_enc = 0X00;                                   //字节3，不加密
	packet->length = _LENGTH;                                     //字节5，一共30位
	packet->seq = zkrt_tx_seq[packet->UAVID[3]];                  //字节6，帧序列号
	zkrt_tx_seq[packet->UAVID[3]]++;
	zkrt_tx_seq[packet->UAVID[3]]%=255;
	for(i=0;i<3;i++)
	packet->APPID[i]= 0x00;                                       //字节7-9，服务器ID，地面站
	packet->crc = crc_calculate(((const uint8_t*)(packet)),47); //字47-48，校验位
	packet->end_code = _END_CODE;                                 //字49，结束符
}




/*对单个字节的crc更新校验*/
void zkrt_update_checksum(zkrt_packet_t* packet, uint8_t ch)
{
	uint16_t crc = packet->crc;
	
	crc_accumulate(ch,&(crc));
	
	packet->crc = crc;
}


uint8_t zkrt_curser_state = 0;
uint8_t zkrt_recv_success = 0;
uint8_t zkrt_app_index = 0;
uint8_t zkrt_uav_index = 0;
uint8_t zkrt_dat_index = 0;
uint8_t zkrt_decode_char(zkrt_packet_t *packet, uint8_t ch)
{
	zkrt_recv_success = 0;
	
	if ((zkrt_curser_state == 0)&&(ch == _START_CODE))		//字节0，得到起始码
	{
		packet->start_code = ch;
		packet->crc = 0XFFFF;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 1;
	}
	else if ((zkrt_curser_state == 1)&&(ch == _VERSION))	//字节1，得到版本号
	{
		packet->ver = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 2;
	}
	else if (zkrt_curser_state == 2)											//字节2，得到session
	{
		packet->session_ack = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 3;
	}
	else if (zkrt_curser_state == 3)											//字节3，得到padding
	{
		packet->padding_enc = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 4;
	}
	else if (zkrt_curser_state == 4)											//字节4，得到cmd
	{
		packet->cmd = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 5;
	}
	else if ((zkrt_curser_state == 5)&&(ch == _LENGTH))		//字节5，得到长度
	{
		packet->length = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 6;
	}
	else if (zkrt_curser_state == 6)											//字节6，接收序列号
	{
		packet->seq = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 7;
	}
	else if (zkrt_curser_state == 7)											//字节7-9，接收APPID
	{
		packet->APPID[zkrt_app_index] = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_app_index++;
		if (zkrt_app_index == 3)
		{
			zkrt_app_index = 0;
			zkrt_curser_state = 8;
		}
	}
	else if (zkrt_curser_state == 8)											//字节10-15，接收UAVID
	{
		packet->UAVID[zkrt_uav_index] = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_uav_index++;
		if (zkrt_uav_index == 6)
		{
			zkrt_uav_index = 0;
			zkrt_curser_state = 9;
		}
	}
	else if (zkrt_curser_state == 9)											//字节16，得到command
	{
		packet->command = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 10;
	}
	else if (zkrt_curser_state == 10)											//字节17-46，接收DATA
	{
		packet->data[zkrt_dat_index] = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_dat_index++;
		if (zkrt_dat_index == 30)
		{
			zkrt_dat_index = 0;
			zkrt_curser_state = 11;
		}
	}
	else if ((zkrt_curser_state == 11)&&(ch == (uint8_t)((packet->crc)&0xff)))	//字节47，CRC1
	{
		zkrt_curser_state = 12;
	}
	else if ((zkrt_curser_state == 12)&&(ch == (uint8_t)((packet->crc)>>8)))		//字节48，CRC2
	{
		zkrt_curser_state = 13;
	}
	else if ((zkrt_curser_state == 13)&&(ch == _END_CODE))											//字节49，结尾
	{
		packet->end_code = ch;
		zkrt_curser_state = 0;
		zkrt_recv_success = 1;
	}
	else
	{
		zkrt_curser_state = 0;
		zkrt_app_index = 0;
		zkrt_uav_index = 0;
		zkrt_dat_index = 0;
	}
	
	return zkrt_recv_success;
}

//对于特定的毒气值进行封包，返回封包结果
void zkrt_read_posion_ack(void)
{
	uint8_t i;
	zkrt_packet_t packet;
	
	packet.cmd = UAV_TO_APP;
	packet.command= DEFAULT_NUM;
	
	packet.UAVID[0] = now_uav_type;
	packet.UAVID[1] = (uint8_t)(now_uav_num&0xff);
	packet.UAVID[2] = (uint8_t)(now_uav_num>>8);
	packet.UAVID[3] = DEVICE_TYPE_GAS;
	packet.UAVID[4] = DEFAULT_NUM;
	packet.UAVID[5] = DEFAULT_NUM;

	//sen_val_buff
	
	for(i=1;i<5;i++)
	{
		if(addrdata[i]==0xEE)
		{
			cunzai[i]=0xFD;
		}
	}
	packet.data[0]=cunzai[1];	
	packet.data[1] = addrdata[1];
	packet.data[2] = (uint8_t)(sen_val_buff[0]&0xff);
	packet.data[3] = (uint8_t)(sen_val_buff[0]>>8);
	packet.data[4]=cunzai[2];	
	packet.data[5] = addrdata[2];
	packet.data[6] = (uint8_t)(sen_val_buff[1]&0xff);
	packet.data[7] = (uint8_t)(sen_val_buff[1]>>8);
	packet.data[8]=cunzai[3];	
	packet.data[9] = addrdata[3];
	packet.data[10] = (uint8_t)(sen_val_buff[2]&0xff);
	packet.data[11] = (uint8_t)(sen_val_buff[2]>>8);
	packet.data[12]=cunzai[4];	
	packet.data[13] = addrdata[4];
	packet.data[14] = (uint8_t)(sen_val_buff[3]&0xff);
	packet.data[15] = (uint8_t)(sen_val_buff[3]>>8);

	for(i=16;i<30;i++)
	{
		packet.data[i] = 0;
	}
	
	zkrt_final_encode(&packet);
	
	CAN1_send_message_fun((uint8_t *)(&packet),_TOTAL_LEN);
}


