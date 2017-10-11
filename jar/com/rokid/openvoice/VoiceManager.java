package com.rokid.openvoice;

import android.os.IBinder;
import android.os.RemoteException;

import com.rokid.openvoice.IVoiceCallback;
import com.rokid.openvoice.IVoiceService;

public class VoiceManager {

	private static IVoiceService voice_service = null;

    private VoiceManager(){}

	public static VoiceManager asInterface(IBinder binder) {
		if (binder != null && binder.isBinderAlive()) {
			voice_service = IVoiceService.Stub.asInterface(binder);
		}
        return null;
	}

	public void startSiren(boolean isopen) throws RemoteException {
		if(voice_service != null){
			voice_service.startSiren(isopen);
		}
	}

	public void setSirenState(int state) throws RemoteException {
		if(voice_service != null){
			voice_service.setSirenState(state);
		}
	}

	public void registCallback(VoiceCallback callback) throws RemoteException {
		if(voice_service != null){
			voice_service.registCallback(new VoiceCallbackWrap(callback));
		}
	}

	private static class VoiceCallbackWrap extends IVoiceCallback.Stub{
        private VoiceCallback callback = null; 
        public VoiceCallbackWrap(VoiceCallback callback){
            this.callback = callback;
        }
        @Override
		public void voiceEvent(int id, int event, double sl, double energy){
            if(callback != null){
                callback.onVoiceEvent(id, event, sl, energy);
            }
        }
        @Override
		public void intermediateResult(int id, int type, String asr){
            if(callback != null){
                callback.onIntermediateResult(id, type, asr);
            }
        }
        @Override
		public void voiceCommand(int id, String asr, String nlp, String action){
            if(callback != null){
                callback.onVoiceCommand(id, asr, nlp, action);
            }
        }
        @Override
		public void speechError(int id, int errcode){
            if(callback != null){
                callback.onSpeechError(id, errcode);
            }
        }
	}
}
