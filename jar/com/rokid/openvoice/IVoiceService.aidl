package com.rokid.openvoice;

import com.rokid.openvoice.IVoiceCallback;

interface IVoiceService {
	
	void setSirenState(int state);
	
	void startSiren(boolean isopen);
	
//	int insertVTWord(String vtWord);
	
//    int deleteVTWord(String word);
    
//    String queryVTWord();
	
	void registCallback(IVoiceCallback callback);
}
