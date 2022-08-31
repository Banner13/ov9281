

#include "ov9281.h"

void OV9281_Write(int df, uint16_t reg, uint8_t data)
{
	struct sensor_iic_data iic;

	iic.reg = reg;
	iic.buf = (uint8_t *)(&data);
	iic.len = 1;
	ioctl(df, SENSOR_IIC_WRITE, &iic);
	
	return;
};

int open_ov9281(int video_fd)
{
	int i2c_addr = 0x60;
	int fd = video_fd;
	
	if (ioctl(fd, SENSOR_SET_ADDRESS, &i2c_addr) < 0) {
		printf("Set sensor addr ioctl error\n");
	}

	return fd;
}


