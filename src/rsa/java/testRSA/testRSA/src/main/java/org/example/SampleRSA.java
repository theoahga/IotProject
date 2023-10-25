package org.example;

import java.security.KeyPair;

public class SampleRSA {
    public static void main(String[] args) throws Exception {
        EncryptDecryptor encryptDecryptor = new EncryptDecryptor();


        String textToEncrypt = "Theo is the best";
        String encoded = encryptDecryptor.encode(textToEncrypt);
        System.out.println(encoded);
        String decoded = encryptDecryptor.decode(encoded);
        System.out.println(decoded);
    }
}
