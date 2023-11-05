package com.irc.iotproject.utils;

public class CrytoUtils {
    public static String encrypt(String text, int key) {
        StringBuilder encryptedText = new StringBuilder();
        for (int i = 0; i < text.length(); i++) {
            char c = text.charAt(i);
            char encryptedChar = (char) (c + key);
            encryptedText.append(encryptedChar);
        }
        return encryptedText.toString();
    }

    public static String decrypt(String text, int key) {
        StringBuilder decryptedText = new StringBuilder();
        for (int i = 0; i < text.length(); i++) {
            char c = text.charAt(i);
            char decryptedChar = (char) (c - key);
            decryptedText.append(decryptedChar);
        }
        return decryptedText.toString();
    }
}
