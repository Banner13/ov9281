


/*
*   company:ShiYin
*   author:Jie.Zhou
*   date: 2022/8/15
*/

/* ****************************************************************************
*   include
* ****************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include "bmp.h"
/* ****************************************************************************
*   definition
* ****************************************************************************/
#define CLEAR(x) memset(&(x), 0, sizeof(x))

typedef struct	//帧缓冲区
{
	void* start;
	unsigned int length;
}buffer;

#define SENSOR_IIC_WRITE   _IOWR('V', BASE_VIDIOC_PRIVATE + 20, struct sensor_iic_data)
#define SENSOR_IIC_READ	   _IOWR('V', BASE_VIDIOC_PRIVATE + 21, struct sensor_iic_data)
#define SENSOR_KEEP_POWER  _IOW('V', BASE_VIDIOC_PRIVATE + 22, int)
#define SENSOR_ENGINE_INIT _IO('V', BASE_VIDIOC_PRIVATE + 23)
#define SENSOR_SET_ADDRESS  _IOW('V', BASE_VIDIOC_PRIVATE + 24, int)



struct sensor_iic_data {
	uint16_t reg;
	uint8_t  *buf;
	uint8_t  len;
};

/* ****************************************************************************
*   global variable
* ****************************************************************************/
char g_device_name[16] = "/dev/video0";
int g_device_fd = -1;
uint32_t g_Width = 1280;
uint32_t g_Height = 800;
uint32_t g_Pixelformat = V4L2_PIX_FMT_SBGGR8;
uint32_t g_Field = V4L2_FIELD_NONE;

uint32_t g_Buffer_count = 4;
buffer* g_Buffers_table = NULL;

uint32_t g_Run_flag = 1;


/* ****************************************************************************
*   code
* ****************************************************************************/
void OV9281_Write(int df, uint16_t reg, uint8_t data)
{
	struct sensor_iic_data iic;

	iic.reg = reg;
	iic.buf = (uint8_t *)(&data);
	iic.len = 1;
	ioctl(df, SENSOR_IIC_WRITE, &iic);
	
	return;
};
	

void process_image(buffer buf)
{
    printf("===process_image===\n");

    int fd = open("/tmp/test1.bmp",O_RDWR|O_CREAT|O_APPEND,0777);
	char bmpHeader[BMP_BITMAPHEADER_SIZE];
	FillBmpHeader(bmpHeader, BMP_BITMAPHEADER_SIZE + g_Width*g_Height, g_Width, g_Height);
	write(fd, bmpHeader, BMP_BITMAPHEADER_SIZE);
    write(fd,buf.start,buf.length);

    close(fd);
}

static inline int open_device(const char *device_name)
{
    struct stat st;
    int fd;

    if (-1 == stat(device_name, &st)) 
    {
        printf("device %s not found\n", device_name);
        return -1;
    }

    if (!S_ISCHR(st.st_mode)) 
    {
        printf("%s is not char-device\n", device_name);
        return -2;
    }

    fd = open(device_name, O_RDWR | O_NONBLOCK, 0);
    if (-1 == fd) 
    {
        printf("open %s fail\n", device_name);
        return -1;
    }

	int i2c_addr = 0x60;
	int video_fd = fd;
	
	if (ioctl(video_fd, SENSOR_SET_ADDRESS, &i2c_addr) < 0) {
		printf("Set sensor addr ioctl error\n");
	}

    return video_fd;
}

static inline void get_capacity(void)
{
    struct v4l2_capability cap;
    CLEAR(cap);

    ioctl(g_device_fd, VIDIOC_QUERYCAP, &cap);

    printf("\n/********************************\n");
    printf("DriverName:%s\nCard Name:%s\nBus info:%s\nDriverVersion:%u.%u.%u\n" 
                                                                      , cap.driver 
                                                                      , cap.card 
                                                                      , cap.bus_info 
                                                                      , (cap.version>>16)&0xff 
                                                                      , (cap.version>>8)&0xff 
                                                                      , cap.version&0xff);
    printf("********************************/\n\n");
}

static inline void get_fmtdesc(void)
{
    struct v4l2_fmtdesc fmtdesc;
    CLEAR(fmtdesc);

    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    printf("Support format:\n");

    while(-1 != ioctl(g_device_fd, VIDIOC_ENUM_FMT, &fmtdesc))
    {
        printf("\t%d.%s\n", fmtdesc.index+1, fmtdesc.description);
        fmtdesc.index++;
    }
}

static inline void set_format(uint32_t Width, uint32_t Height, uint32_t Pixelformat, uint32_t Field)
{
    struct v4l2_format format;
    CLEAR(format);
    
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = Width;
    format.fmt.pix.height = Height; 
    format.fmt.pix.pixelformat = Pixelformat;
    format.fmt.pix.field = Field;
    
    int ret = ioctl(g_device_fd, VIDIOC_S_FMT, &format);
    
    if(ret < 0){
        printf("VIDIOC_S_FMT fail\n");
        return;
    }
}

static inline void get_format(void)
{
    struct v4l2_format format;
    CLEAR(format);

    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    int ret = ioctl(g_device_fd, VIDIOC_G_FMT, &format);

    if(ret < 0)
    {
        printf("VIDIOC_G_FMT fail\n");
        return;
    }
    
    printf("Currentdata format information:\n\twidth:%d\n\theight:%d\n\ttype:%x\n\tpixelformat:%c%c%c%c\n"
                                                , format.fmt.pix.width 
                                                , format.fmt.pix.height 
                                                , format.type 
                                                , format.fmt.pix.pixelformat&0xff 
                                                , (format.fmt.pix.pixelformat>>8)&0xff 
                                                , (format.fmt.pix.pixelformat>>16)&0xff 
                                                , (format.fmt.pix.pixelformat>>24)&0xff);
}

static inline void request_buffers()
{
    struct v4l2_requestbuffers req;
    CLEAR(req);

    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.count = g_Buffer_count;
    req.memory = V4L2_MEMORY_MMAP;
    
    int ret = ioctl(g_device_fd, VIDIOC_REQBUFS, &req);
    if(0 != ret)
    {
        printf("VIDIOC_REQBUFS fail\n");
        return;
    }

}

static inline buffer* create_buffer_table(void)
{
    buffer* buffers_table = (buffer*)calloc(g_Buffer_count, sizeof(buffer));

    if (!buffers_table) 
    {
        fprintf (stderr,"Out of memory/n");

        if (-1 != g_device_fd)
            close(g_device_fd);
        
        exit(EXIT_FAILURE);
    }
/*
for (int i = 0; i < g_Buffer_count; i++)
{
    printf("buffers_table[%d].start is %p\n", i, buffers_table[i].start);
    printf("buffers_table[%d].length is %x\n", i, buffers_table[i].length);
}
*/
    return buffers_table;
}

static inline void into_queue_and_record(void)
{
    for (int i = 0; i < g_Buffer_count; i++) 
    {
        struct v4l2_buffer buf;
        CLEAR(buf);
        
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
printf("[%d]\n", i);
        if (-1 == ioctl(g_device_fd, VIDIOC_QUERYBUF, &buf)) 
        {
            printf("QUERYBUF fail\n");

            if (g_Buffers_table)
                free(g_Buffers_table);

            if (-1 != g_device_fd)
                close(g_device_fd);

            exit(-1);
        }

        g_Buffers_table[i].length= buf.length;
        g_Buffers_table[i].start = mmap(NULL 
                                        , buf.length 
                                        , PROT_READ | PROT_WRITE 
                                        , MAP_SHARED 
                                        , g_device_fd 
                                        , buf.m.offset);

        if (MAP_FAILED == g_Buffers_table[i].start) 
        {
            printf("mmap fail\n");
        
            if (g_Buffers_table)
                free(g_Buffers_table);
        
            if (-1 != g_device_fd)
                close(g_device_fd);
        
            exit(-1);
        }

        ioctl (g_device_fd, VIDIOC_QBUF, &buf);
    }

}

void video_init(void)
{
    g_device_fd = open_device(g_device_name);

    get_capacity();

    get_fmtdesc();

    set_format(g_Width, g_Height, g_Pixelformat, g_Field);

    get_format();

    request_buffers();

    g_Buffers_table = create_buffer_table();

    into_queue_and_record();

    printf("video_init done .\n");
}

void video_thread()
{
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	OV9281_Write(g_device_fd, 0x0100, 0x01);
    ioctl (g_device_fd, VIDIOC_STREAMON, &type);
printf("%d\n", __LINE__);
    //while(g_Run_flag)
    do{
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
		printf("%d\n", __LINE__);

        ioctl (g_device_fd, VIDIOC_DQBUF, &buf);
		printf("%d\n", __LINE__);

        process_image(g_Buffers_table[buf.index]);
		printf("%d\n", __LINE__);

        ioctl(g_device_fd, VIDIOC_QBUF, &buf);

        usleep(40);
    }while(0);
printf("%d\n", __LINE__);

    ioctl(g_device_fd, VIDIOC_STREAMOFF, &type);

}


static inline void video_deinit(void)
{
    for (int i = 0; i < g_Buffer_count; i++)
        munmap(g_Buffers_table[i].start, g_Buffers_table[i].length);

    if (g_Buffers_table)
        free(g_Buffers_table);
    
    if (-1 != g_device_fd)
        close(g_device_fd);
}


int main()
{

    video_init();

    video_thread();

    video_deinit();

    return 0;
}

