/*
*   company:ShiYin
*   author:Jie.Zhou
*   date: 2022/8/15
*/

#ifndef BMP_IMAGE_G
#define BMP_IMAGE_G


/* ****************************************************************************
*   definition
* ****************************************************************************/
#define BMP_ID          "BM"

#define BMP_BITMAPFILEHEADER_SIZE   14
#pragma pack(1)
typedef struct {
    char            bfType[2]; 
    unsigned int    bfSize; 
    unsigned short  bfReserved1; 
    unsigned short  bfReserved2; 
    unsigned int    bfOffBits; 
} BMP_BITMAPFILEHEADER;
#pragma pack()

#define BMP_BITMAPINFOHEADER_SIZE   40
#pragma pack(1)
typedef struct {
    unsigned int    biSize;
    int             biWidth;
    int             biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned int    biCompression;
    unsigned int    biSizeImage;
    int             biXPelsPerMeter;
    int             biYPelsPerMeter;
    unsigned int    biClrUsed;
    unsigned int    biClrImportant;
} BMP_BITMAPINFOHEADER;
#pragma pack()

#pragma pack(1)
typedef struct { 
	unsigned char   rgbBlue;
	unsigned char   rgbGreen;
	unsigned char   rgbRed;
	unsigned char   rgbReserved;
} BMP_RGBQUAD;
#pragma pack() 

#define BMP_BITMAPHEADER_SIZE   1078
#pragma pack(1)
typedef struct { 
	BMP_BITMAPFILEHEADER fileHeader;
	BMP_BITMAPINFOHEADER infoHeader;
	BMP_RGBQUAD RGB_Quad[256];
} BMP;
#pragma pack() 


/*******************************************************************************
 *  prototype
 * ****************************************************************************/
void FillBmpHeader(char *buffer, int bufferSize, int imgWidth, int imgHeight);

#endif
