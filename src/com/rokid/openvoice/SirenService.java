package com.rokid.openvoice;
/**
 * am startservice -n com.rokid.openvoice/com.rokid.openvoice.SirenService --ei action 1
 */
public class SirenService extends android.app.Service{

    private static final int SIREN_AWAKE = 1;
    private static final int SIREN_SLEEP = 2;

    @Override
    public int onStartCommand(android.content.Intent intent, int flags, int startId) {
        boolean isOpen = intent.getBooleanExtra("status", false);
        android.util.Log.e("VoiceService", "onStartCommand  isOpen : " + isOpen);
        if(VoiceService.initialized){
            if(isOpen){
                VoiceManager.setSirenState(VoiceService.SIREN_STATE_AWAKE);
            }else{
				VoiceManager.setSirenState(VoiceService.SIREN_STATE_SLEEP);
            }
        }
        return super.onStartCommand(intent, flags, startId);
    }

	@Override
	public android.os.IBinder onBind(android.content.Intent intent){
		return null;
	}
}
