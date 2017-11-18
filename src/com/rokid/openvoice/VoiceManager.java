package com.rokid.openvoice;

import java.util.ArrayList;

;

public class VoiceManager {

	static {
		System.loadLibrary("openvoice2");
	}

	public static final int SIREN_STATE_AWAKE = 1;
	public static final int SIREN_STATE_SLEEP = 2;
	public static final int SIREN_STATE_STOP = 3;
	public static final int SIREN_STATE_START = 4;

	public static final int VT_TYPE_AWAKE = 1;
	public static final int VT_TYPE_SLEEP = 2;
	public static final int VT_TYPE_HOTWORD = 3;
	public static final int VT_TYPE_OTHER = 4;

	public static native boolean init();

	public static native void startSiren(boolean isopen);

	public static native void setSirenState(int state);

	public static native void networkStateChange(boolean isconnected);

	public static native void updateStack(String appid);

	public static native void updateConfig(String device_id, String device_type_id, String key, String secret);

	public static native void registCallback(VoiceCallback callback);

	public static native int addVTWord(VTWord vt_word);

	public static native int removeVTWord(String vt_word);

	public static native ArrayList getVTWords();

	public static class VTWord {
		public int vt_type;
		public String vt_word;
		public String vt_pinyin;

		public VTWord() {

		}

		public VTWord(int vt_type, String vt_word, String vt_pinyin) {
			this.vt_type = vt_type;
			this.vt_word = vt_word;
			this.vt_pinyin = vt_pinyin;
		}

		@Override
		public String toString() {
			return "VTWord [vt_type=" + vt_type + ", vt_word=" + vt_word
					+ ", vt_pinyin=" + vt_pinyin + "]";
		}
	}
}
