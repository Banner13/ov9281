#ifndef OV9281_H
#define OV9281_H

#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

struct sensor_iic_data {
	uint16_t reg;
	uint8_t  *buf;
	uint8_t  len;
};


#define SENSOR_IIC_WRITE   _IOWR('V', BASE_VIDIOC_PRIVATE + 20, struct sensor_iic_data)
#define SENSOR_IIC_READ	   _IOWR('V', BASE_VIDIOC_PRIVATE + 21, struct sensor_iic_data)
#define SENSOR_KEEP_POWER  _IOW('V', BASE_VIDIOC_PRIVATE + 22, int)
#define SENSOR_ENGINE_INIT _IO('V', BASE_VIDIOC_PRIVATE + 23)
#define SENSOR_SET_ADDRESS  _IOW('V', BASE_VIDIOC_PRIVATE + 24, int)

struct OV9281_regs_init {
	uint16_t reg;
	uint8_t value;
};

static struct OV9281_regs_init OV9281_1280_800_regs_init_list[] =
{

	{ 0x0100, 0x00 },			// PLL configuration
    { 0x0103, 0x01 },			// SC_SOFTWARE_RESET
	{ 0x0302, 0x28 },			// PLL_CTRL_02
	{ 0x030d, 0x50 },			// PLL_CTRL_0D
    { 0x030e, 0x06 },			// PLL_CTRL_0E

    { 0x3001, 0x62 },			// output drive capability control
    { 0x3004, 0x01 },			// SC_CTRL_04
    { 0x3005, 0xff },			// SC_CTRL_05
    { 0x3006, 0xea },			// SC_CTRL_06

	
    { 0x3011, 0x0a },			// SC_CTRL_11
    { 0x3013, 0x18 },			// SC_MIPI_PHY1
    { 0x3022, 0x07 },			// SC_MISC_CTRL
    { 0x3030, 0x10 },			// SC_LP_CTRL4
    { 0x3039, 0x2e },			// SC_CTRL_39
    { 0x303a, 0xf0 },			// SC_CTRL_3A


    { 0x3500, 0x00 },			// LONG EXPO
    { 0x3501, 0x02 },			// LONG EXPO
    { 0x3502, 0xe0 },			// LONG EXPO
    
    { 0x3503, 0x0B },			// AEC MANUAL
    { 0x3505, 0x8c },			// GCVT OPTION
    { 0x3507, 0x03 },			// GAIN SHIFT
    { 0x3508, 0x00 },			// LONG GAIN
    { 0x3509, 0x20 },			// LONG GAIN

    { 0x3666, 0x5a },			// ANA_CORE_6

/*	SENSOR CONTROL REGISTERS
    { 0x3712, 0x80 },
    { 0x372d, 0x22 },
    { 0x3731, 0x80 },
    { 0x3732, 0x30 },
    { 0x3778, 0x00 },
    { 0x377d, 0x22 },
    { 0x3788, 0x02 },
    { 0x3789, 0xa4 },
    { 0x378a, 0x00 },
    { 0x378b, 0x4a },
    { 0x3799, 0x20 },
*/

    { 0x3800, 0x00 },			// Array Horizontal Start Point High Byte
    { 0x3801, 0x00 },			// Array Horizontal Start Point Low Byte
    { 0x3802, 0x00 },			// Array Vertical Start Point High Byte
    { 0x3803, 0x00 },			// Array Vertical Start Point Low Byte
    { 0x3804, 0x05 },			// Array Horizontal End Point High Byte
    { 0x3805, 0x0f },			// Array Horizontal End Point Low Byte
    { 0x3806, 0x03 },			// Array Vertical End Point High Byte
    { 0x3807, 0x2f },			// Array Vertical End Point Low Byte

/*
    { 0x3808, 0x05 },			// ISP Horizontal Output Width High Byte
    { 0x3809, 0x00 },			// ISP Horizontal Output Width Low Byte
    { 0x380a, 0x03 },			// ISP Vertical Output Height High Byte
    { 0x380b, 0x20 },			// ISP Vertical Output Height Low Byte


    { 0x380c, 0x02 },			// Total Horizontal Timing Size High Byte
    { 0x380d, 0xd8 },			// Total Horizontal Timing Size Low Byte
    { 0x380e, 0x03 },			// Total Vertical Timing Size High Byte
    { 0x380f, 0x8e },			// Total Vertical Timing Size Low Byte


    { 0x3810, 0x00 },			// ISP Horizontal Windowing Offset High Byte
    { 0x3811, 0x08 },			// ISP Horizontal Windowing Offset Low Byte
    { 0x3812, 0x00 },			// ISP Vertical Windowing Offset High Byte
    { 0x3813, 0x08 },			// ISP Vertical Windowing Offset Low Byte
    { 0x3814, 0x11 },			// TIMING_X_INC
    { 0x3815, 0x11 },			// TIMING_Y_INC


	{ 0x3820, 0x40 },			// TIMING_FORMAT1
    { 0x3821, 0x00 },			// TIMING_FORMAT2
*/


    { 0x3920, 0xff },			// PWM_CTRL_20
    { 0x3921, 0x00 },			// PWM_CTRL_21
    { 0x3922, 0x00 },			// PWM_CTRL_22
    { 0x3923, 0x00 },			// PWM_CTRL_23
    { 0x3924, 0x00 },			// PWM_CTRL_24
    { 0x3925, 0x00 },			// PWM_CTRL_25
    { 0x3926, 0x00 },			// PWM_CTRL_26
    { 0x3927, 0x00 },			// PWM_CTRL_27
    { 0x3928, 0x1a },			// PWM_CTRL_28
	{ 0x3929, 0x00 },			// PWM_CTRL_29
	{ 0x392A, 0x00 },			// PWM_CTRL_2A


    { 0x4003, 0x40 },			// BLC_CTRL_03
    { 0x4008, 0x04 },			// BLC_CTRL_08
    { 0x4009, 0x0b },			// BLC_CTRL_09
    { 0x400c, 0x00 },			// BLC_CTRL_0C
    { 0x400d, 0x07 },			// BLC_CTRL_0D
    { 0x4010, 0x40 },			// BLC_CTRL_10
    { 0x4043, 0x40 },			// BLC_CTRL_43

    { 0x4307, 0x30 },			// EMBED_CTRL
    { 0x4317, 0x01 },			// MIPI/DVP MODE OPTION

    { 0x4601, 0x04 },			// VFIFO Read Start Point Low Byte


	{ 0x4702, 0x00 },			// VSYNC_RISE_LNT
	{ 0x4703, 0x02 },			// VSYNC_RISE_LNT
	{ 0x4704, 0x00 },			// VSYNC_FALL_LNT
	{ 0x4705, 0x06 },			// VSYNC_FALL_LNT

	
    { 0x470f, 0xe0 },			// BYP_SEL
    { 0x4800, 0x00 },			// MIPI_CTRL00
    { 0x4f07, 0x00 },			// PCHG_ST_OFFS

    { 0x5000, 0x9f },			// ISP CTRL 00
    { 0x5001, 0x00 },			// ISP CTRL 01
    { 0x5e00, 0x00 },			// PRE CTRL00

    { 0x5d00, 0x0b },
    { 0x5d01, 0x02 },
};


void OV9281_Write(int df, uint16_t reg, uint8_t data);
int open_ov9281(int video_fd);

#endif
