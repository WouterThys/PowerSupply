package com.waldo.supply.serial;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;

public class SerialManager {

    private static final SerialManager INSTANCE = new SerialManager();
    private static final char START_CHAR = '$';
    private static final char STOP_CHAR = '&';

    //
    // Static methods
    //

    public static SerialManager serial() {
        return INSTANCE;
    }

    //
    // Variables
    //
    private SerialPort serialPort;
    private String readBuffer = "";

    //
    // Constructor
    //

    private SerialManager() {}


    //
    // Public methods
    //

    public void initialize() {
        serialPort = findActiveSerialPort();
        if (serialPort != null) {
            serialPort.openPort();
            // Data available listener
            serialPort.addDataListener(new SerialPortDataListener() {
                @Override
                public int getListeningEvents() {
                    return SerialPort.LISTENING_EVENT_DATA_AVAILABLE;
                }

                @Override
                public void serialEvent(SerialPortEvent serialPortEvent) {
                    if (serialPortEvent.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) {
                        return;
                    }
                    SerialPort sp = serialPortEvent.getSerialPort();
                    byte[] newData = new byte[sp.bytesAvailable()];
                    sp.readBytes(newData, newData.length);
                    handleInputData(newData);
                }
            });
        }
    }

    public void registerShutdownHook() {
        Runtime.getRuntime().addShutdownHook(new Thread(this::close));
    }

    //
    // Private methods
    //

    private void close() {
        if (serialPort != null) {
            serialPort.closePort();
        }
    }

    private SerialPort findActiveSerialPort() {
        for (SerialPort port : SerialPort.getCommPorts()) {
            if (port.getSystemPortName().toUpperCase().contains("TTY") || port.getSystemPortName().toUpperCase().contains("COM")) {
                // TODO: Write to port and check if answer (with read and write blocking modes?)
                return port;
            }
        }
        return null;
    }

    private void handleInputData(byte[] data) {
        for (byte b : data) {
            if (b == STOP_CHAR) {
                createMessage(readBuffer);
                readBuffer = "";
            } else {
                readBuffer += b;
            }
        }
    }

    private void createMessage(String data) {

    }
}
