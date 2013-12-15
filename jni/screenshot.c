/********************************************************************
	created:	2012/02/07
	filename: 	screenshot.c
	author:		
	
	purpose:	
*********************************************************************/

#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "./myfb.h"
#include "./mybmp.h"

int screen_shot(const char * path)
{
	int ret = -1;
	struct FB * fb = NULL;
	fb = fb_create(1);
	if (fb) {
		ret = save_bmp(path, fb_width(fb), fb_height(fb), fb_bits(fb), fb_bpp(fb));
		fb_destory(fb);
	}
	return ret;
}

int shot(int argc, char * argv[])
{
	char path[256] = ROOT_PATH"/shot.bmp";
	if (argc >= 2) {
		strcpy(path, argv[1]);
	}
	return screen_shot(path);
}