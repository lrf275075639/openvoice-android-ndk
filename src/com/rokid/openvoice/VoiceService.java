package com.rokid.openvoice;

import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.os.Handler;
import android.os.RemoteException;
import android.util.Log;

public class VoiceService extends android.app.Service{

    private String TAG = getClass().getSimpleName();
    private IVoiceCallback callback;
    
    public static final int MSG_VOICE_EVENT             = 0;
    public static final int MSG_INTERMEDIATE_RESULT		= 1;
    public static final int MSG_VOICE_COMMAND           = 2;
    public static final int MSG_SPEECH_ERROR            = 3;
    
    private static final int VOICE_COMING               = 0;
    private static final int VOICE_LOCAL_WAKE           = 1;
    private static final int VOICE_START                = 2;
    private static final int VOICE_ACCEPT               = 3;
    private static final int VOICE_REJECT               = 4;
    private static final int VOICE_CANCEL               = 5;
    private static final int VOICE_LOCAL_SLEEP       	= 6;

    private static final int SPEECH_SERVER_INTERNAL     = 6;
    private static final int SPEECH_VAD_TIMEOUT         = 7;
    private static final int SPEECH_SERVICE_UNAVAILABLE = 101;
    private static final int SPEECH_TIMEOUT             = 103;
    
    public Handler mHandler = new Handler(){
    	public void handleMessage(android.os.Message msg) {
    		switch (msg.what) {
			case MSG_VOICE_EVENT:{
				Bundle data = msg.getData();
				handleVoiceEvent(data.getInt("id"), data.getInt("event"), data.getDouble("sl"), data.getDouble("energy"));
			}
				break;
			case MSG_INTERMEDIATE_RESULT:{
				Bundle data = msg.getData();
				handleIntermediateResult(data.getInt("id"), data.getInt("type"), data.getString("asr"));
			}
				break;
			case MSG_VOICE_COMMAND:{
				Bundle data = msg.getData();
				handleVoiceCommand(data.getInt("id"), data.getString("asr"), data.getString("nlp"), data.getString("action"));
			}
				break;
			case MSG_SPEECH_ERROR:{
				handleSpeechError(msg.arg1, msg.arg2);
			}
				break;
			}
    	}
    };

	public VoiceService(){
		Log.e(TAG, "VoiceService  created ");
        VoiceManager.registCallback(new VoiceCallback(mHandler));
	}

	@Override
	public void onCreate(){
		ConnectivityManager cm = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo mNetworkInfo = cm.getActiveNetworkInfo();
		if(mNetworkInfo != null){
		    VoiceManager.networkStateChange(true);
		}
//		mUEventObserver.startObserving("/sound/card1/pcmC1D0c");
	}
	
	private void handleVoiceEvent(int id, int event, double sl, double energy){
		Intent intent = new Intent("rokid.intent.VoiceEvent");
		intent.putExtra("id", id);
		intent.putExtra("event", event);
		intent.putExtra("sl", sl);
		intent.putExtra("energy", energy);
		sendBroadcast(intent);
//		if(callback != null){
//			try {
//				callback.voiceEvent(id, event, sl, energy);
//			} catch (RemoteException e) {
//				e.printStackTrace();
//			}
//		}
	}
	
	private void handleIntermediateResult(int id, int type, String asr){
		Intent intent = new Intent("rokid.intent.IntermediateResult");
		intent.putExtra("id", id);
		intent.putExtra("type", type);
		intent.putExtra("asr", asr);
		sendBroadcast(intent);
//		if(callback != null){
//			try {
//				callback.intermediateResult(id, type, asr);
//			} catch (RemoteException e) {
//				e.printStackTrace();
//			}
//		}
	}
	
	private void handleVoiceCommand(int id, String asr, String nlp, String action){
		Intent intent = new Intent("rokid.intent.VoiceCommand");
		intent.putExtra("id", id);
		intent.putExtra("asr", asr);
		intent.putExtra("nlp", nlp);
		intent.putExtra("action", action);
		sendBroadcast(intent);
//		if(callback != null){
//			try {
//				callback.voiceCommand(id, asr, nlp, action);
//			} catch (RemoteException e) {
//				e.printStackTrace();
//			}
//		}
	}

	private void handleSpeechError(int id, int errcode){
		Intent intent = new Intent("rokid.intent.SpeechError");
		intent.putExtra("id", id);
		intent.putExtra("errcode", errcode);
		sendBroadcast(intent);
//		if(callback != null){
//			try {
//				callback.speechError(id, errcode);
//			} catch (RemoteException e) {
//				e.printStackTrace();
//			}
//		}
	}

	private final IVoiceService.Stub voice_service = new IVoiceService.Stub() {
		
		@Override
		public void startSiren(boolean isopen) throws RemoteException {
			VoiceManager.startSiren(isopen);
		}
		
		@Override
		public void setSirenState(int state) throws RemoteException {
			VoiceManager.setSirenState(state);
		}
		
		@Override
		public void registCallback(IVoiceCallback callback) throws RemoteException {
			VoiceService.this.callback = callback;
		}
	};
	
//	private final android.os.UEventObserver mUEventObserver = new android.os.UEventObserver() {
//	 
//	@Override
//	public void onUEvent(android.os.UEventObserver.UEvent event){
//		Log.e(TAG, event.toString());
//		String action = event.get("ACTION");
//		if("add".equals(action)){
//			VoiceManager.startSiren(true);	
//		}else if("remove".equals(action)){
//			VoiceManager.startSiren(false);
//		}
//	}
//};
	
	@Override
	public android.os.IBinder onBind(Intent intent) {
		return voice_service;
	}
}
