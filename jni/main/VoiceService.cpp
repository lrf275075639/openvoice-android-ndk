#include <stdio.h>
#include <sys/prctl.h>

#include "VoiceService.h"
#include "audio_recorder.h"

#ifdef USB_AUDIO_DEVICE
#warning "=============================USB_AUDIO_DEVICE==============================="
#endif

VoiceService::VoiceService() {
    _voice_config = make_shared<VoiceConfig>();
    _callback = make_shared<VoiceCallback>();
    _speech = Speech::new_instance();
    clear();
}

bool VoiceService::init() {
    std::lock_guard<std::mutex> lg(siren_mutex);
    if (mCurrentSirenState == SIREN_STATE_UNKNOWN) {
        if (!setup(this, [](void *token, voice_event_t *event) {((VoiceService*)token)->voice_event_callback(event);})) {
            LOGE("init siren failed.");
            return false;
        }
    } else {
        goto done;
    }
    mCurrentSirenState = SIREN_STATE_INITED;
    std::call_once(start_flag, [this]{event_thread = std::thread(&VoiceService::onEvent, this);});
done:
    return true;
}

void VoiceService::start_siren(const bool isopen) {
    LOGV("%s \t isopen : %d \t mCurrState : %d \t opensiren : %d",
         __FUNCTION__, isopen, mCurrentSirenState, openSiren);
    
    std::lock_guard<std::mutex> lg(siren_mutex);
    if (isopen && (mCurrentSirenState == SIREN_STATE_INITED
                   || mCurrentSirenState == SIREN_STATE_STOPED)) {
        openSiren = true;
#ifdef USB_AUDIO_DEVICE
        if(wait_for_alsa_usb_card()){
#endif
            _start_siren_process_stream();
            mCurrentSirenState = SIREN_STATE_STARTED;
#ifdef USB_AUDIO_DEVICE
        }
#endif
    } else if (!isopen && mCurrentSirenState == SIREN_STATE_STARTED) {
        _stop_siren_process_stream();
        mCurrentSirenState = SIREN_STATE_STOPED;
    }
    if (!isopen && mCurrentSirenState != SIREN_STATE_UNKNOWN) openSiren = false;
}

void VoiceService::set_siren_state(const int32_t state) {
    set_siren_state_change(state);
    LOGV("current_status     >>   %d", state);
}

#ifdef USB_AUDIO_DEVICE
bool VoiceService::wait_for_alsa_usb_card(){
    int index = 0;
    while (index++ < 3) {
        if(find_card("USB-Audio") >= 0) {
            return true;
        }
        usleep(1000 * 100);
    }
    return false;
}
#endif

void VoiceService::network_state_change(const bool connected) {
    LOGV("network_state_change      isconnect  <<%d>>", connected);
    std::lock_guard<std::mutex> lg(speech_mutex);
    if (connected && mCurrentSpeechState != SPEECH_STATE_PREPARED) {
        if(_voice_config->prepare(_speech)){
            clear();
            mCurrentSpeechState = SPEECH_STATE_PREPARED;
            response_thread = std::thread(&VoiceService::onResponse, this);
            response_thread.detach();
            
            std::lock_guard<std::mutex> lg(siren_mutex);
            if (openSiren && (mCurrentSirenState == SIREN_STATE_INITED
                              || mCurrentSirenState == SIREN_STATE_STOPED)) {
#ifdef USB_AUDIO_DEVICE
                if(wait_for_alsa_usb_card()){
#endif
                    _start_siren_process_stream();
                    mCurrentSirenState = SIREN_STATE_STARTED;
#ifdef USB_AUDIO_DEVICE
                }
#endif
            }
        }
    } else if (!connected && mCurrentSpeechState == SPEECH_STATE_PREPARED) {
        std::lock_guard<std::mutex> lg(siren_mutex);
        if (mCurrentSirenState == SIREN_STATE_STARTED) {
            _stop_siren_process_stream();
            mCurrentSirenState = SIREN_STATE_STOPED;
        }
        LOGV("==========================BEGIN============================");
        _speech->release();
        LOGV("===========================END=============================");
        mCurrentSpeechState = SPEECH_STATE_RELEASED;
    }
}

void VoiceService::update_stack(const string &appid) {
    this->appid = appid;
    LOGE("%s  %s", __FUNCTION__, this->appid.c_str());
}

void VoiceService::update_config(const string& device_id, const string& device_type_id,
                                 const string& key, const string& secret) {
    if(!_voice_config->save_config(device_id, device_type_id, key, secret)){
    }
}

int32_t VoiceService::vad_start() {
    if(mCurrentSpeechState == SPEECH_STATE_PREPARED) {
        VoiceOptions options;
        if(has_vt) {
            options.voice_trigger.assign(vt_word);
            options.trigger_start = vt_start;
            options.trigger_length = vt_end - vt_start;
            options.voice_power = vt_energy;
            has_vt = false;
        }
        options.stack = appid;
        //        options.skill_options = _callback->get_skill_options();
        return _speech->start_voice(&options);
    }
    return -1;
}

void VoiceService::voice_print(const voice_event_t *voice_event) {
    if (voice_event && HAS_VT(voice_event->flag)) {
        vt_start = voice_event->vt.start;
        vt_end = voice_event->vt.end;
        vt_energy = voice_event->vt.energy;
        vt_word = (char*) voice_event->buff;
        has_vt = true;
    }
}

void VoiceService::voice_event_callback(voice_event_t *voice_event) {
    std::lock_guard<std::mutex> lg(event_mutex);
    int32_t len =  0;
    char *buff = nullptr;
    if(voice_event->length > 0){
        len = voice_event->length;
    }
    char *temp = new char[sizeof(voice_event_t) + len];
    if(len) buff = temp + sizeof(voice_event_t);
    
    voice_event_t *_event_t = (voice_event_t*)temp;
    memcpy(_event_t, voice_event, sizeof(voice_event_t));
    
    if ((HAS_VOICE(_event_t->flag) || HAS_VT(_event_t->flag)) && len) {
        memcpy(buff, voice_event->buff, len);
        _event_t->buff = buff;
    }
    _events.push_back(_event_t);
    event_cond.notify_one();
}

void VoiceService::onEvent() {
    prctl(PR_SET_NAME, __FUNCTION__);
    std::unique_lock<std::mutex> lk(event_mutex, std::defer_lock);
    while(true){
        lk.lock();
        event_cond.wait(lk, [this]{return !_events.empty();});
        voice_event_t *_event = _events.front();
        _events.pop_front();
        lk.unlock();
        LOGV("event : -------------------------%d----", _event->event);
        switch(_event->event) {
            case SIREN_EVENT_WAKE_PRE:
                _callback->voice_event(-1, VoiceEvent::VOICE_COMING, _event->sl);
                LOGV("VAD_COMING");
                break;
            case SIREN_EVENT_WAKE_CMD:
                _callback->voice_event(-1, VoiceEvent::VOICE_LOCAL_WAKE, _event->sl);
                LOGV("VAD_CMD");
                break;
            case SIREN_EVENT_VAD_START:
                if(session_id < 0) {
                    session_id = vad_start();
                    _callback->voice_event(session_id, VoiceEvent::VOICE_START);
                    LOGV("VAD_START\t\t ID  :  <<%d>>", session_id);
                }
                break;
            case SIREN_EVENT_VAD_DATA:
                if (session_id > 0 && HAS_VOICE(_event->flag))
                    _speech->put_voice(session_id, (uint8_t *)_event->buff, _event->length);
                break;
            case SIREN_EVENT_VAD_END:
                if(!_voice_config->cloud_vad_enable()){
                    LOGV("VAD_END\t\t ID  :   <<%d>> ", session_id);
                    if(session_id > 0) _speech->end_voice(session_id);
                    clear();
                }
                break;
            case SIREN_EVENT_VAD_CANCEL:
                LOGI("VAD_CANCEL\t\t ID  :   <<%d>> \t  %d", session_id, asr_finished);
                if(session_id > 0 && (!asr_finished || local_sleep)) _speech->cancel(session_id);
                asr_finished = false;
                if(!_voice_config->cloud_vad_enable()) clear();
                break;
            case SIREN_EVENT_VOICE_PRINT:
                voice_print(_event);
                break;
            case SIREN_EVENT_SLEEP:
                local_sleep = true;
                _callback->voice_event(session_id, VoiceEvent::VOICE_LOCAL_SLEEP);
                break;
        }
        delete[] (char *)_event;
    }
    _speech->release();
    _speech.reset();
}

void VoiceService::onResponse() {
    prctl(PR_SET_NAME, __FUNCTION__);
    auto arbitration = [](const string& activation)->bool {return ("fake" == activation || "reject" == activation);};
    SpeechResult sr;
    string activation, asr;
    json_object *nlp_obj, *activation_obj;
    while (true) {
        if (!_speech->poll(sr)) {
            break;
        }
        LOGV("result : id \t %d \t \t type \t %d \t err \t %d", sr.id, sr.type, sr.err);
        if(sr.type == SPEECH_RES_START) {
            asr_finished = false;
            activation.clear();
        } else if((sr.type == SPEECH_RES_INTER || sr.type == SPEECH_RES_END) && !sr.extra.empty()) {
            nlp_obj = json_tokener_parse(sr.extra.c_str());
            if(TRUE == json_object_object_get_ex(nlp_obj, "activation", &activation_obj)){
                activation = json_object_get_string(activation_obj);
                json_object_put(nlp_obj);
                LOGV("result : activ \t %s", activation.c_str());
                _callback->voice_event(sr.id, transform_string_to_event(activation));
                if(arbitration(activation)) {
                    set_siren_state(SIREN_STATE_SLEEP);
                }
            }
        }
        if(!arbitration(activation)) {
            if(sr.type == SPEECH_RES_INTER || sr.type == SPEECH_RES_ASR_FINISH){
                LOGV("result : asr\t%s", sr.asr.c_str());
                _callback->intermediate_result(sr.id, sr.type, sr.asr);
                if(sr.type == SPEECH_RES_ASR_FINISH){
                    if(session_id == sr.id || _voice_config->cloud_vad_enable()){
                        set_siren_state(SIREN_STATE_SLEEP);
                        asr_finished = true;
                    }
                    asr = sr.asr;
                }
            }else if(sr.type == SPEECH_RES_END) {
                LOGV("result : nlp\t%s", sr.nlp.c_str());
                LOGV("result : action\t%s", sr.action.c_str());
                _callback->voice_command(sr.id, asr, sr.nlp, sr.action);
            }else if(sr.type == SPEECH_RES_CANCELLED){
                if(!local_sleep) _callback->voice_event(sr.id, VoiceEvent::VOICE_CANCEL);
                local_sleep = false;
            }else if(sr.type == SPEECH_RES_ERROR && (sr.err != SPEECH_SUCCESS)) {
                if(session_id == sr.id && _voice_config->cloud_vad_enable())
                    set_siren_state(SIREN_STATE_SLEEP);
                _callback->speech_error(sr.id, sr.err);
                asr_finished = false;
                local_sleep = false;
                activation.clear();
            }
        }
        if(sr.type >= SPEECH_RES_END) clear(sr.id);
    }
    LOGV("exit !!");
}
