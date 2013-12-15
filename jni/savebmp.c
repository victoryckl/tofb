/********************************************************************
	created:	2012/02/07
	filename: 	savebmp.c
	author:		
	
	purpose:	
*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "mybmp.h"

typedef struct
{
	BMPHEADER header;
	union {
		RGB565INFO r565;
		RGB888INFO r888;
		RGBX8888INFO r8888;
	}u;
}BMP_WRITE;

static int get_rgb565_header(int w, int h, BMP_WRITE * bmp)
{
	int size = 0;
	int linesize = 0;
	int bytespp = 2;
	if (bmp) {
		linesize = UpAlign4(w * bytespp);
		size = linesize * h;
		memset(bmp, 0, sizeof(* bmp));
		
		bmp->header.bfType[0] = 'B';
		bmp->header.bfType[1] = 'M';
		bmp->header.bfOffBits = BMP_HEAD_SIZE + sizeof(bmp->u.r565);
		bmp->header.bfSize = UpAlign4(bmp->header.bfOffBits + size);
		size = bmp->header.bfSize - bmp->header.bfOffBits;
		
		bmp->u.r565.i.biSize = sizeof(bmp->u.r565);
		bmp->u.r565.i.biWidth = w;
		bmp->u.r565.i.biHeight = -h;
		bmp->u.r565.i.biPlanes = 1;
		bmp->u.r565.i.biBitCount = 8 * bytespp;
		bmp->u.r565.i.biCompression = BI_BITFIELDS;
		bmp->u.r565.i.biSizeImage = size;
		
		bmp->u.r565.rgb[0] = 0xF800;
		bmp->u.r565.rgb[1] = 0x07E0;
		bmp->u.r565.rgb[2] = 0x001F;
		bmp->u.r565.rgb[3] = 0;
		
		printf("rgb565:%dbpp,%d*%d,%d\n", bmp->u.r565.i.biBitCount, w, h, bmp->header.bfSize);
	}
	return size;
}

static int get_rgb888_header(int w, int h, BMP_WRITE * bmp)
{
	int size = 0;
	int linesize = 0;
	int bytespp = 3;
	if (bmp) {
		linesize = UpAlign4(w * bytespp);
		size = linesize * h;
		memset(bmp, 0, sizeof(* bmp));

		bmp->header.bfType[0] = 'B';
		bmp->header.bfType[1] = 'M';
		bmp->header.bfOffBits = BMP_HEAD_SIZE + sizeof(bmp->u.r888);
		bmp->header.bfSize = bmp->header.bfOffBits + size;
		bmp->header.bfSize = UpAlign4(bmp->header.bfSize);//windows要求文件大小必须是4的倍数
		size = bmp->header.bfSize - bmp->header.bfOffBits;
		
		bmp->u.r888.i.biSize = sizeof(bmp->u.r888);
		bmp->u.r888.i.biWidth = w;
		bmp->u.r888.i.biHeight = -h;
		bmp->u.r888.i.biPlanes = 1;
		bmp->u.r888.i.biBitCount = 8 * bytespp;
		bmp->u.r888.i.biCompression = BI_RGB;
		bmp->u.r888.i.biSizeImage = size;

		printf("rgb888:%dbpp,%d*%d,%d\n", bmp->u.r888.i.biBitCount, w, h, bmp->header.bfSize);
	}
	return size;
}

static int get_rgbx8888_header(int w, int h, BMP_WRITE * bmp)
{
	int size = 0;
	int linesize = 0;
	int bytespp = 4;
	if (bmp) {
		linesize = UpAlign4(w * bytespp);
		size = linesize * h;
		memset(bmp, 0, sizeof(* bmp));
		
		bmp->header.bfType[0] = 'B';
		bmp->header.bfType[1] = 'M';
		bmp->header.bfOffBits = BMP_HEAD_SIZE + sizeof(bmp->u.r8888);
		bmp->header.bfSize = bmp->header.bfOffBits + size;
		bmp->header.bfSize = UpAlign4(bmp->header.bfSize);
		size = bmp->header.bfSize - bmp->header.bfOffBits;
		
		bmp->u.r8888.i.biSize = sizeof(bmp->u.r8888);
		bmp->u.r8888.i.biWidth = w;
		bmp->u.r8888.i.biHeight = -h;
		bmp->u.r8888.i.biPlanes = 1;
		bmp->u.r8888.i.biBitCount = 8 * bytespp;
		bmp->u.r8888.i.biCompression = BI_BITFIELDS;
		bmp->u.r8888.i.biSizeImage = size;
		
		bmp->u.r8888.rgb[0] = 0xFF000000;
		bmp->u.r8888.rgb[1] = 0x00FF0000;
		bmp->u.r8888.rgb[2] = 0x0000FF00;
		bmp->u.r8888.rgb[3] = 0;
		
		printf("rgbx8888:%dbpp,%d*%d,%d\n", bmp->u.r8888.i.biBitCount, w, h, bmp->header.bfSize);
	}
	return size;
}

int save_bmp(const char * path, int w, int h, void * pdata, int bpp)
{
	int success = -1;
	FILE * hfile = NULL;
	int size = 0;
	BMP_WRITE bmp;

	do 
	{
		if (path == NULL || w <= 0 || h <= 0 || pdata == NULL) {
			printf("if (path == NULL || w <= 0 || h <= 0 || pdata == NULL)\n");
			break;
		}

		remove(path);
		hfile = fopen(path, "wb");
		if (hfile == NULL) {
			printf("open(%s) failed!\n", path);
			break;
		}

		printf("%s,", path);
		switch (bpp)
		{
		case 16:
			size = get_rgb565_header(w, h, &bmp);
			break;
		case 24:
		size = get_rgb888_header(w, h, &bmp);
			break;
		case 32:
			size = get_rgbx8888_header(w, h, &bmp);
			break;
		default:
			printf("\nerror: not support format!\n");
			break;
		}

		if (size < 0) {
			break;
		}

		fwrite(bmp.header.bfType, 1, BMP_HEAD_SIZE, hfile);
		fwrite(&(bmp.u.r565), 1, bmp.u.r565.i.biSize, hfile);
		fwrite(pdata, 1, size, hfile);
		success = 0;
	} while (0);

	if (hfile != NULL)
		fclose(hfile);
	
	return success;
}

//-------------------------------------------------------------------
