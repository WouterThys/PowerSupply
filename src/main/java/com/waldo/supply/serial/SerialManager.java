package com.waldo.supply.serial;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;

public class SerialManager {

    public static final byte id = 1;

    private static final SerialManager INSTANCE = new SerialManager();
    private static final int BUFFER_LENGTH = 1024;

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
    private byte[] readBuffer = new byte[BUFFER_LENGTH];
    private int bufferCount;
    private OnMessageListener messageListener;

    //
    // Constructor
    //

    private SerialManager() {}


    //
    // Public methods
    //

    public void initialize() {
        serialPort = SerialPort.getCommPort("dev/ttyUSB0");//findActiveSerialPort();
        if (serialPort != null) {
            serialPort.setBaudRate(57600);
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
            System.out.println("Connected to: " + serialPort.getSystemPortName());
        }
    }

    public void registerShutdownHook() {
        Runtime.getRuntime().addShutdownHook(new Thread(this::close));
    }

    public void addOnMessageListener(OnMessageListener messageListener) {
        this.messageListener = messageListener;
    }

    public Message write(String command, String message) {
        if (serialPort.isOpen()) {
            Message m = Message.encode(command, message);
            String toWrite = m.toString();
            System.out.println("Writing: " + toWrite);
            serialPort.writeBytes(toWrite.getBytes(), toWrite.getBytes().length);
            return m;
        }
        return null;
    }

    public Message write(SerialPort port, String command, String message) {
        if (port.isOpen()) {
            Message m = Message.encode(command, message);
            String toWrite = m.toString();
            System.out.println("Writing: " + toWrite);
            port.writeBytes(toWrite.getBytes(), toWrite.getBytes().length);
            return m;
        }
        return null;
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
            if (port.getSystemPortName().toUpperCase().contains("USB") || port.getSystemPortName().toUpperCase().contains("COM")) {
                if (isPicListening(port)) {
                    return port;
                }
            }
        }
        return null;
    }

    private boolean isPicListening(SerialPort port) {
        int numRead;
        try {
            port.openPort();
            port.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 2000, 0);

            // Write message
            Message write = write(port, "I", "I");

            // Read
            byte[] read = new byte[4];
            port.readBytes(read, read.length);
            System.out.println("Read: " + new String(read) + " on port: " + port.getSystemPortName());
            Message r = Message.decode(new String(read));
            return (r != null) && (r.id == Message.ACK_CHAR) && (r.ackId == write.ackId);

        } finally {
            port.closePort();
        }
    }

    private void handleInputData(byte[] data) {
        for (byte b : data) {
            if (b == Message.STOP_CHAR) {
                // TODO: create new message event
                readBuffer[bufferCount] = b;
                bufferCount++;

                Message m = Message.decode(new String(readBuffer, 0, bufferCount));
                if (m != null && messageListener != null) {
                    messageListener.onMessageReceived(m);
                }

                bufferCount = 0;
            } else {
                if (bufferCount < BUFFER_LENGTH) {
                    readBuffer[bufferCount] = b;
                    bufferCount++;
                } else {
                    System.err.println("Buffer length exceeded");
                    bufferCount = 0;
                }

            }
        }
    }
}
