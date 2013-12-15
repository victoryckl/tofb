/********************************************************************
	created:	2012/02/07
	filename: 	myfb.h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef _myfb_h__
#define _myfb_h__
//-------------------------------------------------------------------

#include "./_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UpAlign4(n) (((n) + 3) & ~3)
#define UpAlign8(n) (((n) + 7) & ~7)
#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

enum {
	eRGB_MIN = 0,
	eRGB_565,
	eRGB_888,
	eRGBX_8888,

	eRGB_AUTO,//如果没有设置输出图片的格式，则自动选择与输入匹配的格式
	eRGB_MAX
};

static const char * g_rgbstr[] =
{
	"",
	"rgb565",
	"rgb888",
	"rgbx8888",
};

static const int g_rgbbpp[] = 
{
	0, 16, 24, 32,
};

struct FB;

int fb_bpp(struct FB *fb);
int fb_width(struct FB *fb);
int fb_height(struct FB *fb);
int fb_size(struct FB *fb);
int fb_virtual_size(struct FB *fb);
void * fb_bits(struct FB *fb);
void fb_update(struct FB *fb);

struct FB * fb_create(int readonly);
void fb_destory(struct FB *fb);

typedef struct FBinfo {
	int bpp;
	int width;
	int height;
}FBinfo;
int fb_info(FBinfo * fbinfo);


int bpp_to_type(int frombpp);
const char * type_to_rgbstr(int type);
const char * bpp_to_rgbstr(int frombpp);

#ifdef __cplusplus
};
#endif

//-------------------------------------------------------------------
#endif // #ifndef _myfb_h__
