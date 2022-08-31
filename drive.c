


/*
*   company:ShiYin
*   author:Jie.Zhou
*   date: 2022/8/15
*/

/* ****************************************************************************
*   include
* ****************************************************************************/
#include <stdio.h>
#include <stdbool.h>
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
#include "ov9281.h"
/* ****************************************************************************
*   definition
* ****************************************************************************/
#define CLEAR(x) memset(&(x), 0, sizeof(x))

typedef struct	//帧缓冲区
{
	void* start;
	unsigned int length;
}buffer;



/* ****************************************************************************
*   global variable
* ****************************************************************************/
char g_device_name[16] = "/dev/video0";
int g_device_fd = -1;
uint32_t g_Width = 1280;
uint32_t g_Height = 800;
uint32_t g_Pixelformat = V4L2_PIX_FMT_SBGGR8;
uint32_t g_Field = V4L2_FIELD_NONE;

uint32_t g_Buffer_count = 12;
buffer* g_Buffers_table = NULL;

uint32_t g_Run_flag = 1;


/* ****************************************************************************
*   code
* ****************************************************************************/

void process_image(buffer buf)
{
    printf("===process_image===\n");

    int fd = open("/tmp/test.bmp",O_RDWR|O_CREAT|O_APPEND,0777);
	char bmpHeader[BMP_BITMAPHEADER_SIZE];
	FillBmpHeader(bmpHeader, BMP_BITMAPHEADER_SIZE + g_Width*g_Height, g_Width, g_Height);
	write(fd, bmpHeader, BMP_BITMAPHEADER_SIZE);
    write(fd,buf.start,buf.length);

    close(fd);
}

static inline int open_video(const char *device_name)
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

	fd = open(device_name, O_RDWR , 0);
    // fd = open(device_name, O_RDWR | O_NONBLOCK, 0);
    if (-1 == fd) 
    {
        printf("open %s fail\n", device_name);
        return -1;
    }

    return fd;
}


void EngineOpen(const char *device_name)
{
	int video_fd = open_video(device_name);
	g_device_fd = open_ov9281(video_fd);
}


static void init_device(int video_fd, uint16_t width, uint16_t height, bool use_udmabuf)
{
	/* capabilities */

	struct v4l2_capability cap;

	if (-1 == ioctl(video_fd, VIDIOC_QUERYCAP, &cap)) {
		printf("VIDIOC_QUERYCAP error\n");
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		printf("V4L2_CAP_VIDEO_CAPTURE error\n");
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		printf("V4L2_CAP_STREAMING error\n");
	}

	/* Select video input, video standard and tune here. */

	struct v4l2_cropcap cropcap;

	CLEAR(cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(video_fd, VIDIOC_CROPCAP, &cropcap))
		printf("VIDIOC_CROPCAP error\n");

	struct v4l2_crop crop;

	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	crop.c = cropcap.defrect; /* reset to default */

	if (-1 == ioctl(video_fd, VIDIOC_S_CROP, &crop))
		printf("VIDIOC_S_CROP error\n");

	struct v4l2_format fmt;

	/* format */

	CLEAR(fmt);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = width;
	fmt.fmt.pix.height = height;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR8;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;

	if (-1 == ioctl(video_fd, VIDIOC_S_FMT, &fmt))
		printf("VIDIOC_S_FMT error\n");

	struct v4l2_requestbuffers req;
	CLEAR(req);

	req.count = g_Buffer_count;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = use_udmabuf ? V4L2_MEMORY_USERPTR : V4L2_MEMORY_MMAP;

	if (-1 == ioctl(video_fd, VIDIOC_REQBUFS, &req)) {
		printf("V4L2_BUF_TYPE_VIDEO_CAPTURE error\n");
	}
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


void InitRegisters(void)
{

	int sz = 0;
	int fps = 60;
	int lcu = 19;
	

	sz = sizeof(OV9281_1280_800_regs_init_list)/ sizeof(OV9281_1280_800_regs_init_list[0]);
	for(int i = 0;i<sz;i++) {
		OV9281_Write(g_device_fd, OV9281_1280_800_regs_init_list[i].reg, OV9281_1280_800_regs_init_list[i].value);
	}
	fps = 60;
	lcu = 19;

	return;
}


void video_init(void)
{

	EngineOpen(g_device_name);

	init_device(g_device_fd, g_Width, g_Height, false);

	g_Buffers_table = create_buffer_table();

	into_queue_and_record();

	InitRegisters();

    printf("video_init done .\n");
}

void video_thread()
{
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	OV9281_Write(g_device_fd, 0x0100, 0x01);
    ioctl (g_device_fd, VIDIOC_STREAMON, &type);

    //while(g_Run_flag)
    do{
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        ioctl (g_device_fd, VIDIOC_DQBUF, &buf);

        process_image(g_Buffers_table[buf.index]);

        ioctl(g_device_fd, VIDIOC_QBUF, &buf);

        usleep(40);
    }while(0);


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

