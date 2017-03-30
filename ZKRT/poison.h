#ifndef __POISON_H__
#define __POISON_H__
#include "stm32f0xx.h"

#define TRUE 1
#define FALSE 0
#define GAS_TYPE_NUM 25
#define makeword(a,b) (a<<8) | b
#define Bit7  7
#define Bit6	6
#define Bit5  5


extern uint32_t sensor_type;
extern uint8_t ControlType;/*7->7Bit 6->6Bit 5->5Bit*/
enum GAS_TYPE {
      CO=0x01, H2S, NH3, CO2, N02, 
      NO, SO2, CL2, O2, H2, 
      PH3, CH2O, O3, F2, HF, 
      HCL, HBR, C2H4O, COCL2, SIH4, 
      CLO2, SF6, TVOC, CH4
};

//#define CO_ADDR 0X01
//#define H2S_ADDR 0X02
//#define O2_ADDR 0X03
//#define EX_ADDR 0X04
//#define N02_ADDR 0X05
//#define NO_ADDR 0X06
//#define SO2_ADDR 0X07
//#define CL2_ADDR 0X08
//#define NH3_ADDR 0X09
//#define H2_ADDR 0X0A
//#define PH3_ADDR 0X0B
//#define CH2O_ADDR 0X0C
//#define O3_ADDR 0X0D
//#define F2_ADDR 0X0E
//#define HF_ADDR 0X0F
//#define HCL_ADDR 0X10
//#define HBR_ADDR 0X11
//#define C2H4O_ADDR 0X12
//#define COCL2_ADDR 0X13
//#define SIH4_ADDR 0X14
//#define CLO2_ADDR 0X15
//#define CO2_ADDR 0X16
//#define SF6_ADDR 0X17
//#define TVOC_ADDR 0X18
//#define CH4_ADDR 0X19


uint8_t ReadModel (uint8_t address, uint16_t *model);//
uint8_t ReadType (uint8_t address, uint8_t *model);
uint8_t ReadMeaRange (uint8_t address, uint16_t *range);
uint8_t ReadUint (uint8_t address, uint8_t *uint);
uint8_t ReadDecimal (uint8_t address, uint8_t *decimal);
uint8_t ReadMeaVal (uint8_t address, uint16_t *meaval);
uint8_t CalibraZero (uint8_t address);
uint8_t CalibraSpan (uint8_t address,uint16_t val);
uint8_t CalibraReset(uint8_t address,uint16_t val);
uint8_t ChangeAddr(uint8_t address,uint8_t target_addr);
void CheckSensorType(uint32_t *sen_type);
void ReadAllVal(uint32_t sen_type,uint16_t *buff);

#endif


