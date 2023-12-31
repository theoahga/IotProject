package com.irc.iotproject.network;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.concurrent.BlockingQueue;

/**
 * The thread allows us to send message from a UDP packet
 * */
public class SendThread extends Thread{
    private BlockingQueue<String> queue;
    private String ip;
    private String port;

    public SendThread(BlockingQueue<String> queue, String ip, String port){
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
            UDPSocket.close();
        } catch (InterruptedException | IOException e) {
            throw new RuntimeException(e);
        } finally {
            Thread.currentThread().interrupt();
        }

    }
}