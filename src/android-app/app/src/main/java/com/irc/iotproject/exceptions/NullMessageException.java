package com.irc.iotproject.exceptions;

/*
* Throw this Exception when Message is null or empty
* */
public class NullMessageException extends Exception{
    private String message;

    public NullMessageException(String message) {
        this.message = message;
    }
}
