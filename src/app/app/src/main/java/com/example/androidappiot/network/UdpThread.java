package com.example.androidappiot.network;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.concurrent.BlockingQueue;

public class UdpThread extends Thread {
    private BlockingQueue<String> queue;
    private String ip;
    private String port;
    private DatagramSocket UDPSocket; // structure Java permettant d'acceder au réseau (UDP)

    public UdpThread(BlockingQueue<String> queue, String ip, String port){
        this.queue = queue;
        this.ip = ip;
        this.port = port;
    }

    @Override
    public void run() {
        try {
            String message = queue.take();
            DatagramSocket UDPSocket = new DatagramSocket();
            InetAddress address = InetAddress.getByName(this.ip);
            byte[] data = message.getBytes();
            DatagramPacket packet = new DatagramPacket(data,data.length, address, Integer.parseInt(this.port));
            UDPSocket.send(packet);
        } catch (InterruptedException | IOException e) {
            throw new RuntimeException(e);
        } finally {
            Thread.currentThread().interrupt();
        }
    }
}