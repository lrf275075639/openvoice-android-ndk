package com.rokid.openvoice;

public abstract class VoiceCallback{
	public abstract void onVoiceEvent(int id, int event, double sl, double energy);
	public abstract void onIntermediateResult(int id, int type, String asr);
	public abstract void onVoiceCommand(int id, String asr, String nlp, String action);
	public abstract void onSpeechError(int id, int errcode);
}
	
