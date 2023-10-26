package com.example.androidappiot;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;


import com.example.androidappiot.network.UDPSocketListener;
import com.example.androidappiot.network.UdpThread;

import java.util.concurrent.ArrayBlockingQueue;

public class MainActivity extends AppCompatActivity {
    private Button send_btn;
    private EditText ip_text_input;
    private String port;
    private UDPSocketListener udpSocketListener;
    private final static int key = 12341;
    private final static String DATA_HEADER = "DMST:";
    private Thread udpListenerThread;

    private ArrayBlockingQueue<String> arrayBlockingQueue = new ArrayBlockingQueue<>(10);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        send_btn = findViewById(R.id.send);
        ip_text_input = findViewById(R.id.edit_text);

        this.port = "10000";
        this.udpSocketListener = new UDPSocketListener() {
            @Override
            public void handle(String data) {
                handleCommunication(data);
            }
        };

        initEvents();
    }

    private void initEvents(){
        //Buttons
        initCommunication();
    }

    private void initCommunication() {
        send_btn.setOnClickListener(v -> {
            arrayBlockingQueue.add(DATA_HEADER + encrypt("getValues()", key));
            UdpThread thread = new UdpThread(arrayBlockingQueue, ip_text_input.getText().toString(), port);
            thread.start();
            udpListenerThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    udpSocketListener.startListening(10000);
                }
            });
            udpListenerThread.start();
        });
    }

    public static String encrypt(String text, int key) {
        StringBuilder encryptedText = new StringBuilder();
        for (int i = 0; i < text.length(); i++) {
            char c = text.charAt(i);
            char encryptedChar = (char) (c + key);
            encryptedText.append(encryptedChar);
        }
        return encryptedText.toString();
    }

    public static String decrypt(String text, int key) {
        StringBuilder decryptedText = new StringBuilder();
        for (int i = 0; i < text.length(); i++) {
            char c = text.charAt(i);
            char decryptedChar = (char) (c - key);
            decryptedText.append(decryptedChar);
        }
        return decryptedText.toString();
    }

    private void handleCommunication(String data) {
        if (!(data != null && data.startsWith(DATA_HEADER))) {
            System.out.println("received message did not have good header: " + data);
            return;
        }
        String decryptedData = decrypt(data.substring(DATA_HEADER.length()), key);
        String[] parts = decryptedData.split(";");

        TextView temperatureTextView = findViewById(R.id.temperature);
        TextView lumTextView = findViewById(R.id.luminosite);
        //TODO do this in handler to ask main thread
        temperatureTextView.setText(parts[0]);
        lumTextView.setText(parts[1]);

        udpSocketListener.stopListening();
        udpListenerThread.interrupt();
    }
}