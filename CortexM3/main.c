/**
* @file			main.c
* @brief		메인 함수 
* @author		강이석	
* @remarks
* @par 수정이력 
*	@li	2012.12.07 : 초기  함수 작성  
*/


/**
* @mainpage		Cortex-M3 제어 참조 코드 (STM32F103ZET)
* @section intro	소개
*	@li	소개 :  Cortex-M3 입문 수강생용 참조 코드 
* @section CREATEINFO 작성정보 	
*	@li	개발자명 : 강이석 
* 	@li	개발 보드: Solutionbank Cortex-M3 V1.0  
*/

#include "system.h"
#include "led.h"
#include "fnd.h"
#include "extif.h"
#include "serial.h"
#include "timer.h"
#include "rtc.h"
#include "sensor.h"
#include "key.h"
#include "sdcard.h"
#include "stm32f10x_sdio.h"
#include "lcd.h"
#include "touch.h"
#include "i2c_sw.h"
#include "ff.h"



//=====  드라이버  콜백 처리 함수 =====
static void SysTick_Process(void);
static void Serial1_Process(uint8_t Data);

static void Seonsor_Scan(void);
static void Timer_200msProcess(void);
static void Timer_1sProcess(void);
static void Touch_Process(uint16_t X, uint16_t Y);

static void Init(void);
// SD 카드 관련 함수 
static void SDCard_Init(void);
static void SDCard_App(void);

//=====  Private typedef  =====
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

//=====  Private define  =====
#define BlockSize            512 /* Block Size in Bytes */
#define BufferWordsSize      (BlockSize >> 2)
#define NumberOfBlocks       2  /* For Multi Blocks operation (Read/Write) */
#define MultiBufferWordsSize ((BlockSize * NumberOfBlocks) >> 2)


//=====  Private variables  =====
SD_CardInfo SDCardInfo;
uint32_t Buffer_Block_Tx[BufferWordsSize], Buffer_Block_Rx[BufferWordsSize];
uint32_t Buffer_MultiBlock_Tx[MultiBufferWordsSize], Buffer_MultiBlock_Rx[MultiBufferWordsSize];
volatile TestStatus EraseStatus = FAILED, TransferStatus1 = FAILED, TransferStatus2 = FAILED;
SD_Error Status = SD_OK;

//=====  Private functions  =====
FATFS fs;				// Work area (file system object) for logical drive
FIL fsrc, fdst;			// file objects
BYTE buffer[512];		// file copy buffer
FRESULT res;			// FatFs function common result code
UINT br, bw;			// File R/W count



/**
* @fn		int main(void)
* @brief
*	- 메인 함수 
* @remarks
* @param	void
* @return	int			
*/
int main()
{

	Init();		// 시스템 초기화 

	// SD Card 테스트 
	SDCard_App();

 	while(1)
	{
    
    } 
//  return 0;
}


/**
* @fn		static void Init(void)
* @brief
*	@li		시스템 초기화 함수 
* @remarks
* @param	void
* @return	void			
*/
static void Init(void)
{
	
	//=====  시스템 초기화 =====
	// 클럭 초기화
	System_ClockConfig();
	// GPIO 초기화
	System_GPIOConfig();
	// 인터럽트 초기화
	System_InterruptConfig();
	// SysTick 초기화 
	System_SysTickConfig(100, SysTick_Process);	// SysTick 주기를 100ms로 설정. 


	//=====  드라이버 초기화  =====
	ExtIF_Init();
	LED_Init();
	FND_Init();
	Timer_Init();
	RTC_Init();
	Serial_Open(SERIAL_PORT1,115200, Serial1_Process);
	Serial_Open(SERIAL_PORT2,115200, (void *)0);
	Sensor_Init();
	Key_Init();
	SDCard_Init();
	LCD_Init();
//	I2C_SW_Init();
//	Touch_Init();
}


static void SDCard_Init(void)
{
	SD_Error Status = SD_OK;

	// SD Init를 초기화 한다.
	Status = SD_Init();
	
	// SD 카드 정보를 읽어 온다. 
	if (Status == SD_OK)
	{
		// CSD/CID MSD 레지스터 정보를 읽어온다. 
		Status = SD_GetCardInfo(&SDCardInfo);
	}
	
	// SD 카드를 선택한다. 
	if (Status == SD_OK)
	{
		// Select Card
		Status = SD_SelectDeselect((uint32_t) (SDCardInfo.RCA << 16));
	}

	// SD 버스폭 결정 	  
	if (Status == SD_OK)
	{
		// Status = SD_EnableWideBusOperation(SDIO_BusWide_4b);	// ajuguri, 아직 4선 방식은 구현이 안됨 .
	}
	  
	// 정송 모드 결정 
	if (Status == SD_OK)
	{  
		// Status = SD_SetDeviceMode(SD_DMA_MODE);
		// Status = SD_SetDeviceMode(SD_POLLING_MODE);
		Status = SD_SetDeviceMode(SD_INTERRUPT_MODE);
	}

}

static void SDCard_App(void)
{
	uint16_t a;
	FRESULT res;
	FILINFO finfo;
	DIR dirs;
	int i;
	char *fn;
	char path[50]={""};  
	char name[]={"test.txt"};
		
	  
//	disk_initialize(0);
		
	f_mount(0, &fs);

	// 파일 읽기
	if(f_open(&fsrc,"test.txt", FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		Serial_WriteString(0, "\r\ntest.txt Opened\r\n");
		// 파일을 읽어서 출력한다. 
		for (;;) 
		{
			for(a=0; a<512; a++) buffer[a]=0; 
			res = f_read(&fsrc, buffer, sizeof(buffer), &br);
			Serial_WriteString(0,buffer);	

			if (res || br == 0) break;
		}			
	}
	else
	{
		Serial_WriteString(0, "Fail to Opent test.txt\r\n");
	}
	
	if (f_opendir(&dirs, path) == FR_OK) 
	{
		while (f_readdir(&dirs, &finfo) == FR_OK)  
		{
			if (finfo.fattrib & AM_ARC) 
			{
				if(!finfo.fname[0]) 
			  		break;		 
				Serial_WriteString(0,finfo.fname);
				res = f_open(&fsrc, finfo.fname, FA_OPEN_EXISTING | FA_READ);
				//res = f_open(&fdst, "a3.rar", FA_CREATE_ALWAYS | FA_WRITE);
				br=1;
				a=0;
				for (;;) 
				{
					for(a=0; a<512; a++) buffer[a]=0; 
					res = f_read(&fsrc, buffer, sizeof(buffer), &br);
					Serial_WriteString(0,buffer);	
					//printp("\r\n@@@@@res=%2d	br=%6d	bw=%6d",res,br,bw);
					if (res || br == 0) break;	 // error or eof
					//res = f_write(&fdst, buffer, br, &bw);
					//printp("\r\n$$$$$res=%2d	br=%6d	bw=%6d",res,br,bw);
					//if (res || bw < br) break;   // error or disk full	
				}
				f_close(&fsrc);
				//f_close(&fdst);	
				//printf("\r\n asdasdasdasdasdasdas");	
				//break;						  
			}
		} 
		
	}
	  
//	while(1);
}






//===== 드라이버 콜백 처리 함수 =====
static void SysTick_Process(void)
{
	static uint16_t Time = 0;
	
	Time++;
	
	FND_Write((Time) % 10, 0, 0);
	FND_Write((Time / 10) % 10, 1, 1);
	FND_Write((Time / 100) % 10, 2, 0);
	FND_Write((Time / 1000) % 10, 3, 0);
}

static void Serial1_Process(uint8_t Data)
{



}





