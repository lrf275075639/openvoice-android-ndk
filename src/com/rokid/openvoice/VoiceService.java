package com.rokid.openvoice;

import android.content.Intent;
import android.content.Context;
import android.os.IBinder;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.net.NetworkInfo;
import android.net.ConnectivityManager;
import org.json.JSONObject;
import org.json.JSONException;

public class VoiceService extends android.app.Service{

    String TAG = getClass().getSimpleName();
    
    public static boolean initialized = false;
    
    public static final int SIREN_STATE_AWAKE = 1;
    public static final int SIREN_STATE_SLEEP = 2;
    
    private static final int VOICE_START = 100;
    private static final int VOICE_DATA = 101;
    private static final int VOICE_END = 102;
    private static final int VOICE_CANCEL = 103;
    private static final int VOICE_COMING = 107;
    private static final int VOICE_LOCAL_WAKE_NOCMD = 108;
    private static final int VOICE_LOCAL_WAKE = 109;
    private static final int VOICE_SLEEP = 111;
    private static final int VOICE_AVAILABLE = 200;
    private static final int VOICE_UNAVAILABLE = 201;

	public VoiceService(){
		Log.e(TAG, "VoiceService  created ");
        VoiceManager.init();
        VoiceManager.registCallback(new VoiceCallback());
	}

	@Override
	public void onCreate(){
		ConnectivityManager cm = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo mNetworkInfo = cm.getActiveNetworkInfo();
//		if(mNetworkInfo != null){
		    VoiceManager.networkStateChange(true);
//		}

//		mUEventObserver.startObserving("/sound/card1/pcmC1D0c");
	}

//	private final IVoiceCallback.Stub callback = new IVoiceCallback.Stub(){
//
//		@Override
//		public void onVoiceCommand(String asr, String nlp, String action){
//            
//			Log.e(TAG, "asr\t" + asr);
//
//            String appId = "";
//            try{
//                appId = new JSONObject(nlp).getString("appId");
//            }catch(JSONException e){
//                e.printStackTrace();    
//            }
//            if(appId != null && appId.length() > 0 && !appId.equals("ROKID.EXCEPTION")){
//			    VoiceManager.updateStack(appId + ":");
//            } 
//		}
//	
//		@Override
//        public void onVoiceEvent(int event, boolean has_sl, double sl_degree, double energy, double threshold){}
//
//        @Override
//        public void onArbitration(String extra){}
//
//        @Override
//        public void onSpeechError(int errcode){}
//	};

	private final android.os.UEventObserver mUEventObserver = new android.os.UEventObserver() {
		 
		@Override
		public void onUEvent(android.os.UEventObserver.UEvent event){
			Log.e(TAG, event.toString());
            if(initialized){
    			String action = event.get("ACTION");
    			if("add".equals(action)){
    				VoiceManager.startSiren(true);	
    			}else if("remove".equals(action)){
    				VoiceManager.startSiren(false);
    			}
            }
		}
	};

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
}
