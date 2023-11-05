package com.irc.iotproject;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.ItemTouchHelper;
import androidx.recyclerview.widget.RecyclerView;

import android.graphics.Typeface;
import android.media.metrics.PlaybackStateEvent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.Looper;
import android.provider.Settings;
import android.text.SpannableString;
import android.text.style.StyleSpan;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.irc.iotproject.model.CustomAdapter;
import com.irc.iotproject.model.UnitItem;
import com.irc.iotproject.network.ListenerThread;
import com.irc.iotproject.network.SendThread;
import com.irc.iotproject.utils.MessageUtils;
import com.irc.iotproject.utils.PropertiesUtils;

import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ArrayBlockingQueue;

public class MainActivity extends AppCompatActivity {
    private RecyclerView recyclerView;
    private CustomAdapter recyclerAdapter;
    private EditText ip;
    private EditText port;
    private Button loopBtn;
    private TextView lastUpdate;
    private ArrayBlockingQueue<String> messageToSendBlockingQueue = new ArrayBlockingQueue<>(10);
    private ArrayBlockingQueue<String> receivedMessageBlockingQueue = new ArrayBlockingQueue<>(1);
    private List<UnitItem> unitItems;
    private UnitItem brightnessItem;
    private UnitItem temperatureItem;
    private CountDownTimer countDownTimer;
    private CountDownTimer progressbarTimer;
    private boolean isLoopRunning = false;
    private ProgressBar progressBar;
    private TextView test;
    private EditText timing;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Properties loading ..
        PropertiesUtils.loadProperties();

        // Input init
        recyclerView = findViewById(R.id.recycler_view);
        ip = findViewById(R.id.ip);
        port = findViewById(R.id.port);
        loopBtn = findViewById(R.id.connect);
        lastUpdate = findViewById(R.id.lastUpdateTime);
        progressBar = findViewById(R.id.progressBar);
        timing = findViewById(R.id.timing);

        test = findViewById(R.id.textdetest);



        initRecyclerView();
        initButton();
    }

    private void initButton() {

        // Loop Button
       loopBtn.setOnClickListener(v -> {
           if (!isLoopRunning) {
               startLoopForNSeconds(Integer.parseInt(timing.getText().toString()));

               isLoopRunning = true;

               loopBtn.setText("Stop");
           } else {
               stopLoop();

               loopBtn.setText("Start");
           }
       });
    }


    private void sendMessage(String message){
        messageToSendBlockingQueue.add(MessageUtils.buildMessageWithHeader(message));
        String ip = this.ip.getText().toString();
        String port = this.port.getText().toString();
        SendThread thread = new SendThread(messageToSendBlockingQueue, ip, port);
        thread.start();
    }

    private void handleReceiveMessage(){
        // Waiting the response
        String message = null;
        try {
            message = receivedMessageBlockingQueue.take();
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }


        String rawMessage = MessageUtils.getRawMessage(message);
        Map<String, String> dataset = MessageUtils.getDatasetFromRawMessage(rawMessage);
        temperatureItem.setValue(Double.parseDouble(dataset.get("temperature")));
        brightnessItem.setValue(Double.parseDouble(dataset.get("brightness")));

        // Last update timestamp
        Date date = new Date(Long.parseLong(dataset.get("timestamp")));
        LocalDateTime localDateTime = date.toInstant().atZone(ZoneId.systemDefault()).toLocalDateTime();
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
        String dateTimeFormatted = localDateTime.format(formatter);

        SpannableString spanString = new SpannableString("Last Update : " +dateTimeFormatted);
        spanString.setSpan(new StyleSpan(Typeface.ITALIC), 0, spanString.length(), 0);
        lastUpdate.setText(spanString);

        recyclerAdapter.notifyDataSetChanged();
    }

    private void startLoopForNSeconds(int n) {
        int second = n * 1000;

        // Progress bar Timer
        progressbarTimer = new CountDownTimer(second, 10){
            @Override
            public void onTick(long millisUntilFinished) {
                long secondsRemaining = millisUntilFinished / 1000;
                int progress = (int) (100 * millisUntilFinished / second);
                progressBar.setProgress(progress);
            }

            @Override
            public void onFinish() {
                progressBar.setProgress(0);
            }
        };

        // Loop Timer
        countDownTimer = new CountDownTimer(10000000, second) {
            @Override
            public void onTick(long millisUntilFinished) {
                long secondsRemaining = millisUntilFinished / 1000;

                progressBar.setProgress(100);
                progressbarTimer.start();

                ListenerThread listenerThread = new ListenerThread(Integer.parseInt(System.getProperty("listened_port")),receivedMessageBlockingQueue);
                listenerThread.start();

                sendMessage("getValues()");

                handleReceiveMessage();
            }

            @Override
            public void onFinish() {
                loopBtn.setText("Start");
            }
        };

        progressbarTimer.start();
        countDownTimer.start();
    }

    private void stopLoop() {
        countDownTimer.cancel();
        progressbarTimer.cancel();
        isLoopRunning = false;
    }

    private void initRecyclerView(){
        // Units Initialization
        brightnessItem = new UnitItem("Brightness", "lx", (double) 0, R.drawable.light, "L");
        temperatureItem = new UnitItem("Temperature", "°C", (double) 0, R.drawable.thermometer, "T");

        unitItems = new ArrayList<>();
        unitItems.add(brightnessItem);
        unitItems.add(temperatureItem);

        recyclerAdapter = new CustomAdapter(unitItems);
        recyclerView.setAdapter(recyclerAdapter);

        // Divider
        DividerItemDecoration dividerItemDecoration = new DividerItemDecoration(this, DividerItemDecoration.VERTICAL);
        recyclerView.addItemDecoration(dividerItemDecoration);

        // Drag & Drop
        ItemTouchHelper itemTouchHelper = new ItemTouchHelper(simpleCallback);
        itemTouchHelper.attachToRecyclerView(recyclerView);
    }


    ItemTouchHelper.SimpleCallback simpleCallback = new ItemTouchHelper.SimpleCallback(ItemTouchHelper.UP | ItemTouchHelper.DOWN | ItemTouchHelper.START | ItemTouchHelper.END, 0) {
        @Override
        public boolean onMove(@NonNull RecyclerView recyclerView, @NonNull RecyclerView.ViewHolder viewHolder, @NonNull RecyclerView.ViewHolder target) {

            int fromPosition = viewHolder.getAdapterPosition();
            int toPosition = target.getAdapterPosition();
            Collections.swap(unitItems, fromPosition, toPosition);
            recyclerView.getAdapter().notifyItemMoved(fromPosition, toPosition);

            String order = "";
            for (UnitItem item : unitItems) {
                order += item.getAlias();
            }

            sendMessage(order);
            // TODO send this order
            test.setText("Send to passerelle : "+order);

            return false;
        }

        @Override
        public void onSwiped(@NonNull RecyclerView.ViewHolder viewHolder, int direction) {

        }
    };
}

