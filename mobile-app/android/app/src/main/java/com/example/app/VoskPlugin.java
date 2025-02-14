package com.example.app;

import android.content.pm.PackageManager;

import org.json.JSONException;
import org.json.JSONObject;
import org.vosk.Model;
import org.vosk.Recognizer;
import org.vosk.android.StorageService;

import com.getcapacitor.Plugin;
import com.getcapacitor.PluginCall;
import com.getcapacitor.PluginMethod;
import com.getcapacitor.JSObject;
import com.getcapacitor.annotation.CapacitorPlugin;

import java.io.IOException;

import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.media.AudioFormat;
import android.util.Log;

import androidx.core.app.ActivityCompat;


@CapacitorPlugin(name = "VoskPlugin")
public class VoskPlugin extends Plugin {

    private Model model;
    private Recognizer recognizer;
    private boolean isListening = false;

    @Override
    public void load() {
        super.load();
        Log.i("VoskPlugin", "Plugin loaded and model initialization started.");
        initModel();
    }

    @PluginMethod
    public void startRecognition(PluginCall call) {
        if (ActivityCompat.checkSelfPermission(getContext(), android.Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(getActivity(), new String[]{android.Manifest.permission.RECORD_AUDIO}, 1);
            call.reject("Audio recording permission is required.");
            return;
        }

        if (model == null) {
            call.reject("Model not initialized yet. Please wait and try again.");
            return;
        }

        try {
            recognizer = new Recognizer(model, 16000.0f);

            // Avvia l'acquisizione audio in un thread separato
            isListening = true;
            new Thread(this::startAudioCapture).start();

            // Rispondi al JavaScript con un successo
            JSObject response = new JSObject();
            response.put("status", "listening");
            call.resolve(response);

        } catch (IOException e) {
            call.reject("Errore durante il riconoscimento", e);
        }
    }

    @PluginMethod
    public void stopRecognition(PluginCall call) {
        isListening = false;
        JSObject response = new JSObject();
        response.put("status", "stopped");
        call.resolve(response);
    }

    @PluginMethod
    public void isListening(PluginCall call) {
        JSObject response = new JSObject();
        response.put("isListening", isListening);
        call.resolve(response);
    }

    private void startAudioCapture() {
        int sampleRate = 16000;
        int bufferSize = AudioRecord.getMinBufferSize(
            sampleRate,
            AudioFormat.CHANNEL_IN_MONO,
            AudioFormat.ENCODING_PCM_16BIT
        );

        if (ActivityCompat.checkSelfPermission(getContext(), android.Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
            return;
        }
        AudioRecord audioRecord = new AudioRecord(
            MediaRecorder.AudioSource.MIC,
            sampleRate,
            AudioFormat.CHANNEL_IN_MONO,
            AudioFormat.ENCODING_PCM_16BIT,
            bufferSize
        );

        audioRecord.startRecording();
        byte[] buffer = new byte[bufferSize];

        while (isListening) {
            int bytesRead = audioRecord.read(buffer, 0, buffer.length);
            if (bytesRead > 0) {
                if (recognizer.acceptWaveForm(buffer, bytesRead)) {
                    // Ottieni e notifica il risultato finale
                    String finalResult = recognizer.getResult();
                    try {
                        JSONObject obj = new JSONObject(finalResult);
                        Log.i("VoskPlugin", "Testo trascritto: " + obj.getString("text"));
                        audioRecord.stop();
                        audioRecord.release();
                    } catch (JSONException e) {
                        Log.e(TAG, "startAudioCapture: {}", e);
                    }
                    notifyTranscription(finalResult); // Notifica la frase completa
                }
            }
        }

        audioRecord.stop();
        audioRecord.release();
    }


    private void notifyTranscription(String transcription) {
        JSObject update = new JSObject();
        update.put("transcription", transcription);
        notifyListeners("transcriptionUpdate", update);
    }

    private void initModel() {
        if (this.model != null) {
            Log.i("VoskPlugin", "Model already initialized");
            return;
        }
        StorageService.unpack(getContext(), "model-it", "model",
            (model) -> {
                this.model = model;
                Log.i("VoskPlugin", "Model unpacked successfully");
            },
            (exception) -> Log.e("VoskPlugin", "Failed to unpack the model: " + exception.getMessage()));
    }


}
