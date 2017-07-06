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

//-------------------------------------------------------------------
//拷贝
void rgb_copy(const void * psrc, void * pdst, int sw, int sh, int dw, int dh, int bpp)
{
	int bytes = UpAlign8(bpp) >> 3; // bpp / 8
	int srclinesize = UpAlign4(sw * bytes);
	int dstlinesize = UpAlign4(dw * bytes);
	int copylinesize = srclinesize < dstlinesize ? srclinesize : dstlinesize;
	int copylines = sh < dh ? sh : dh;
	
	const unsigned char * psrcline = (const unsigned char *)psrc;
	const unsigned char * pend = psrcline + copylines * srclinesize;
	unsigned char * pdstline = (unsigned char *)pdst;
	
	while (psrcline < pend) {
		memcpy(pdstline, psrcline, copylinesize);
		psrcline += srclinesize;
		pdstline += dstlinesize;
	}
}

void rbg565_copy(const void * psrc, void * pdst, int sw, int sh, int dw, int dh)
{
	rgb_copy(psrc, pdst, sw, sh, dw, dh, 16);
}

void rbg888_copy(const void * psrc, void * pdst, int sw, int sh, int dw, int dh)
{
	rgb_copy(psrc, pdst, sw, sh, dw, dh, 24);
}

void rbgx8888_copy(const void * psrc, void * pdst, int sw, int sh, int dw, int dh)
{
	rgb_copy(psrc, pdst, sw, sh, dw, dh, 32);
}

//-------------------------------------------------------------------
//行数据翻转
void line_reversal(void * pdata, int w, int h, int bpp)
{
	int bytes     = UpAlign8(bpp) >> 3; // bpp / 8
	int linesize  = UpAlign4(w * bytes);//4的整数倍
	int copylines = h >> 1;

	int i;
	unsigned char * pline = NULL;
	unsigned char * pline1 = NULL;
	unsigned char * linebuffer = NULL;

	if (pdata && w > 0 && h > 1) {//至少两行才需要翻转
		linebuffer = (unsigned char *)malloc(linesize);
		if (linebuffer) {
			pline  = (unsigned char *)pdata;
			pline1 = (unsigned char *)pdata + linesize * (h - 1);
			for (i = 0; i < copylines; i++) {
				memcpy(linebuffer, pline, linesize);
				memcpy(pline, pline1, linesize);
				memcpy(pline1, linebuffer, linesize);
				pline  += linesize;
				pline1 -= linesize;
			}
			free(linebuffer);
		}
	}
}

void rgb565_line_reversal(void * p565, int w, int h)
{
	line_reversal(p565, w, h, 16);
}

void rgb888_line_reversal(void * p888, int w, int h)
{
	line_reversal(p888, w, h, 24);
}

void rgbx8888_line_reversal(void * p8888, int w, int h)
{
	line_reversal(p8888, w, h, 32);
}

//-------------------------------------------------------------------
//转换
static int rgb565_to_rgb888(const void * psrc, int w, int h, void * pdst)
{
	int srclinesize = UpAlign4(w * 2);
	int dstlinesize = UpAlign4(w * 3);

	const unsigned char  * psrcline;
	const unsigned short * psrcdot;
	unsigned char  * pdstline;
	unsigned char  * pdstdot;

	int i,j;

	if (!psrc || !pdst || w <= 0 || h <= 0) {
		printf("rgb565_to_rgb888 : parameter error\n");
		return -1;
	}

	psrcline = (const unsigned char *)psrc;
	pdstline = (unsigned char *)pdst;
	for (i=0; i<h; i++) {
		psrcdot = (const unsigned short *)psrcline;
		pdstdot = pdstline;
		for (j=0; j<w; j++) {
			//565 b|g|r -> 888 r|g|b
			*pdstdot++ = (unsigned char)(((*psrcdot) >> 0 ) << 3);
			*pdstdot++ = (unsigned char)(((*psrcdot) >> 5 ) << 2);
			*pdstdot++ = (unsigned char)(((*psrcdot) >> 11) << 3);
			psrcdot++;
		}
		psrcline += srclinesize;
		pdstline += dstlinesize;
	}

	return 0;
}

static int rgb888_to_rgb565(const void * psrc, int w, int h, void * pdst)
{
	int srclinesize = UpAlign4(w * 3);
	int dstlinesize = UpAlign4(w * 2);
	
	const unsigned char * psrcline;
	const unsigned char * psrcdot;
	unsigned char  * pdstline;
	unsigned short * pdstdot;
	
	int i,j;
	
	if (!psrc || !pdst || w <= 0 || h <= 0) {
		printf("rgb888_to_rgb565 : parameter error\n");
		return -1;
	}

	psrcline = (const unsigned char *)psrc;
	pdstline = (unsigned char *)pdst;
	for (i=0; i<h; i++) {
		psrcdot = psrcline;
		pdstdot = (unsigned short *)pdstline;
		for (j=0; j<w; j++) {
			//888 r|g|b -> 565 b|g|r
			*pdstdot =  (((psrcdot[0] >> 3) & 0x1F) << 0)//r
						|(((psrcdot[1] >> 2) & 0x3F) << 5)//g
						|(((psrcdot[2] >> 3) & 0x1F) << 11);//b
			psrcdot += 3;
			pdstdot++;
		}
		psrcline += srclinesize;
		pdstline += dstlinesize;
	}

	return 0;
}

int bgr565_to_abgr8888(const void * psrc, int w, int h, void * pdst)
{
	int srclinesize = UpAlign4(w * 2);
	int dstlinesize = UpAlign4(w * 4);

	const unsigned char  * psrcline;
	const unsigned short * psrcdot;
	unsigned char  * pdstline;
	unsigned char  * pdstdot;

	int i,j;

	if (!psrc || !pdst || w <= 0 || h <= 0) {
		printf("bgr565_to_abgr8888 : parameter error\n");
		return -1;
	}

	psrcline = (const unsigned char *)psrc;
	pdstline = (unsigned char *)pdst;
	for (i=0; i<h; i++) {
		psrcdot = (const unsigned short *)psrcline;
		pdstdot = pdstline;
		for (j=0; j<w; j++) {
			/*a*/*pdstdot++ = 0xFF;
			/*b*/*pdstdot++ = (unsigned char)(((*psrcdot) >> 11) << 3);//b
			/*g*/*pdstdot++ = (unsigned char)(((*psrcdot) >> 5 ) << 2);//g
			/*r*/*pdstdot++ = (unsigned char)(((*psrcdot) >> 0 ) << 3);//r
			psrcdot++;
		}
		psrcline += srclinesize;
		pdstline += dstlinesize;
	}
	
	return 0;
}

int rgb565_to_bgra8888(const void * psrc, int w, int h, void * pdst)
{
	int srclinesize = UpAlign4(w * 2);
	int dstlinesize = UpAlign4(w * 4);

	const unsigned char  * psrcline;
	const unsigned short * psrcdot;
	unsigned char  * pdstline;
	unsigned char  * pdstdot;

	int i,j;

	if (!psrc || !pdst || w <= 0 || h <= 0) {
		printf("rgb565_to_bgra8888 : parameter error\n");
		return -1;
	}

	psrcline = (const unsigned char *)psrc;
	pdstline = (unsigned char *)pdst;
	for (i=0; i<h; i++) {
		psrcdot = (const unsigned short *)psrcline;
		pdstdot = pdstline;
		for (j=0; j<w; j++) {
			/*b*/*pdstdot++ = (unsigned char)(((*psrcdot) >> 0 ) << 3);//b
			/*g*/*pdstdot++ = (unsigned char)(((*psrcdot) >> 5 ) << 2);//g
			/*r*/*pdstdot++ = (unsigned char)(((*psrcdot) >> 11) << 3);//r
			/*a*/*pdstdot++ = 0xFF;
			psrcdot++;
		}
		psrcline += srclinesize;
		pdstline += dstlinesize;
	}
	
	return 0;
}

static int bgra8888_to_rgb565(const void * psrc, int w, int h, void * pdst)
{
	int srclinesize = UpAlign4(w * 4);
	int dstlinesize = UpAlign4(w * 2);
	
	const unsigned char * psrcline;
	const unsigned char * psrcdot;
	unsigned char  * pdstline;
	unsigned short * pdstdot;
	
	int i,j;
	
	if (!psrc || !pdst || w <= 0 || h <= 0) {
		printf("bgra8888_to_rgb565 : parameter error\n");
		return -1;
	}
	
	psrcline = (const unsigned char *)psrc;
	pdstline = (unsigned char *)pdst;
	for (i=0; i<h; i++) {
		psrcdot = psrcline;
		pdstdot = (unsigned short *)pdstline;
		for (j=0; j<w; j++) {
			//8888 b|g|r|a -> 565 r|g|b
			*pdstdot = 
			     (((psrcdot[2] >> 3) & 0x1F) << 11) //r
				|(((psrcdot[1] >> 2) & 0x3F) <<  5) //g
			    |(((psrcdot[0] >> 3) & 0x1F) <<  0);//b
			
			psrcdot += 4;
			pdstdot++;
		}
		psrcline += srclinesize;
		pdstline += dstlinesize;
	}
	
	return 0;
}

static int bgr888_to_bgra8888(const void * psrc, int w, int h, void * pdst)
{
	int srclinesize = UpAlign4(w * 3);
	int dstlinesize = UpAlign4(w * 4);

	const unsigned char * psrcline;
	const unsigned char * psrcdot;
	unsigned char  * pdstline;
	unsigned char  * pdstdot;

	int i,j;

	if (!psrc || !pdst || w <= 0 || h <= 0) {
		printf("bgr888_to_bgra8888 : parameter error\n");
		return -1;
	}

	psrcline = (const unsigned char *)psrc;
	pdstline = (unsigned char *)pdst;
	for (i=0; i<h; i++) {
		psrcdot = psrcline;
		pdstdot = pdstline;
		for (j=0; j<w; j++) {
			/*b*/*pdstdot++ = *psrcdot++;//b
			/*g*/*pdstdot++ = *psrcdot++;//g
			/*r*/*pdstdot++ = *psrcdot++;//r
			/*a*/*pdstdot++ = 0xFF;
		}
		psrcline += srclinesize;
		pdstline += dstlinesize;
	}
	
	return 0;
}

static int bgr888_to_abgr8888(const void * psrc, int w, int h, void * pdst)
{
	int srclinesize = UpAlign4(w * 3);
	int dstlinesize = UpAlign4(w * 4);

	const unsigned char * psrcline;
	const unsigned char * psrcdot;
	unsigned char  * pdstline;
	unsigned char  * pdstdot;

	int i,j;

	if (!psrc || !pdst || w <= 0 || h <= 0) {
		printf("bgr888_to_abgr8888 : parameter error\n");
		return -1;
	}

	psrcline = (const unsigned char *)psrc;
	pdstline = (unsigned char *)pdst;
	for (i=0; i<h; i++) {
		psrcdot = psrcline;
		pdstdot = pdstline;
		for (j=0; j<w; j++) {
			/*a*/*pdstdot++ = 0xFF;
			/*b*/*pdstdot++ = *psrcdot++;//b
			/*g*/*pdstdot++ = *psrcdot++;//g
			/*r*/*pdstdot++ = *psrcdot++;//r
		}
		psrcline += srclinesize;
		pdstline += dstlinesize;
	}
	
	return 0;
}

static int bgra8888_to_bgr888(const void * psrc, int w, int h, void * pdst)
{
	int srclinesize = UpAlign4(w * 4);
	int dstlinesize = UpAlign4(w * 3);
	
	const unsigned char * psrcline;
	const unsigned char * psrcdot;
	unsigned char  * pdstline;
	unsigned char  * pdstdot;
	
	int i,j;
	
	if (!psrc || !pdst || w <= 0 || h <= 0) {
		printf("bgra8888_to_bgr888 : parameter error\n");
		return -1;
	}
	
	psrcline = (const unsigned char *)psrc;
	pdstline = (unsigned char *)pdst;
	for (i=0; i<h; i++) {
		psrcdot = psrcline;
		pdstdot = pdstline;
		for (j=0; j<w; j++) {
			/*b*/*pdstdot++ = *psrcdot++;//b
			/*g*/*pdstdot++ = *psrcdot++;//g
			/*r*/*pdstdot++ = *psrcdot++;//r
			psrcdot++;//a
		}
		psrcline += srclinesize;
		pdstline += dstlinesize;
	}
	
	return 0;
}

static int bgra8888_to_abgr8888(const void * psrc, int w, int h, void * pdst)
{
	int srclinesize = UpAlign4(w * 4);
	int dstlinesize = UpAlign4(w * 4);
	
	const unsigned char * psrcline;
	const unsigned char * psrcdot;
	unsigned char  * pdstline;
	unsigned char  * pdstdot;
	
	int i,j;
	
	if (!psrc || !pdst || w <= 0 || h <= 0) {
		printf("bgra8888_to_abgr8888 : parameter error\n");
		return -1;
	}
	
	psrcline = (const unsigned char *)psrc;
	pdstline = (unsigned char *)pdst;
	for (i=0; i<h; i++) {
		psrcdot = psrcline;
		pdstdot = pdstline;

		for (j=0; j<w; j++) {
			/*a*/pdstdot[0] = psrcdot[3];//a
			/*b*/pdstdot[1] = psrcdot[0];//b
			/*g*/pdstdot[2] = psrcdot[1];//g
			/*r*/pdstdot[3] = psrcdot[2];//r

			psrcdot+=4;
			pdstdot+=4;
			
		}
		psrcline += srclinesize;
		pdstline += dstlinesize;
	}
	
	return 0;
}

static int abgr8888_to_bgra8888(const void * psrc, int w, int h, void * pdst)
{
	int srclinesize = UpAlign4(w * 4);
	int dstlinesize = UpAlign4(w * 4);
	
	const unsigned char * psrcline;
	const unsigned char * psrcdot;
	unsigned char  * pdstline;
	unsigned char  * pdstdot;
	
	int i,j;
	
	if (!psrc || !pdst || w <= 0 || h <= 0) {
		printf("abgr8888_to_bgra8888 : parameter error\n");
		return -1;
	}
	
	psrcline = (const unsigned char *)psrc;
	pdstline = (unsigned char *)pdst;
	for (i=0; i<h; i++) {
		psrcdot = psrcline;
		pdstdot = pdstline;

		for (j=0; j<w; j++) {
			/*b*/pdstdot[0] = psrcdot[1];//b
			/*g*/pdstdot[1] = psrcdot[2];//g
			/*r*/pdstdot[2] = psrcdot[3];//r
			/*a*/pdstdot[3] = psrcdot[0];//a

			psrcdot+=4;
			pdstdot+=4;
			
		}
		psrcline += srclinesize;
		pdstline += dstlinesize;
	}
	
	return 0;
}

void * rgb565_to_rgb888_buffer(const void * psrc, int w, int h)
{
	int size = h * UpAlign4(w * 3);
	void * pdst = NULL;
	if (psrc && w > 0 && h > 0) {
		pdst = malloc(size);
		if (pdst) {
			if (rgb565_to_rgb888(psrc, w, h, pdst)) {
				free(pdst);
				pdst = NULL;
			}
		}
	}
	return pdst;
}

void * rgb888_to_rgb565_buffer(const void * psrc, int w, int h)
{
	int size = h * UpAlign4(w * 2);
	void * pdst = NULL;
	if (psrc && w > 0 && h > 0) {
		pdst = malloc(size);
		if (pdst) {
			if (rgb888_to_rgb565(psrc, w, h, pdst)) {
				free(pdst);
				pdst = NULL;
			}
		}
	}
	return pdst;
}

void * rgb565_to_bgra8888_buffer(const void * psrc, int w, int h)
{
	int size = h * UpAlign4(w * 4);
	void * pdst = NULL;
	if (psrc && w > 0 && h > 0) {
		pdst = malloc(size);
		if (pdst) {
			if (rgb565_to_bgra8888(psrc, w, h, pdst)) {
				free(pdst);
				pdst = NULL;
			}
		}
	}
	return pdst;
}

void * bgr565_to_abgr8888_buffer(const void * psrc, int w, int h)
{
	int size = h * UpAlign4(w * 4);
	void * pdst = NULL;
	if (psrc && w > 0 && h > 0) {
		pdst = malloc(size);
		if (pdst) {
			if (bgr565_to_abgr8888(psrc, w, h, pdst)) {
				free(pdst);
				pdst = NULL;
			}
		}
	}
	return pdst;
}

void * bgra8888_to_rgb565_buffer(const void * psrc, int w, int h)
{
	int size = h * UpAlign4(w * 2);
	void * pdst = NULL;
	if (psrc && w > 0 && h > 0) {
		pdst = malloc(size);
		if (pdst) {
			if (bgra8888_to_rgb565(psrc, w, h, pdst)) {
				free(pdst);
				pdst = NULL;
			}
		}
	}
	return pdst;
}

void * bgr888_to_bgra8888_buffer(const void * psrc, int w, int h)
{
	int size = h * UpAlign4(w * 4);
	void * pdst = NULL;
	if (psrc && w > 0 && h > 0) {
		pdst = malloc(size);
		if (pdst) {
			if (bgr888_to_bgra8888(psrc, w, h, pdst)) {
				free(pdst);
				pdst = NULL;
			}
		}
	}
	return pdst;
}

void * bgr888_to_abgr8888_buffer(const void * psrc, int w, int h)
{
	int size = h * UpAlign4(w * 4);
	void * pdst = NULL;
	if (psrc && w > 0 && h > 0) {
		pdst = malloc(size);
		if (pdst) {
			if (bgr888_to_abgr8888(psrc, w, h, pdst)) {
				free(pdst);
				pdst = NULL;
			}
		}
	}
	return pdst;
}

void * bgra8888_to_bgr888_buffer(const void * psrc, int w, int h)
{
	int size = h * UpAlign4(w * 3);
	void * pdst = NULL;
	if (psrc && w > 0 && h > 0) {
		pdst = malloc(size);
		if (pdst) {
			if (bgra8888_to_bgr888(psrc, w, h, pdst)) {
				free(pdst);
				pdst = NULL;
			}
		}
	}
	return pdst;
}

void * bgra8888_to_abgr8888_buffer(const void * psrc, int w, int h)
{
	int size = h * UpAlign4(w * 4);
	void * pdst = NULL;
	if (psrc && w > 0 && h > 0) {
		pdst = malloc(size);
		if (pdst) {
			if (bgra8888_to_abgr8888(psrc, w, h, pdst)) {
				free(pdst);
				pdst = NULL;
			}
		}
	}
	return pdst;
}

void * abgr8888_to_bgra8888_buffer(const void * psrc, int w, int h)
{
	int size = h * UpAlign4(w * 4);
	void * pdst = NULL;
	if (psrc && w > 0 && h > 0) {
		pdst = malloc(size);
		if (pdst) {
			if (abgr8888_to_bgra8888(psrc, w, h, pdst)) {
				free(pdst);
				pdst = NULL;
			}
		}
	}
	return pdst;
}

static const RGB_CONVERT_FUN g_convert_func[3][3] = 
{
	{NULL, rgb565_to_rgb888_buffer, bgr565_to_abgr8888_buffer},
	{rgb888_to_rgb565_buffer, NULL, bgr888_to_abgr8888_buffer},
	{bgra8888_to_rgb565_buffer, bgra8888_to_bgr888_buffer, bgra8888_to_abgr8888_buffer}
};

RGB_CONVERT_FUN get_convert_func(int frombpp, int tobpp)
{
	RGB_CONVERT_FUN func_ptr = NULL;
	frombpp = UpAlign8(frombpp) / 8 - 2;
	tobpp = UpAlign8(tobpp) / 8 - 2;
	if ((frombpp >= 0 && frombpp <= 2)
		&& (tobpp >= 0 && tobpp <= 2)) {
		func_ptr = g_convert_func[frombpp][tobpp];
	}
	return func_ptr;
}

//-------------------------------------------------------------------
