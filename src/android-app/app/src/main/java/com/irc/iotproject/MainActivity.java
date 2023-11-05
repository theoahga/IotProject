package com.irc.iotproject;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.ItemTouchHelper;
import androidx.recyclerview.widget.RecyclerView;

import android.graphics.Typeface;
import android.media.metrics.PlaybackStateEvent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.text.SpannableString;
import android.text.style.StyleSpan;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.irc.iotproject.model.CustomAdapter;
import com.irc.iotproject.model.UnitItem;
import com.irc.iotproject.network.ListenerThread;
import com.irc.iotproject.network.SendThread;
import com.irc.iotproject.utils.MessageUtils;
import com.irc.iotproject.utils.PropertiesUtils;

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
    private Button connectBtn;
    private TextView lastUpdate;
    private ArrayBlockingQueue<String> messageToSendBlockingQueue = new ArrayBlockingQueue<>(10);
    private ArrayBlockingQueue<String> receivedMessageBlockingQueue = new ArrayBlockingQueue<>(10);
    private List<UnitItem> unitItems;
    private UnitItem brightnessItem;
    private UnitItem temperatureItem;

    private TextView test;
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
        connectBtn = findViewById(R.id.connect);
        lastUpdate = findViewById(R.id.lastUpdateTime);

        test = findViewById(R.id.textdetest);



        initRecyclerView();
        initButton();
    }

    private void initButton() {
       connectBtn.setOnClickListener(v -> {
           sendMessage("getValues()");
           handleReceiveMessage();
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
        // Start listener Thread
        ListenerThread listenerThread = new ListenerThread(Integer.parseInt(System.getProperty("listened_port")), receivedMessageBlockingQueue);
        listenerThread.start();

        // Waiting the response
        String message = null;
        try {
            message = receivedMessageBlockingQueue.take();
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }


        String rawMessage = MessageUtils.getRawMessage(message);
        Map<String, Double> dataset = MessageUtils.getDatasetFromRawMessage(rawMessage);
        temperatureItem.setValue(dataset.get("temperature"));
        brightnessItem.setValue(dataset.get("brightness"));

        // Last update timestamp
        Date date = new Date(Integer.parseInt(String.valueOf(dataset.get("timestamp"))));
        SpannableString spanString = new SpannableString(date.toString());
        spanString.setSpan(new StyleSpan(Typeface.ITALIC), 0, spanString.length(), 0);
        lastUpdate.setText(spanString);

        recyclerAdapter.notifyDataSetChanged();
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


            // TODO send this order
            test.setText(order);

            return false;
        }

        @Override
        public void onSwiped(@NonNull RecyclerView.ViewHolder viewHolder, int direction) {

        }
    };
}

