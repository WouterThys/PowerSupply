package com.waldo.powersupply.managers.serialmanager;


public interface SerialListener {
    void onSerialError(String error, Throwable throwable);
    void onTransmitted(SerialMessage serialMessage);
    void onReceived(SerialMessage serialMessage);
}
