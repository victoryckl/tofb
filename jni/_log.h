#ifdef __cplusplus
extern "C" { 
#endif

#ifndef LOG_TAG
#	define LOG_TAG    "\t\t" // ������Զ����LOG�ı�ʶ
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
#	include <android/log.h> // ��������LOG���õ��ĺ������ڵ�·��
	
//Android.mk��Ҫ���LOCAL_LDLIBS := -llog
//# �����������һ��Ļ�������ʾ��__android_log_print δ����
#	define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__) // ����LOG����
#	define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__) // ����LOG����
#	define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__) // ����LOG����
#	define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__) // ����LOG����
#	define LOGV(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__) // ����LOG����
	
#endif //#ifdef WIN32

//���Գ���
void testLog(void);

#ifdef __cplusplus
}; 
#endif