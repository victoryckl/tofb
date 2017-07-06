/********************************************************************
	created:	2012/04/07
	filename: 	bmp_to_screen.c
	author:		
	
	purpose:	
*********************************************************************/

//-------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./myfb.h"
#include "./mybmp.h"
#include "./getopt.h"

#define MAX_PATH 512
enum {
	eFile = 0,
	eFb   = 1,
	eAction_file_to_file = (eFile << 1) | eFile,// 00
	eAction_file_to_fb   = (eFile << 1) | eFb  ,// 01
	eAction_fb_to_file   = (eFb   << 1) | eFile,// 10
												// 11 fb to fb 不支持
};

struct option_info{
	int  action;
	int  output_rgb;
	char input_file[MAX_PATH];
	char output_file[MAX_PATH];
};

static int check_rgb(const char * rgbstr)
{
	int rgb = eRGB_MIN;
	if (!stricmp(rgbstr, g_rgbstr[eRGB_565])){
		rgb = eRGB_565;
	} else if (!stricmp(rgbstr, g_rgbstr[eRGB_888])) {
		rgb = eRGB_888;
	} else if (!stricmp(rgbstr, g_rgbstr[eRGBX_8888])) {
		rgb = eRGBX_8888;
	} else {
		printf("check_rgb(%s) not support rgb format\n", rgbstr);
	}
	return rgb;
}

static void usage(void)
{
	printf("\nUsage: tofb -i bmp | fb   -o bmp | fb  [-r rgbxxx]\n");
	printf("Options:\n");
	printf("  -i bmp or fb    input bmp or fb\n");
	printf("  -o bmp or fb    output bmp or fb\n");
	printf("       bmp or fb support format: %s, %s, %s\n",
		g_rgbstr[eRGB_565], g_rgbstr[eRGB_888], g_rgbstr[eRGBX_8888]);
	printf("  -r rgbxxx        the format of output bmp\n");
	printf("  -h               display this information\n");
	printf("  -v               output version information\n");
}

static void version(void)
{
	printf("\ntofb V1.0.0.2017.07.06.02\n");
	printf("\nThis is free software: you are free to change and redistribute it.\n");
	printf("There is NO WARRANTY, to the extent permitted by law.\n");
}


static int check_option(int argc, char * argv[], struct option_info * info)
{
	int ret = 0;
	int c;

	if (argc <= 1) {
		usage();
		return -1;
	}

	memset(info, 0, sizeof(*info));

	ret = 0;
	while (!ret)
    {
		c = getopt (argc, argv, "i:o:r:hv");
		if (c == -1) {
			ret = 0;
			break;
		}
		
		switch (c)
        {
		case 'i':
			strncpy(info->input_file, optarg, sizeof(info->input_file)-1);
			info->action |= stricmp("fb", info->input_file) ? (eFile << 1) : (eFb << 1);
			break;
		case 'o':
			strncpy(info->output_file, optarg, sizeof(info->output_file)-1);
			info->action |= stricmp("fb", info->output_file) ? eFile : eFb;
			break;
		case 'r':
			info->output_rgb = check_rgb(optarg);
			if (eRGB_MIN == info->output_rgb) {
				ret = -1;
			}
			break;
		case 'v':
			version();
			ret = -1;
			break;
		case 'h':
        case '?':
			usage();
			ret = -1;
			break;
        default:
			printf ("?? getopt returned character code 0x%x ??\n", c);
			usage();
			ret = -1;
			break;
        }
    }
	
	if (info->output_rgb == eRGB_MIN) {
		info->output_rgb = eRGB_565;//默认输出为565格式
	}

	//如果只有两个参数，而且没有前缀，则认为argv[1]为输入图片，并且将图片打印到fb中
	if ((argc == 2) && (optind < argc)) {
		info->action = eAction_file_to_fb;
		strncpy(info->input_file, argv[1], sizeof(info->input_file)-1);
		return 0;
	}
	//还有多余参数，则认为非法，要退出
	if (optind < argc) {
		printf ("non-option ARGV-elements: ");
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		printf ("\n");
		usage();
		return -1;
	}

	if (ret) {
		return ret;
	}

	ret = -1;
	do 
	{
		if (info->action == eAction_file_to_file
			|| info->action == eAction_file_to_fb
			|| info->action == eAction_fb_to_file)
		{
			//do nothing
		} else {
			printf("check_option : unknown action type!\n");
			break;
		}
		ret = 0;
	} while (0);

	return ret;
}

static int file_to_file(const char * srcpath, const char * dstpath, int output_rgb)
{
	int ret = -1;
	BMP_READ * bmp = NULL;
	int w, h;
	int srcbpp, dstbpp;
	void * pdata = NULL, * bmpdata = NULL;
	RGB_CONVERT_FUN convert_func = NULL;

	do 
	{
		bmp = bmp_open(srcpath);
		if (!bmp) {
			break;
		}
		
		w = bmp_width(bmp);
		h = bmp_height(bmp);
		bmpdata = bmp_data(bmp);
		srcbpp = bmp_bpp(bmp);
		dstbpp = g_rgbbpp[output_rgb];

		printf("bmp to fb, %dx%d,%d=>%d,%s=>%s\n", w,h,srcbpp,dstbpp,srcpath,dstpath);
		convert_func = get_convert_func(srcbpp, dstbpp);
		if (convert_func) {
			pdata = convert_func(bmpdata, w, h);
			bmpdata = pdata;
		}

		if (!bmp_forward(bmp)) {
			line_reversal(bmpdata, w, h, dstbpp);
		}
		
		ret = save_bmp(dstpath, w, h, bmpdata, dstbpp);
	} while (0);

	bmp_close(bmp);
	if (pdata) {
		free(pdata);
	}
	return ret;	
}

static int file_to_fb(const char * srcpath)
{
	int ret = -1;
	BMP_READ * bmp = NULL;
	struct FB * fb = NULL;
	int sw, sh;
	int srcbpp, dstbpp;
	void * pdata = NULL, * bmpdata = NULL;
	RGB_CONVERT_FUN convert_func = NULL;
	
	do 
	{
		bmp = bmp_open(srcpath);
		if (!bmp) {
			break;
		}
		fb = fb_create(0);
		if (!fb) {
			break;
		}
		
		sw = bmp_width(bmp);
		sh = bmp_height(bmp);
		bmpdata = bmp_data(bmp);
		srcbpp = bmp_bpp(bmp);
		dstbpp = fb_bpp(fb);
		
		printf("bmp to fb, %dx%d,%d=>%d,%s\n", sw,sh,srcbpp,dstbpp,srcpath);
		if (srcbpp==32 && dstbpp==32) {
			convert_func = abgr8888_to_bgra8888_buffer;
		} else if (srcbpp==24 && dstbpp==32) {
			convert_func = bgr888_to_bgra8888_buffer;
		} else if (srcbpp==16 && dstbpp==32) {
			convert_func = rgb565_to_bgra8888_buffer;
		} else {
			convert_func = get_convert_func(srcbpp, dstbpp);
		}
		
		if (convert_func) {
			pdata = convert_func(bmpdata, sw, sh);
			bmpdata = pdata;
		}
		
		if (!bmp_forward(bmp)) {
			line_reversal(bmpdata, sw, sh, dstbpp);
		}
		
		rgb_copy(bmpdata, fb_bits(fb), sw, sh, fb_width(fb), fb_height(fb), dstbpp);
		ret = 0;
	} while (0);
	
	fb_destory(fb);
	bmp_close(bmp);
	if (pdata) {
		free(pdata);
	}
	return ret;	
}

static int fb_to_file(const char * dstpath, int output_rgb)
{
	int ret = -1;
	int w, h;
	int srcbpp, dstbpp;
	void * pdata = NULL, * fbdata = NULL;
	RGB_CONVERT_FUN convert_func = NULL;

	struct FB * fb = NULL;

	do 
	{
		fb = fb_create(1);
		if (!fb) {
			break;
		}

		w = fb_width(fb);
		h = fb_height(fb);
		fbdata = fb_bits(fb);
		srcbpp = fb_bpp(fb);
		dstbpp = g_rgbbpp[output_rgb];
		
		printf("fb to bmp, %dx%d,%d=>%d,%s\n", w,h,srcbpp,dstbpp,dstpath);
		convert_func = get_convert_func(srcbpp, dstbpp);
		if (convert_func) {
			pdata = convert_func(fbdata, w, h);
			fbdata = pdata;
		}

		ret = save_bmp(dstpath, w, h, fbdata, dstbpp);
	} while (0);

	if (pdata) {
		free(pdata);
	}
	fb_destory(fb);
	
	return ret;
}

static struct option_info g_info = {0};

int tofb(int argc, char * argv[])
{
	int ret = -1;
	char path[256] = ROOT_PATH"/in.bmp";

	ret = check_option(argc, argv, &g_info);
	if (ret) {
		return ret;
	}

	switch(g_info.action)
	{
	case eAction_file_to_file:
		ret = file_to_file(g_info.input_file, g_info.output_file, g_info.output_rgb);
		break;
	case eAction_file_to_fb:
		ret = file_to_fb(g_info.input_file);
		break;
	case eAction_fb_to_file:
		ret = fb_to_file(g_info.output_file, g_info.output_rgb);
		break;
	default:
		break;
	}

	return ret;
}

//-------------------------------------------------------------------
