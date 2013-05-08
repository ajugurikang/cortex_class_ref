#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

/* I2C���� ����ϴ� ��Ʈ ���� */
#define I2C_SW_SDA_PIN		GPIO_Pin_7
#define I2C_SW_SCL_PIN		GPIO_Pin_6
#define I2C_SW_SDA_PORT		GPIOB
#define I2C_SW_SCL_PORT		GPIOB

#define I2C_SW_CLOCK_DELAY	1000

/* ���� inline �Լ� ����  */
static inline void I2C_SW_SetStart(void);
static inline void I2C_SW_SetAddress(uint8_t Addr);
static inline uint8_t I2C_SW_GetAck(void);
static inline void I2C_SW_SetAck(void);
static inline void I2C_SW_SetNAck(void);
static inline void I2C_SW_SetData(uint8_t Data);
static inline uint8_t I2C_SW_GetData(void);
static inline void I2C_SW_SetStop(void);
static inline void I2C_SW_SetSDA_InputMode(void);
static inline void I2C_SW_SetSDA_OutputMode(void);
static inline void I2C_SW_SetSCL_InputMode(void);
static inline void I2C_SW_SetSCL_OutputMode(void);
static inline void I2C_SW_SetSDA_High(void);
static inline void I2C_SW_SetSDA_Low(void);
static inline void I2C_SW_SetSCL_High(void);
static inline void I2C_SW_SetSCL_Low(void);
static inline void I2C_SW_SetBit(uint8_t Bit);
static inline uint8_t I2C_SW_GetBit(void);
static inline void I2C_SW_PortInit(void);


static void I2C_SW_Delay(uint32_t Delay);


void I2C_SW_Init(void)
{
	/* ��Ʈ �ʱ�ȭ */	
	I2C_SW_PortInit();

}

uint8_t I2C_SW_Write(uint8_t Addr, uint8_t *Data, uint8_t Length)
{
	uint8_t i;

	/* ��Ʈ �ʱ�ȭ */	
	I2C_SW_SetSDA_High();
	I2C_SW_SetSCL_High();	
	I2C_SW_SetSDA_OutputMode();
	I2C_SW_SetSCL_OutputMode();

	
	I2C_SW_SetStart();	// ��ŸƮ ��Ʈ 

	/* ��巹���� ������ */
	Addr = Addr & 0xFE;			// Write ��巹���� �����. 
	I2C_SW_SetAddress(Addr);	// ��巹���� ������. 
	if(I2C_SW_GetAck() == 1)	// Ack ��ȣ�� �˻��Ѵ�. 
	{
		I2C_SW_PortInit();
		return 0;		// Ack ��ȣ�� ���� ���� 
	}

	for(i = 0; i < Length; i++)
	{
		I2C_SW_SetData(*Data);
		if(I2C_SW_GetAck() == 1)	// Ack ��ȣ�� �˻��Ѵ�. 
		{
			I2C_SW_PortInit();
			return 0;		// Ack ��ȣ�� ���� ���� 
		}
		Data++;
	}
	/* I2C �� �����Ѵ�. */	
	I2C_SW_SetStop();

	return 1;
}

uint8_t I2C_SW_Read(uint8_t Addr, uint8_t *Data, uint8_t Length)
{
	uint8_t i;

	/* ��Ʈ �ʱ�ȭ */	
	I2C_SW_SetSDA_High();
	I2C_SW_SetSCL_High();	
	I2C_SW_SetSDA_OutputMode();
	I2C_SW_SetSCL_OutputMode();

	I2C_SW_SetStart();	// ��ŸƮ ��Ʈ 

	/* ��巹���� ������ */
	Addr = Addr | 0x01;			// Read ��巹���� �����. 
	I2C_SW_SetAddress(Addr);	// ��巹���� ������. 
	if(I2C_SW_GetAck() == 1)	// Ack ��ȣ�� �˻��Ѵ�. 
	{
		I2C_SW_PortInit();
		return 0;		// Ack ��ȣ�� ���� ���� 
	}

	/* �����͸� �д´� */
	I2C_SW_SetSDA_InputMode();		// SDA�� �Է����� �����. 
	
	for(i = 0; i < (Length - 1); i++)
	{
		*Data = I2C_SW_GetData();
		I2C_SW_SetAck();
		Data++;
	}
	/* ������ �����͸� ���� ������ NACK ��ȣ�� ������. */
	*Data = I2C_SW_GetData();
	I2C_SW_SetNAck();

	/* I2C �� �����Ѵ�. */
	I2C_SW_SetStop();

	return 1;
}



static inline void I2C_SW_SetStart(void)
{
	I2C_SW_SetSDA_High();
	I2C_SW_SetSCL_High();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
	I2C_SW_SetSDA_Low();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
	I2C_SW_SetSCL_Low();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
}

static inline void I2C_SW_SetAddress(uint8_t Addr)
{
	uint8_t i;
	
	for(i = 0; i < 8; i++)
	{
		I2C_SW_SetBit((Addr & (1 << (7 - i)))? 1 : 0);			
	}
}

static inline uint8_t I2C_SW_GetAck(void)
{
	uint8_t InputValue;
	
	I2C_SW_SetSDA_InputMode();		// SDA�� �÷��� ���·� �����. 
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
	I2C_SW_SetSCL_High();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY / 2);
	InputValue = GPIO_ReadInputDataBit(I2C_SW_SDA_PORT, I2C_SW_SDA_PIN);
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY / 2);
	I2C_SW_SetSCL_Low();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
	I2C_SW_SetSDA_OutputMode();		// SDA�� ��� ���·� �����. 	
	return InputValue;
}

static inline void I2C_SW_SetAck(void)
{
	I2C_SW_SetSDA_OutputMode();		// SDA�� ��� ���·� �����. 		
	I2C_SW_SetSDA_Low();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);	
	I2C_SW_SetSCL_High();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
	I2C_SW_SetSCL_Low();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
	I2C_SW_SetSDA_InputMode();		// SDA�� �÷��� ���·� �����. 	
}

static inline void I2C_SW_SetNAck(void)
{
	I2C_SW_SetSDA_OutputMode();		// SDA�� ��� ���·� �����. 		
	I2C_SW_SetSDA_High();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);	
	I2C_SW_SetSCL_High();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
	I2C_SW_SetSCL_Low();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
}


static inline void I2C_SW_SetData(uint8_t Data)
{
	uint8_t i;
	
	for(i = 0; i < 8; i++)
	{
		I2C_SW_SetBit((Data & (1 << (7 - i)))? 1 : 0);			
	}
}

static inline uint8_t I2C_SW_GetData(void)
{
	uint8_t i,RetVal = 0;
	
	for(i = 0; i < 8; i++)
	{
		RetVal <<= 1;
		if(I2C_SW_GetBit())
		{
			RetVal += 1;
		}					
	}
	return RetVal;
}

static inline void I2C_SW_SetStop(void)
{
	I2C_SW_SetSDA_Low();
	I2C_SW_SetSCL_Low();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
	I2C_SW_SetSCL_High();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY / 4);
	I2C_SW_SetSDA_High();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
}


static inline void I2C_SW_SetSDA_InputMode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = I2C_SW_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(I2C_SW_SDA_PORT, &GPIO_InitStructure);
}

static inline void I2C_SW_SetSDA_OutputMode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = I2C_SW_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(I2C_SW_SDA_PORT, &GPIO_InitStructure);
}

static inline void I2C_SW_SetSCL_InputMode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = I2C_SW_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(I2C_SW_SCL_PORT, &GPIO_InitStructure);
}


static inline void I2C_SW_SetSCL_OutputMode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = I2C_SW_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(I2C_SW_SCL_PORT, &GPIO_InitStructure);
}

static inline void I2C_SW_SetSDA_High(void)
{
    GPIO_SetBits(I2C_SW_SDA_PORT, I2C_SW_SDA_PIN);
}

static inline void I2C_SW_SetSDA_Low(void)
{
    GPIO_ResetBits(I2C_SW_SDA_PORT, I2C_SW_SDA_PIN);
}

static inline void I2C_SW_SetSCL_High(void)
{
    GPIO_SetBits(I2C_SW_SCL_PORT, I2C_SW_SCL_PIN);
}

static inline void I2C_SW_SetSCL_Low(void)
{
    GPIO_ResetBits(I2C_SW_SCL_PORT, I2C_SW_SCL_PIN);
}

static inline void I2C_SW_SetBit(uint8_t Bit)
{
	if(Bit)
	{
		I2C_SW_SetSDA_High();
		I2C_SW_Delay(I2C_SW_CLOCK_DELAY);	
		I2C_SW_SetSCL_High();
		I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
		I2C_SW_SetSCL_Low();
		I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
	}
	else
	{
		I2C_SW_SetSDA_Low();
		I2C_SW_Delay(I2C_SW_CLOCK_DELAY);	
		I2C_SW_SetSCL_High();
		I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
		I2C_SW_SetSCL_Low();
		I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
	}
}

static inline uint8_t I2C_SW_GetBit(void)
{
	uint8_t InputValue;
	
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);
	I2C_SW_SetSCL_High();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY / 2);
	InputValue = GPIO_ReadInputDataBit(I2C_SW_SDA_PORT, I2C_SW_SDA_PIN);
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY / 2);
	I2C_SW_SetSCL_Low();
	I2C_SW_Delay(I2C_SW_CLOCK_DELAY);

	return InputValue;
}



static inline void I2C_SW_PortInit(void)
{
	/* ��Ʈ �ʱ�ȭ */	
	I2C_SW_SetSDA_InputMode();
	I2C_SW_SetSCL_InputMode();
}


static void I2C_SW_Delay(uint32_t Delay)
{
	for( ;Delay !=0; Delay--);
}



