#include <jni.h>
#include <stdarg.h>
#include <assert.h>
#include <pthread.h>
#include <mutex>
#include <list>

#include "json.h"
#include "VoiceService.h"

shared_ptr<VoiceService> voice_service = make_shared<VoiceService>();

JavaVM* _vm;

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
        Java_com_rokid_openvoice_VoiceManager_updateConfig(JNIEnv *env, jclass, 
        jstring device_id, jstring device_type_id, jstring key, jstring secret);
JNIEXPORT void JNICALL 
        Java_com_rokid_openvoice_VoiceManager_registCallback(JNIEnv *env, jclass, jobject obj);
}

class Handle{
public:
    Handle(JNIEnv *env){
        pthread_mutex_init(&mutex, NULL);
	    pthread_cond_init(&cond, NULL);

        jclass callback = env->FindClass("com/rokid/openvoice/VoiceCallback");
        if(callback == NULL){
            LOGI("find class error");
            return;
        }
        callback_method_table[MSG_VOICE_EVENT_ID]
            = env->GetMethodID(callback, "onVoiceEvent", "(IIDD)V");
        callback_method_table[MSG_INTERMEDIATE_RESULT_ID]
            = env->GetMethodID(callback, "onIntermediateResult", "(IILjava/lang/String;)V");
        callback_method_table[MSG_VOICE_COMMAND_ID]
            = env->GetMethodID(callback, "onVoiceCommand", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        callback_method_table[MSG_SPEECH_ERROR_ID]
            = env->GetMethodID(callback, "onSpeechError", "(II)V");
    }

    void start(jobject jobj){
        this->jobj = jobj;
        std::call_once(start_flag, [&]{pthread_create(&thread, NULL, [](void *token)->void*{return ((Handle *)token)->thread_loop();}, this);});
    }

    class Message{
    public:
        Message(){}
        Message(int32_t what, const string& data){
            this->what = what;
            this->data = data;
        }
        int32_t what;
        string data;
    };

    enum{
            MSG_VOICE_EVENT_ID = 0,
            MSG_INTERMEDIATE_RESULT_ID,
            MSG_VOICE_COMMAND_ID,
            MSG_SPEECH_ERROR_ID,
    };

    void send_message(const Message& msg){
        pthread_mutex_lock(&mutex);
        message_queue.push_back(msg);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

private:
    void* thread_loop(){
        _vm->AttachCurrentThread(&_env, NULL);
        while(true){
            pthread_mutex_lock(&mutex);
            while(message_queue.empty()){
                pthread_cond_wait(&cond, &mutex);
            }
            handle_message(message_queue.front());
            message_queue.pop_front();
            pthread_mutex_unlock(&mutex);
        }
        _vm->DetachCurrentThread();
        return NULL;
    }

    void handle_message(Handle::Message& msg){
        json_object *json_obj = json_tokener_parse(msg.data.c_str());
        switch(msg.what){
        case MSG_VOICE_EVENT_ID:{
                json_object *id_obj, *event_obj, *sl_obj, *energy_obj;
                int32_t id, event;
                double sl, energy;
                if(TRUE == json_object_object_get_ex(json_obj, "id", &id_obj))
                    id = json_object_get_int(id_obj);
                if(TRUE == json_object_object_get_ex(json_obj, "event", &event_obj))
                    event = json_object_get_int(event_obj);
                if(TRUE == json_object_object_get_ex(json_obj, "sl", &sl_obj))
                    sl = json_object_get_double(sl_obj);
                if(TRUE == json_object_object_get_ex(json_obj, "energy", &energy_obj))
                    energy = json_object_get_double(energy_obj);
                _env->CallVoidMethod(jobj, callback_method_table[msg.what], id, event, sl, energy);
            }
            break;         
        case MSG_INTERMEDIATE_RESULT_ID:{
                jstring jasr = 0;
                json_object *id_obj, *type_obj, *asr_obj;
                int32_t id, type;
                std::string asr;
                if(TRUE == json_object_object_get_ex(json_obj, "id", &id_obj))
                    id = json_object_get_int(id_obj);
                if(TRUE == json_object_object_get_ex(json_obj, "type", &type_obj))
                    type = json_object_get_int(type_obj);
                if(TRUE == json_object_object_get_ex(json_obj, "asr", &asr_obj))
                     asr = json_object_get_string(asr_obj);
                if(asr.length() > 0) jasr = _env->NewStringUTF(asr.c_str());
                _env->CallVoidMethod(jobj, callback_method_table[msg.what], id, type, jasr);
            }
            break;
        case MSG_VOICE_COMMAND_ID:{
                jstring jasr = 0, jnlp = 0, jaction = 0;
                int32_t id;
                json_object *id_obj, *asr_obj, *nlp_obj, *action_obj;
                std::string asr, nlp, action;
                if(TRUE == json_object_object_get_ex(json_obj, "id", &id_obj))
                    id = json_object_get_int(id_obj);
                if(TRUE == json_object_object_get_ex(json_obj, "asr", &asr_obj))
                    asr = json_object_get_string(asr_obj);
                if(TRUE == json_object_object_get_ex(json_obj, "nlp", &nlp_obj))
                    nlp = json_object_get_string(nlp_obj);
                if(TRUE == json_object_object_get_ex(json_obj, "action", &action_obj))
                    action = json_object_get_string(action_obj);
                if(asr.length() > 0) jasr = _env->NewStringUTF(asr.c_str());
                if(nlp.length() > 0) jnlp = _env->NewStringUTF(nlp.c_str());
                if(action.length() > 0) jaction = _env->NewStringUTF(action.c_str());
                _env->CallVoidMethod(jobj, callback_method_table[msg.what], id, jasr, jnlp, jaction);
            }
            break;
        case MSG_SPEECH_ERROR_ID:{
                json_object *id_obj, *errcode_obj;
                int32_t id, errcode;
                if(TRUE == json_object_object_get_ex(json_obj, "id", &id_obj))
                    id = json_object_get_int(id_obj);
                if(TRUE == json_object_object_get_ex(json_obj, "errcode", &errcode_obj))
                    errcode = json_object_get_int(errcode_obj);
                _env->CallVoidMethod(jobj, callback_method_table[msg.what], id, errcode);
            }
            break;
        }
        json_object_put(json_obj);
    }

    std::list<Message> message_queue;

    pthread_mutex_t mutex;
	pthread_cond_t cond;
    pthread_t thread;

    jmethodID callback_method_table[4];
    JNIEnv *_env;
    jobject jobj;

    std::once_flag start_flag;
};

shared_ptr<Handle> handle;

JNIEXPORT jboolean JNICALL 
Java_com_rokid_openvoice_VoiceManager_init(JNIEnv *env, jclass)
{
	LOGD("%s", __FUNCTION__);
    handle = make_shared<Handle>(env);
    return voice_service->init();
}

JNIEXPORT void JNICALL 
Java_com_rokid_openvoice_VoiceManager_startSiren(JNIEnv *env, jclass, jboolean isopen)
{
	LOGD("%s", __FUNCTION__);
	voice_service->start_siren((bool)isopen);
}

JNIEXPORT void JNICALL 
Java_com_rokid_openvoice_VoiceManager_setSirenState(JNIEnv *env, jclass, jint state)
{
	LOGD("%s", __FUNCTION__);
	voice_service->set_siren_state((int)state);
}

JNIEXPORT void JNICALL 
Java_com_rokid_openvoice_VoiceManager_networkStateChange(JNIEnv *env, jclass, jboolean isconnect)
{
	LOGD("%s", __FUNCTION__);
	voice_service->network_state_change((bool)isconnect);
}

JNIEXPORT void JNICALL 
Java_com_rokid_openvoice_VoiceManager_updateStack(JNIEnv *env, jclass, jstring appid)
{
	LOGD("%s", __FUNCTION__);
    voice_service->update_stack(string(env->GetStringUTFChars(appid, NULL)));
}

JNIEXPORT void JNICALL 
Java_com_rokid_openvoice_VoiceManager_updateConfig(JNIEnv *env, jclass, 
        jstring device_id, jstring device_type_id, jstring key, jstring secret)
{
	LOGD("%s", __FUNCTION__);
    voice_service->update_config(
                string(env->GetStringUTFChars(device_id, NULL)),
                string(env->GetStringUTFChars(device_type_id, NULL)),
                string(env->GetStringUTFChars(key, NULL)),
                string(env->GetStringUTFChars(secret, NULL)));
}

JNIEXPORT void JNICALL 
Java_com_rokid_openvoice_VoiceManager_registCallback(JNIEnv *env, jclass, jobject obj){
	LOGD("%s", __FUNCTION__);
    if(handle.get()) handle->start(env->NewGlobalRef(obj));
    voice_service->regist_callback(
        [&](int32_t method_id, const string& data){if(handle.get()) handle->send_message(Handle::Message(method_id, data));});
}

static JNINativeMethod method_table[] = {
	{ "init", "()Z", (void*)Java_com_rokid_openvoice_VoiceManager_init},
	{ "startSiren", "(Z)V", (void*)Java_com_rokid_openvoice_VoiceManager_startSiren},
	{ "setSirenState", "(I)V", (void*)Java_com_rokid_openvoice_VoiceManager_setSirenState},
	{ "networkStateChange", "(Z)V", (void*)Java_com_rokid_openvoice_VoiceManager_networkStateChange},
	{ "updateStack", "(Ljava/lang/String;)V", (void*)Java_com_rokid_openvoice_VoiceManager_updateStack},
	{ "updateConfig", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", (void*)Java_com_rokid_openvoice_VoiceManager_updateConfig},
	{ "registCallback", "(Lcom/rokid/openvoice/VoiceCallback;)V", (void*)Java_com_rokid_openvoice_VoiceManager_registCallback},
};

int register_com_rokid_openvoice_VoiceManager(JNIEnv* env)
{
	const char* className = "com/rokid/openvoice/VoiceManager";
	jclass target = env->FindClass(className);
	if (target == NULL) {
		LOGD("find class for %s failed", className);
		return -1;
	}
#define NELEM(x)            (sizeof(x)/sizeof(*(x)))
    if (env->RegisterNatives(target, method_table, NELEM(method_table)) < 0) {
        char* msg;
        asprintf(&msg, "RegisterNatives failed for '%s'; aborting...", className);
        env->FatalError(msg);
    }
    return 0;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env;
	// store a global java vm pointer
	// for voice_manager java callback
    _vm = vm;
	if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
		LOGD("%s: JNI_OnLoad failed", "VoiceManager");
		return -1;
	}
    register_com_rokid_openvoice_VoiceManager(env);
	return JNI_VERSION_1_4;
}
