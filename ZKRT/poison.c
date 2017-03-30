#include "poison.h"
#include "stm32f0xx.h"
#include "rs485.h"
#include "string.h"
#include "delay.h"

#define MAX_CMD_LEN 7
#define MAX_REC_LEN 7
#define NO_OF_SENSOR  4

uint8_t cmd_data [MAX_CMD_LEN];
uint8_t rec_data [MAX_REC_LEN];

uint8_t meaVal_data [NO_OF_SENSOR][MAX_REC_LEN];

uint8_t addrdata[5]={0};
uint32_t sensor_type=0;//32λ��־λ

uint8_t ControlType=Bit7;/*7->7Bit 6->6Bit 5->5Bit*/


/**
  * @brief  ����У����
  * @param  pSendBuf ����ָ�� ��nEnd ���鳤��
  * @retval ����16λУ��ֵ
  */
unsigned int GetCheck (unsigned char *pSendBuf, unsigned char nEnd )
{
   unsigned char i,j;
   unsigned int wCrc = (unsigned int)(0xffff);
   for (i = 0; i<nEnd; i++)
   {
      wCrc ^= (unsigned int)(pSendBuf[i]);
      for(j=0; j<8; j++ )
      {
         if (wCrc & 0x01)
         {
            wCrc >>= 1;
            wCrc ^= 0xA001;
         }
         else wCrc >>=1;
      }
   }
   return wCrc;
}
/**
  * @brief  ��ȡ�ͺ�
  * @param  address �ӻ���ַ,model �ͺ�ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
uint8_t ReadModel (uint8_t address, uint16_t *model)
{
   uint16_t check = 0;
   uint8_t i = 0;
   delay_xms (7);            //��������֮֡����3.5ms���ϣ����� 5ms
   cmd_data[i++] = address;   //��ַ
   cmd_data[i++] = 0X01;     //����
   cmd_data[i++] = 0X00;     //���ݳ���
   check = GetCheck(cmd_data, 3);
   cmd_data[i++] = check & 0XFF;     //У���λ
   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
   RS485_Send_Data(cmd_data,i);
   
   RS485_Receive_Data(rec_data, 7);
   check = GetCheck(rec_data, 5);
   if((check&0xff) == rec_data[5] && ((check>>8)&0XFF) ==rec_data[6]){
      *model=makeword(rec_data[4],rec_data[3]);;
      return TRUE;}
   else return FALSE;
}

/**
  * @brief  ��ȡ��������
  * @param  address �ӻ���ַ,type ����ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
uint8_t ReadType (uint8_t address, uint8_t *type)
{
   uint16_t check = 0;
   uint8_t i = 0;
   delay_xms (7);            //��������֮֡����3.5ms���ϣ����� 5ms
   cmd_data[i++] = address;   //��ַ
   cmd_data[i++] = 0X04;     //����
   cmd_data[i++] = 0X00;     //���ݳ���
   check = GetCheck(cmd_data, 3);
   cmd_data[i++] = check & 0XFF;     //У���λ
   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
   RS485_Send_Data(cmd_data,i);
   ControlType = Bit6;
	 delay_xms (50);            //��������֮֡����3.5ms���ϣ����� 5ms
   RS485_Receive_Data(rec_data, ControlType);
   check = GetCheck(rec_data, 4);
   if((check&0xff) == rec_data[4] && ((check>>8)&0XFF) ==rec_data[5])
   {
        *type=rec_data[3];
      return TRUE;
   }
   else return FALSE;
}

/**
  * @brief  ��ȡ����
  * @param  address �ӻ���ַ,range ������ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
uint8_t ReadMeaRange (uint8_t address, uint16_t *range)
{
   uint16_t check = 0;
   uint8_t i = 0;
   delay_xms (7);            //��������֮֡����3.5ms���ϣ����� 5ms
   cmd_data[i++] = address;   //��ַ
   cmd_data[i++] = 0X05;     //����
   cmd_data[i++] = 0X00;     //���ݳ���
   check = GetCheck(cmd_data, 3);
   cmd_data[i++] = check & 0XFF;     //У���λ
   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
   RS485_Send_Data(cmd_data,i);
   
   RS485_Receive_Data(rec_data, 7);
   check = GetCheck(rec_data, 5);
   if((check&0xff) == rec_data[5] && ((check>>8)&0XFF) ==rec_data[6])
   {
      *range=makeword(rec_data[4],rec_data[3]);
      return TRUE;
   }
   else return FALSE;
}

/**
  * @brief  ��ȡ��λ
  * @param  address �ӻ���ַ,uint ��λ��ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
uint8_t ReadUint (uint8_t address, uint8_t *uint)
{
   uint16_t check = 0;
   uint8_t i = 0;
   delay_xms (7);            //��������֮֡����3.5ms���ϣ����� 5ms
   cmd_data[i++] = address;   //��ַ
   cmd_data[i++] = 0X06;     //����
   cmd_data[i++] = 0X00;     //���ݳ���
   check = GetCheck(cmd_data, 3);
   cmd_data[i++] = check & 0XFF;     //У���λ
   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
   RS485_Send_Data(cmd_data,i);
   
   RS485_Receive_Data(rec_data, 6);
   check = GetCheck(rec_data, 4);
   if((check&0xff) == rec_data[4] && ((check>>8)&0XFF) ==rec_data[5])
   {
      *uint=rec_data[3];
      return TRUE;
   }
   else return FALSE;
}

/**
  * @brief  ��ȡС��
  * @param  address �ӻ���ַ,decimal С����ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
uint8_t ReadDecimal (uint8_t address, uint8_t *decimal)
{
   uint16_t check = 0;
   uint8_t i = 0;
   delay_xms (7);            //��������֮֡����3.5ms���ϣ����� 5ms
   cmd_data[i++] = address;   //��ַ
   cmd_data[i++] = 0X07;     //����
   cmd_data[i++] = 0X00;     //���ݳ���
   check = GetCheck(cmd_data, 3);
   cmd_data[i++] = check & 0XFF;     //У���λ
   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
   RS485_Send_Data(cmd_data,i);
   ControlType = Bit6;
	 delay_xms (50);            //��������֮֡����3.5ms���ϣ����� 5ms
   RS485_Receive_Data(rec_data, ControlType);
   check = GetCheck(rec_data, 4);
   if((check&0xff) == rec_data[4] && ((check>>8)&0XFF) ==rec_data[5])
   {
      *decimal=rec_data[3];
      return TRUE;
   }
   else return FALSE;
}

/**
  * @brief  ��ȡ����ֵ
  * @param  address �ӻ���ַ,value������ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
uint8_t ReadMeaVal (uint8_t address, uint16_t *meaval)
{
 //  static uint8_t temp=1;
		uint8_t k;
   uint16_t check = 0;
   uint8_t i = 0;
   delay_xms (7);            //��������֮֡����3.5ms���ϣ����� 5ms
   cmd_data[i++] = address;   //��ַ
   cmd_data[i++] = 0X20;     //����
   cmd_data[i++] = 0X00;     //���ݳ���
   check = GetCheck(cmd_data, 3);
   cmd_data[i++] = check & 0XFF;     //У���λ
   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
   RS485_Send_Data(cmd_data,i);
   ControlType = Bit7;
	 delay_xms (50);            //��������֮֡����3.5ms���ϣ����� 5ms
   RS485_Receive_Data(meaVal_data[address], ControlType);
	 for(k=0;k<7;k++)
	 {
		 rec_data[k] = meaVal_data[address][k];
	 }
   check = GetCheck(rec_data, 5);
   if((check&0xff) == rec_data[5] && ((check>>8)&0XFF) ==rec_data[6])
   {
      *meaval=makeword(rec_data[4],rec_data[3]);
      return TRUE;
   }
   else return FALSE;
}

/**
  * @brief  У��zero
  * @param  address �ӻ���ַ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
uint8_t CalibraZero (uint8_t address)
{
   uint16_t check = 0;
   uint8_t i = 0;
   delay_xms (7);            //��������֮֡����3.5ms���ϣ����� 5ms
   cmd_data[i++] = address;   //��ַ
   cmd_data[i++] = 0X38;     //����
   cmd_data[i++] = 0X00;     //���ݳ���
   check = GetCheck(cmd_data, 3);
   cmd_data[i++] = check & 0XFF;     //У���λ
   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
   RS485_Send_Data(cmd_data,i);
   
   RS485_Receive_Data(rec_data, 5);
   check = GetCheck(rec_data, 3);
   if((check&0xff) == rec_data[3] && ((check>>8)&0XFF) ==rec_data[4]) return TRUE;
   else return FALSE;
}

/**
  * @brief  У��span
  * @param  address �ӻ���ַ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
uint8_t CalibraSpan (uint8_t address,uint16_t val)
{
   uint16_t check = 0;
   uint8_t i = 0;
   delay_xms (7);            //��������֮֡����3.5ms���ϣ����� 5ms
   cmd_data[i++] = address;   //��ַ
   cmd_data[i++] = 0X39;     //����
   cmd_data[i++] = 0X02;     //���ݳ���
   cmd_data[i++] = val & 0XFF;
   cmd_data[i++] = (val>>8) & 0XFF;
   check = GetCheck(cmd_data, 5);
   cmd_data[i++] = check & 0XFF;     //У���λ
   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
   RS485_Send_Data(cmd_data,i);
   
   RS485_Receive_Data(rec_data, 5);
   check = GetCheck(rec_data, 3);
   if((check&0xff) == rec_data[3] && ((check>>8)&0XFF) ==rec_data[4]) return TRUE;
   else return FALSE;
}

/**
  * @brief  У���궨
  * @param  address �ӻ���ַ���궨��ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
uint8_t CalibraReset(uint8_t address,uint16_t val)
{
   uint16_t check = 0;
   uint8_t i = 0;
   delay_xms (7);            //��������֮֡����3.5ms���ϣ����� 5ms
   cmd_data[i++] = address;   //��ַ
   cmd_data[i++] = 0X3E;     //����
   cmd_data[i++] = 0X02;     //���ݳ���
   cmd_data[i++] = val & 0XFF;
   cmd_data[i++] = (val>>8) & 0XFF;
   check = GetCheck(cmd_data, 5);
   cmd_data[i++] = check & 0XFF;     //У���λ
   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
   RS485_Send_Data(cmd_data,i);
   
   RS485_Receive_Data(rec_data, 6);
   check = GetCheck(rec_data, 4);
   if((check&0xff) == rec_data[4] && ((check>>8)&0XFF) ==rec_data[5]) return TRUE;
   else return FALSE;
}

/**
  * @brief  �޸Ĵ�������ַ
  * @param  address ԭ�ӻ���ַ��target_addrĿ���ַ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
uint8_t ChangeAddr(uint8_t address,uint8_t target_addr)
{
   uint16_t check = 0;
   uint8_t i = 0;
   delay_xms (7);            //��������֮֡����3.5ms���ϣ����� 5ms
   cmd_data[i++] = address;   //��ַ
   cmd_data[i++] = 0X10;     //����
   cmd_data[i++] = 0X01;     //���ݳ���
   cmd_data[i++] = target_addr;
   check = GetCheck(cmd_data, 4);
   cmd_data[i++] = check & 0XFF;     //У���λ
   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
   RS485_Send_Data(cmd_data,i);
   
   RS485_Receive_Data(rec_data, 5);
   check = GetCheck(rec_data, 3);
   if((check&0xff) == rec_data[3] && ((check>>8)&0XFF) ==rec_data[4]) return TRUE;
   else return FALSE;
}

/**
  * @brief ������ɴ���������
  * @param  sen_type ���������ͱ�־ 
  * @retval 
  */
void CheckSensorType(uint32_t *sen_type)
{
   uint8_t addr;
   uint32_t sen_temp=0;
   uint8_t type=0;
   delay_xms(10);
   for(addr=1; addr<=4; addr++)
   {
      if(ReadType(addr, &type)) 
      {
         sen_temp = (sen_temp) | (0x00000001 << addr);
      }
      delay_xms(10);
   }
   *sen_type = sen_temp;
}

uint8_t _i;
void ReadAllVal(uint32_t sen_type,uint16_t *buff)
{
   uint8_t j = 0;
	//uint8_t decimal[6];
   memset(buff,0X00,8);              //���ֽڴ���
   for(_i=1; _i<=4; _i++)
   {
      if((0x00000001 << _i) & sensor_type)
      {
				addrdata[_i]=_i;
				//ReadDecimal(_i,&decimal[j]);
        ReadMeaVal(_i, &buff[j++]);
      }
			else
			{
				addrdata[_i]=0xee;
				j++;
			}
   }
}

