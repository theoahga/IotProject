package com.irc.iotproject.network;

import java.net.DatagramSocket;
import java.net.SocketException;

public class ReceiveThread extends Thread{

    public ReceiveThread(String port){

    }

    @Override
    public void run() {
        try{
            DatagramSocket socket = new DatagramSocket(10000);
            byte[] receiveData = new byte[1024];

        } catch (SocketException e) {
            throw new RuntimeException(e);
        }
    }
}
