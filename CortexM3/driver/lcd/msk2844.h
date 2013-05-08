#ifndef _MSK_2844_H_
#define _MSK_2844_H_


// Display Control System
#define MSK_IX_START_OSC					0x000
#define MSK_IX_DEVICE_OUTPUT_CONTROL		0x001
#define MSK_IX_LCD_ALT_CONTROL				0x002
#define MSK_IX_ENTRY_MODE					0x003
#define MSK_IX_RESIZING_CONTROL1			0x004
#define MSK_IX_RESIZING_CONTROL2			0x005
#define MSK_IX_DISPLAY_CONTROL1				0x007
#define MSK_IX_DISPLAY_CONTROL2				0x008
#define MSK_IX_DISPLAY_CONTROL3				0x009
#define MSK_IX_DISPLAY_CONTROL4				0x00B
#define MSK_IX_EXT_DISPLAY_IF_CONTROL1		0x00C
#define MSK_IX_FRAME_CYCLE_ADJ_CONTROL		0x00D
#define MSK_IX_EXT_DISPLAY_IF_CONTROL2		0x00E
#define MSK_IX_EXT_DISPLAY_IF_CONTROL3		0x00F
#define MSK_IX_GATE_LTPS_IF_CONTROL1		0x010
#define MSK_IX_GATE_LTPS_IF_CONTROL2		0x011
#define MSK_IX_GATE_LTPS_IF_CONTROL3		0x012
#define MSK_IX_GATE_LTPS_IF_CONTROL4		0x013
#define MSK_IX_GATE_LTPS_IF_CONTROL5		0x015
#define MSK_IX_GATE_LTPS_IF_CONTROL6		0x016
#define MSK_IX_GATE_LTPS_IF_CONTROL7		0x017
#define MSK_IX_GATE_LTPS_IF_CONTROL8		0x018
#define MSK_IX_GATE_LTPS_IF_CONTROL9		0x019
#define MSK_IX_GATE_LTPS_IF_CONTROL10		0x01B

// Power Control System
#define MSK_IX_POWER_CONTROL1				0x100
#define MSK_IX_GATE_IF_CONTROL1				0x110
#define MSK_IX_GATE_IF_CONTROL2				0x111

// RAM Access System
#define MSK_IX_RAM_ADDR_SET1				0x200	// Horizontal Address, Y
#define MSK_IX_RAM_ADDR_SET2				0x201	// Vertical Address, X
#define MSK_IX_RAM_DATA_RW					0x202
#define MSK_IX_RAM_WRITE_DATA_MASK1			0x203
#define MSK_IX_RAM_WRITE_DATA_MASK2			0x204

// Window Address Control System
#define MSK_IX_HOR_RAM_ADDR_POSITION1		0x210	// Horizontal Start Address, Y 
#define MSK_IX_HOR_RAM_ADDR_POSITION2		0x211	// Horizontal End Address, Y
#define MSK_IX_VER_RAM_ADDR_POSITION1		0x212	// Vertical Start Address, X
#define MSK_IX_VER_RAM_ADDR_POSITION2		0x213	// Vertical End Address, X

// Gamma Control
#define MSK_IX_GAMMA_CONTROL1				0x300
#define MSK_IX_GAMMA_CONTROL2				0x301
#define MSK_IX_GAMMA_CONTROL3				0x302
#define MSK_IX_GAMMA_CONTROL4				0x303
#define MSK_IX_GAMMA_CONTROL5				0x304
#define MSK_IX_GAMMA_CONTROL6				0x305
#define MSK_IX_GAMMA_CONTROL7				0x306
#define MSK_IX_GAMMA_CONTROL8				0x307
#define MSK_IX_GAMMA_CONTROL9				0x308
#define MSK_IX_GAMMA_CONTROL10				0x309

//Coordinate Control System
#define MSK_IX_LINE_NUMBER_CONTROL			0x400
#define MSK_IX_SCREEN_CONTROL				0x401
#define MSK_IX_BASE_PIC_RAM_AREA_START		0x402
#define MSK_IX_BASE_PIC_RAM_AREA_END		0x403
#define MSK_IX_VER_SCROLL_CONTROL			0x404
#define MSK_IX_BASC_PIC1_RAM_MAG_AREA_START	0x405
#define MSK_IX_BASC_PIC1_RAM_MAG_AREA_END	0x406

// OSD Control
#define MSK_IX_OSD_PIC1_DISPLAY_POSITION	0x500
#define MSK_IX_OSD_PIC1_RAM_AREA_START		0x501
#define MSK_IX_OSD_PIC1_RAM_AREA_END		0x502
#define MSK_IX_OSD_PIC2_DISPLAY_POSITION	0x503
#define MSK_IX_OSD_PIC2_RAM_AREA_START		0x504
#define MSK_IX_OSD_PIC2_RAM_AREA_END		0x505
#define MSK_IX_OSD_PIC3_DISPLAY_POSITION	0x506
#define MSK_IX_OSD_PIC3_RAM_AREA_START		0x507
#define MSK_IX_OSD_PIC3_RAM_AREA_END		0x508

#define MSK_IX_GATE_IF_CONTROL1_TE			0x0100					



// HD66795 �������� (Gate Driver)
#define GATE_IX_POWER_CONTROL1				0x00
#define GATE_IX_POWER_CONTROL2				0x01
#define GATE_IX_POWER_CONTROL3				0x02
#define GATE_IX_POWER_CONTROL4				0x03
#define GATE_IX_DISPLAY_CONTROL1			0x05
#define GATE_IX_DISPLAY_CONTROL2			0x06
#define GATE_IX_DISPLAY_CONTROL3			0x07
#define GATE_IX_GATE_SCAN_CONTROL1			0x08
#define GATE_IX_GATE_SCAN_CONTROL2			0x0C




#endif




