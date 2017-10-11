package com.rokid.openvoice;

interface IVoiceCallback {
	
	void voiceEvent(int id, int event, double sl, double energy);
    void intermediateResult(int id, int type, String asr);
    void voiceCommand(int id, String asr, String nlp, String action);
    void speechError(int id, int errcode);
}
