package com.irc.iotproject;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.irc.iotproject.network.UdpThread;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;

public class MainActivity extends AppCompatActivity {
    private Button send_btn;
    private String ip;
    private EditText ip_text_input;
    private String port;
    private ArrayBlockingQueue<String> arrayBlockingQueue = new ArrayBlockingQueue<>(10);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        send_btn = findViewById(R.id.send);
        ip_text_input = findViewById(R.id.edit_text);

        this.port = "10000";

        initEvents();
    }

    private void initEvents(){
        //Buttons
        initCommunication();
    }

    private void initCommunication() {
        send_btn.setOnClickListener(v -> {
            arrayBlockingQueue.add("DMST:getValues()");
            UdpThread thread = new UdpThread(arrayBlockingQueue, ip_text_input.getText().toString(), port);
            thread.start();
        });
    }
}