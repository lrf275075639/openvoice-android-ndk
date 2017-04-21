package com.rokid_openvoice_speech_app;

import android.app.Activity;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import com.rokid.speech.Speech;
import com.rokid.speech.SpeechCallback;

public class MainActivity extends Activity implements SpeechCallback{
	
	private static final String SPEECH_CONFIG_FILE = "file:///android_asset/speech_profile.jaon"; 
	
	private int SOURCE = MediaRecorder.AudioSource.MIC;
    private static int SAMPLE_RETE = 44100;
    private static int CHANNEL = AudioFormat.CHANNEL_IN_STEREO;
    private static int FORMAT = AudioFormat.ENCODING_PCM_16BIT;

    private boolean isRecord = false;
    private int minBufferSize = 0;
    private AudioRecord mAudioRecord;
    Speech mSpeech = null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		mSpeech = new Speech(SPEECH_CONFIG_FILE);
		mSpeech.config("codec", "opu");
		mSpeech.config("vt", "ָפחק");
		creatAudioRecord();
		
	}
	
	public void bt_start(View v){
		new Thread(recordThread).start();
	}
	
	public void bt_end(View v){
		isRecord = false;
	}
	
	public void creatAudioRecord(){
		minBufferSize = AudioRecord.getMinBufferSize(SAMPLE_RETE,CHANNEL, FORMAT);
		mAudioRecord = new AudioRecord(SOURCE, SAMPLE_RETE,CHANNEL, FORMAT, minBufferSize);
	}
	
	Runnable recordThread = new Runnable() {
		
		@Override
		public void run() {
			int size = 0;
			byte[] data = new byte[minBufferSize];
			int id = mSpeech.startVoice(null);
			while (isRecord == true) {
				size = mAudioRecord.read(data, 0, minBufferSize);
				if (AudioRecord.ERROR_INVALID_OPERATION != size) {
					//write to speech
					mSpeech.putVoice(id, data);
				}
			}
			mAudioRecord.stop();
			mSpeech.endVoice(id);
		}
	};
	@Override
	protected void onDestroy() {
		mAudioRecord.release();
		mSpeech.finalize();
	}

	@Override
	public void onAction(int arg0, String arg1) {
		showToast(arg1);
	}

	@Override
	public void onAsr(int arg0, String arg1) {
		showToast(arg1);
	}

	@Override
	public void onCancel(int arg0) {
		
	}

	@Override
	public void onComplete(int arg0) {
		
	}

	@Override
	public void onError(int arg0, int arg1) {
		
	}

	@Override
	public void onNlp(int arg0, String arg1) {
		showToast(arg1);
	}

	@Override
	public void onStart(int arg0) {
		
	}
	
	private void showToast(final String text){
		runOnUiThread(new Runnable() {
			
			@Override
			public void run() {
				Toast.makeText(getApplicationContext(), text, Toast.LENGTH_SHORT).show();
			}
		});
	}
}
