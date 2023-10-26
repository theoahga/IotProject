package com.example.androidappiot.network;

import java.net.DatagramPacket;
import java.net.DatagramSocket;

public abstract class UDPSocketListener {

    private DatagramSocket socket;
    private boolean isRunning;

    public UDPSocketListener() {
        isRunning = true;
    }

    public void startListening(int port) {
        try {
            socket = new DatagramSocket(port);

            byte[] buffer = new byte[1024];

            while (isRunning) {
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                socket.receive(packet);
                String message = new String(packet.getData(), 0, packet.getLength());
                handle(message);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void stopListening() {
        isRunning = false;
        if (socket != null && !socket.isClosed()) {
            socket.close();
        }
    }

    public abstract void handle(String data);
}
