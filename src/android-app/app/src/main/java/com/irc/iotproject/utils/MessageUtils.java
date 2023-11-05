package com.irc.iotproject.utils;

import com.irc.iotproject.exceptions.NullMessageException;

import java.util.HashMap;
import java.util.Map;

public class MessageUtils {
    public static String buildMessageWithHeader(String rawMessage) {
        return getHeader() + ":" + encryptRawMessage(rawMessage);
    }

    public static String getRawMessage(String encryptedMessageWithHeader) {
        String encryptedMessage = encryptedMessageWithHeader.substring((getHeader()+ ":").length());
        return decryptRawMessage(encryptedMessage);
    }

    private static String encryptRawMessage(String rawMessage) {
        String encryptedMessage = null;
        try{
            checkMessage(rawMessage);
            encryptedMessage = CrytoUtils.encrypt(rawMessage,CrytoUtils.getKey());
        }catch(NumberFormatException e){
            e.printStackTrace();
        } catch (NullMessageException e) {
            e.printStackTrace();
        }
        return encryptedMessage;
    }

    private static String decryptRawMessage(String rawEncryptedMessage) {
        String decryptedMessage = null;
        try{
            checkMessage(rawEncryptedMessage);
            decryptedMessage = CrytoUtils.decrypt(rawEncryptedMessage,CrytoUtils.getKey());
        }catch(NumberFormatException e){
            e.printStackTrace();
        } catch (NullMessageException e) {
            e.printStackTrace();
        }
        return decryptedMessage;
    }

    public static void checkMessage(String message) throws NullMessageException {
        if(message.isEmpty() || message == ""){
            throw new NullMessageException("Message cannot be empty or null");
        }
    }

    public static String getHeader(){
        try{
            return System.getProperty("request_header");
        }catch (Exception e){
            throw new NullPointerException("There is no properties for 'request_header'");
        }
    }

    public static Map<String, Double> getDatasetFromRawMessage(String message){
        Map<String, Double> result = new HashMap<>();
        String[] elements = message.split(";");
        for (String elt : elements) {
            if(elt.startsWith("T:")){
                result.put("temperature", Double.parseDouble(elt.substring("T:".length())));
            }else if (elt.startsWith("L:")) {
                result.put("brightness", Double.parseDouble(elt.substring("L:".length())));
            }else {
                result.put("timestamp", Double.parseDouble(elt));
            }
        }
        return result;
    }
}
