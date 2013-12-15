/********************************************************************
	created:	2012/04/07
	filename: 	readbmp.c
	author:		
	
	purpose:	
*********************************************************************/

//-------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "mybmp.h"

struct tagBMP_Read
{
	BMPHEADER header;
	union {
		RGB565INFO r565;
		RGB888INFO r888;
		RGBX8888INFO r8888;
	}u;

	int tpye;//类型
	int forward;//图像行数据是否是正向的
	FILE * hfile;
	int dataSize;//图像数据的大小
	unsigned char * pdata;//图像数据
};

static long filesize(FILE *stream)
{
	long curpos, length;
	curpos = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	length = ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}

static int read_head(BMP_READ * bmp)
{
	fseek(bmp->hfile, 0, SEEK_SET);
	fread(bmp->header.bfType, 1, BMP_HEAD_SIZE, bmp->hfile);
	return 0;
}

static int read_info(BMP_READ * bmp)
{
	fseek(bmp->hfile, BMP_HEAD_SIZE, SEEK_SET);
	fread(&(bmp->u), 1, sizeof(bmp->u), bmp->hfile);
	return 0;
}

static int check_bmp(BMP_READ * bmp)
{
	int ok = -1;
	int bitCount, w, h;
	do 
	{
		if (bmp->header.bfType[0] != 'B' || bmp->header.bfType[1] != 'M') {
			printf("check_bmp : not bmp file! %c%c\n",
				bmp->header.bfType[0], bmp->header.bfType[1]);
			break;
		}

		w = bmp->u.r8888.i.biWidth;
		h = bmp->u.r8888.i.biHeight;
		if (w <= 0 || h == 0) {
			printf("check_bmp : bmp width(%d) or height(%d) is error\n", w, h);
			break;
		}

		bitCount = bmp->u.r8888.i.biBitCount;
		if (32 == bitCount) {
			if ( BI_BITFIELDS != bmp->u.r8888.i.biCompression
				||  0xFF000000 != bmp->u.r8888.rgb[0]
				||  0x00FF0000 != bmp->u.r8888.rgb[1]
				||  0x0000FF00 != bmp->u.r8888.rgb[2]
				||  0          != bmp->u.r8888.rgb[3] )
			{
				printf("sorry! 32bits only support rgbx8888 bmp!!\n");
				break;
			}
			bmp->tpye = eRGBX_8888;
		} else if (24 == bitCount) {
			if (bmp->u.r888.i.biCompression != BI_RGB) {
				printf("sorry! not support 24bits compress (%d) bmp!!\n",
					bmp->u.r888.i.biCompression);
				break;
			}
			bmp->tpye = eRGB_888;
		} else if (16 == bitCount) {
			if ( BI_BITFIELDS != bmp->u.r565.i.biCompression
				||  0xF800 != bmp->u.r565.rgb[0]
				||  0x07E0 != bmp->u.r565.rgb[1]
				||  0x001F != bmp->u.r565.rgb[2]
				||  0x0000 != bmp->u.r565.rgb[3] ) 
			{
				printf("sorry! 16bits only support rgb565 bmp!!\n");
				break;
			}
			bmp->tpye = eRGB_565;
		} else {
			printf("sorry! not support %d bits bmp!!\n", bitCount);
			break;
		}

		if (h < 0) {
			bmp->forward = 1;
			h = -h;
			bmp->u.r8888.i.biHeight = h;
		}
		bmp->dataSize = filesize(bmp->hfile) - bmp->header.bfOffBits;
		ok = 0;
	} while (0);

	return ok;
}

static int read_data(BMP_READ * bmp)
{
	unsigned char * pdata = NULL;
	pdata = (unsigned char *)malloc(bmp->dataSize);
	if (pdata) {
		fseek(bmp->hfile, bmp->header.bfOffBits, SEEK_SET);
		fread(pdata, 1, bmp->dataSize, bmp->hfile);
		bmp->pdata = pdata;
		return 0;
	} else {
		printf("read_data : malloc(%d)\n", bmp->dataSize);
		return -1;
	}
}

static BMP_READ * _bmp_open(const char * path)
{
	int success = -1;
	BMP_READ * bmp = NULL;
	FILE * hfile = NULL;
	do 
	{
		if (!path) {
			printf("bmp_open : path is null!\n");
			break;
		}
		hfile = fopen(path, "rb");
		if (!hfile) {
			printf("bmp_open : fopen(%s) failed!\n", path);
			break;
		}
		bmp = (BMP_READ *)malloc(sizeof(BMP_READ));
		if (!bmp) {
			printf("bmp_open : malloc(sizeof(BMP_READ)) failed! %d\n", sizeof(BMP_READ));
			break;
		}
		memset(bmp, 0, sizeof(BMP_READ));
		bmp->hfile = hfile;
		
		read_head(bmp);
		read_info(bmp);
		if (check_bmp(bmp) < 0) {
			break;
		}
		success = 0;
	} while (0);
	
	if (success < 0) {//如果失败，则清理现场
		if (bmp) {
			bmp_close(bmp);
			bmp = NULL;
		}
	}
	
	return bmp;
}

int bmp_info(const char * path, ImageInfo * imageinfo)
{
	int success = -1;
	BMP_READ * bmp = NULL;
	FILE * hfile = NULL;
	do 
	{
		if (NULL == imageinfo) {
			printf("bmp_info : imageinfo is null!\n");
			break;
		}
		memset(imageinfo, 0, sizeof(*imageinfo));
		bmp = _bmp_open(path);
		if (NULL == bmp) {
			break;
		}
		imageinfo->bpp = bmp->u.r8888.i.biBitCount;
		imageinfo->width = bmp->u.r8888.i.biWidth;
		imageinfo->height = bmp->u.r8888.i.biHeight;

		printf("%s,%s,%dbpp,%d*%d,forward:%d\n",
			path,
			g_rgbstr[bmp->tpye],
			imageinfo->bpp,
			imageinfo->width,
			imageinfo->height,
			bmp->forward);

		success = 0;
	} while (0);
	
	bmp_close(bmp);
	return success;
}

BMP_READ * bmp_open(const char * path)
{
	int success = -1;
	BMP_READ * bmp = NULL;
	FILE * hfile = NULL;
	do 
	{
		bmp = _bmp_open(path);
		if (NULL == bmp) {
			break;
		}
		if (read_data(bmp) < 0) {
			break;
		}
		success = 0;
	} while (0);

	if (bmp) {//关闭文件
		if (bmp->hfile) {
			fclose(bmp->hfile);
			bmp->hfile = NULL;
		}
	}
	
	if (success < 0) {//如果失败，则清理现场
		if (bmp) {
			bmp_close(bmp);
			bmp = NULL;
		}
	}
	
	return bmp;
}

void bmp_close(BMP_READ * bmp)
{
	if (bmp) {
		if (bmp->hfile) {
			fclose(bmp->hfile);
		}
		if (bmp->pdata) {
			free(bmp->pdata);
		}
		free(bmp);
	}
}

int bmp_width(BMP_READ * bmp)
{
	if (bmp)
		return bmp->u.r8888.i.biWidth;
	return 0;
}
int bmp_height(BMP_READ * bmp)
{
	if (bmp)
		return bmp->u.r8888.i.biHeight;
	return 0;
}
int bmp_bpp(BMP_READ * bmp)
{
	if (bmp)
		return bmp->u.r8888.i.biBitCount;
	return 0;
}
void * bmp_data(BMP_READ * bmp)
{
	if (bmp)
		return bmp->pdata;
	return NULL;
}
int bmp_forward(BMP_READ * bmp)
{
	if (bmp)
		return bmp->forward;
	return 0;
}


//-------------------------------------------------------------------
