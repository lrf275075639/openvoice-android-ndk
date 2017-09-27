#include <jni.h>
#include "VoiceService.h"

shared_ptr<VoiceService> service = make_shared<VoiceService>();

//extern JavaVM* _vm;
static jmethodID voice_event;
static jmethodID intermediate_result;
static jmethodID voice_command;
static jmethodID speech_error;

extern "C" {
JNIEXPORT jboolean JNICALL 
        Java_com_rokid_openvoice_VoiceManager_init(JNIEnv *env, jclass);
JNIEXPORT void JNICALL 
        Java_com_rokid_openvoice_VoiceManager_startSiren(JNIEnv *env, jclass, jboolean isopen);
JNIEXPORT void JNICALL 
        Java_com_rokid_openvoice_VoiceManager_setSirenState(JNIEnv *env, jclass, jint state);
JNIEXPORT void JNICALL 
        Java_com_rokid_openvoice_VoiceManager_networkStateChange(JNIEnv *env, jclass, jboolean isconnect);
JNIEXPORT void JNICALL 
        Java_com_rokid_openvoice_VoiceManager_updateStack(JNIEnv *env, jclass, jstring appid);
JNIEXPORT void JNICALL 
        Java_com_rokid_openvoice_VoiceManager_registCallback(JNIEnv *env, jclass, jstring appid, jobject obj);
//jint JNI_OnLoad(JavaVM* vm, void* reserved);
}

JNIEXPORT jboolean JNICALL 
Java_com_rokid_openvoice_VoiceManager_init(JNIEnv *env, jclass)
{
	LOGD("%s", __FUNCTION__);
    jclass callback = env->FindClass("com/rokid/openvoice/VoiceCallback");
    if(!callback){
        LOGI("find class error");
        return false;
    }
    voice_event = env->GetMethodID(callback, "onVoiceEvent", "(IIDD)V");
    intermediate_result = env->GetMethodID(callback, "onIntermediateResult", "(IILjava/lang/String;)V");
    voice_command = env->GetMethodID(callback, "onVoiceCommand", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    speech_error = env->GetMethodID(callback, "onSpeechError", "(II)V");
    return service->init();
}

JNIEXPORT void JNICALL 
Java_com_rokid_openvoice_VoiceManager_startSiren(JNIEnv *env, jclass, jboolean isopen)
{
	LOGD("%s", __FUNCTION__);
	service->start_siren((bool)isopen);
}

JNIEXPORT void JNICALL 
Java_com_rokid_openvoice_VoiceManager_setSirenState(JNIEnv *env, jclass, jint state)
{
	LOGD("%s", __FUNCTION__);
	service->set_siren_state((int)state);
}

JNIEXPORT void JNICALL 
Java_com_rokid_openvoice_VoiceManager_networkStateChange(JNIEnv *env, jclass, jboolean isconnect)
{
	LOGD("%s", __FUNCTION__);
	service->network_state_change((bool)isconnect);
}

JNIEXPORT void JNICALL 
Java_com_rokid_openvoice_VoiceManager_updateStack(JNIEnv *env, jclass, jstring appid)
{
	LOGD("%s", __FUNCTION__);
    service->update_stack(string(env->GetStringUTFChars(appid, NULL)));
}

JNIEXPORT void JNICALL 
Java_com_rokid_openvoice_VoiceManager_registCallback(JNIEnv *env, jclass, jstring appid, jobject obj){
	LOGD("%s      %d   %d", __FUNCTION__, voice_event, obj);
//    env->CallVoidMethod(obj, voice_event, 1);
//    service->regist_callback(jobj);
}

//static JNINativeMethod _options_nmethods[] = {
//	{ "init", "()J", (void*)com_rokid_openvoice_VoiceManager_init },
//	{ "startSiren", "(J)V", (void*)com_rokid_openvoice_VoiceManager_startSiren },
//	{ "setSirenState", "(JLjava/lang/String;)V", (void*)com_rokid_openvoice_VoiceManager_setSirenState },
//	{ "networkStateChange", "(JLjava/lang/String;)V", (void*)com_rokid_openvoice_VoiceManager_networkStateChange },
//	{ "updateStack", "(JLjava/lang/String;)V", (void*)com_rokid_openvoice_VoiceManager_updateStack },
//	{ "registCallback", "(JZ)V", (void*)com_rokid_openvoice_VoiceManager_registCallback },
//};
//
//int register_com_rokid_openvoice_VoiceManager(JNIEnv* env)
//{
//	const char* kclass = "com/rokid/openvoice/VoiceManager";
//	jclass target = env->FindClass(kclass);
//	if (target == NULL) {
//		Log::e("find class for %s failed", kclass);
//		return -1;
//	}
//	return jniRegisterNativeMethods(env, kclass, _options_nmethods, NELEM(_voice_manager_nmethods));
//}
//
//jint JNI_OnLoad(JavaVM* vm, void* reserved)
//{
//	JNIEnv* env;
//
//	// store a global java vm pointer
//	// for voice_manager java callback
//    _vm = vm;
//
//	if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
//		LOGD("%s: JNI_OnLoad failed", "VoiceManager");
//		return -1;
//	}
//    register_com_rokid_openvoice_VoiceManager(env);
//	return JNI_VERSION_1_4;
//}

