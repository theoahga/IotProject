package com.irc.iotproject.utils;

public class MessageUtils {
    public String buildMessageWithHeader(String rawMessage){
        return "DMST:"+encryptRawMessage(rawMessage);
    }

    private String encryptRawMessage(String rawMessage){

        return "";
    }
}
