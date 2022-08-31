/*******************************************************************************
 *  include
 * ****************************************************************************/
#include "bmp.h"



/*******************************************************************************
 *  code
 * ****************************************************************************/
void FillBmpHeader(char *buffer, int bufferSize, int imgWidth, int imgHeight)
{
	if (bufferSize < imgWidth * imgHeight + BMP_BITMAPHEADER_SIZE)
		return;

	BMP *bmpHeader;
	bmpHeader = (BMP*)buffer;

	bmpHeader->fileHeader.bfType[0] = 'B';
	bmpHeader->fileHeader.bfType[1] = 'M';
	bmpHeader->fileHeader.bfSize = imgWidth * imgHeight + BMP_BITMAPHEADER_SIZE;
	bmpHeader->fileHeader.bfOffBits = BMP_BITMAPHEADER_SIZE;

	bmpHeader->infoHeader.biSize = BMP_BITMAPINFOHEADER_SIZE;
	bmpHeader->infoHeader.biWidth = imgWidth;
	bmpHeader->infoHeader.biHeight = imgHeight;
	bmpHeader->infoHeader.biPlanes = 1;
	bmpHeader->infoHeader.biBitCount = 8;
	bmpHeader->infoHeader.biCompression = 0;
	bmpHeader->infoHeader.biSizeImage = imgWidth * imgHeight;
	bmpHeader->infoHeader.biXPelsPerMeter = 0;
	bmpHeader->infoHeader.biYPelsPerMeter = 0;
	bmpHeader->infoHeader.biClrUsed = 1 << 8;
	bmpHeader->infoHeader.biClrImportant = 1 << 8;

	for(int i = 0; i < 256; i++)
	{
		bmpHeader->RGB_Quad[i].rgbBlue = i;
		bmpHeader->RGB_Quad[i].rgbGreen = i;
		bmpHeader->RGB_Quad[i].rgbRed = i;
		bmpHeader->RGB_Quad[i].rgbReserved = 0;
	}

}


