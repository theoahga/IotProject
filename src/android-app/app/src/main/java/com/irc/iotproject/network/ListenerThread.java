package com.irc.iotproject.network;

import com.irc.iotproject.ThreadListener;

import org.jetbrains.annotations.NotNull;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.util.concurrent.ArrayBlockingQueue;

public class ListenerThread extends Thread{
    private final int port;
    private ArrayBlockingQueue<String> messageToSendBlockingQueue;

    public ListenerThread(@NotNull int port, ArrayBlockingQueue<String> messageToSendBlockingQueue ){
        this.port = port;
        this.messageToSendBlockingQueue = messageToSendBlockingQueue;
    }

    @Override
    public void run() {
        try{
            DatagramSocket socket = new DatagramSocket(port);

            byte[] buffer = new byte[1024];
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

            // Waiting a response
            socket.receive(packet);
            String message = new String(packet.getData(), 0, packet.getLength());
            messageToSendBlockingQueue.put(message);

            socket.close();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }

    }

}
