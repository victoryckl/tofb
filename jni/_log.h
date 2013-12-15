#ifdef __cplusplus
extern "C" { 
#endif

#ifndef LOG_TAG
#	define LOG_TAG    "\t\t" // 这个是自定义的LOG的标识
#endif

#ifdef WIN32

int log_d(const char * format, ...);
int log_i(const char * format, ...);
int log_w(const char * format, ...);
int log_e(const char * format, ...);
int log_v(const char * format, ...);

#	define LOGD	log_d
#	define LOGI	log_i
#	define LOGW	log_w
#	define LOGE	log_e
#	define LOGV	log_v
	
#else // #ifdef WIN32
	
#	include <jni.h>
#	include <android/log.h> // 这个是输出LOG所用到的函数所在的路径
	
//Android.mk中要添加LOCAL_LDLIBS := -llog
//# 如果不包含这一句的话，会提示：__android_log_print 未定义
#	define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__) // 定义LOG类型
#	define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__) // 定义LOG类型
#	define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__) // 定义LOG类型
#	define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__) // 定义LOG类型
#	define LOGV(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__) // 定义LOG类型
	
#endif //#ifdef WIN32

//测试程序
void testLog(void);

#ifdef __cplusplus
}; 
#endif