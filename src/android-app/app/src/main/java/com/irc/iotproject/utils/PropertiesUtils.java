package com.irc.iotproject.utils;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

public class PropertiesUtils {
    public static void loadProperties(){
        // Cannot load a config file ... So we did it by code
        Properties properties = new Properties();
        properties.setProperty("request_header","DMST");
        properties.setProperty("crypto_key","12341");
        properties.setProperty("listened_port","10001");

        System.setProperties(properties);
    }
}
