package com.rokid.openvoice;

import android.util.Log;

public final class VoiceCallback{

    private String TAG = getClass().getSimpleName();

    private void onVoiceEvent(int id, int event, double sl, double energy){

    }

    private void onIntermediateResult(int id, int type, String asr){

    }

    private void onVoiceCommand(int id, String asr, String nlp, String action){
        Log.e(TAG, "id \t " + id);
        Log.e(TAG, "asr \t " + asr);
        Log.e(TAG, "nlp \t" + nlp);
        Log.e(TAG, "action \t" + action);
    }

    private void onSpeechError(int id, int errcode){

    }
}
