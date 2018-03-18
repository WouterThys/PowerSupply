package com.waldo.powersupply.managers.serialmanager;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;
import com.waldo.powersupply.managers.serialmanager.SerialEnums.BaudRateValues;
import com.waldo.powersupply.managers.serialmanager.SerialEnums.MessageTypes;
import com.waldo.utils.DateUtils;

import javax.swing.*;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.ExecutionException;

public class SerialManager {

    /*
     *                  SINGLETON
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    private static final SerialManager Instance = new SerialManager();
    public static SerialManager serMgr() {
        return Instance;
    }
    private SerialManager() { }

    /*
     *                  VARIABLES
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    private List<SerialListener> serialListenerList = new ArrayList<>();
    private SerialPort serialPort;
    private final List<SerialMessage> txSerialMessageList = new ArrayList<>();
    private final List<SerialMessage> rxSerialMessageList = new ArrayList<>();

    private final WriteQueue<SerialMessage> writeQueue = new WriteQueue<>(100);
    private WriteQueueWorker writeQueueWorker;

    // Settings
    private MessageTypes messageType;
    private boolean waitForAcknowledge = true;
    private int waitForAcknowledgeTime = 100;
    private volatile boolean writing = false;

    /*
     *                  METHODS
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    public void init(SerialListener serialListener) {
        serialListenerList.add(serialListener);
    }

    public void startHandshake(HandshakeListener handshakeListener, BaudRateValues baudRate, MessageTypes messageType) {
        new HandshakeWorker(handshakeListener, baudRate, messageType).execute();
    }

    public void registerShutDownHook() {
        Runtime.getRuntime().addShutdownHook(new Thread(this::close));
    }

    public List<SerialPort> getSerialPorts() {
        List<SerialPort> portList = new ArrayList<>();
        Collections.addAll(portList, SerialPort.getCommPorts());
        return portList;
    }

    public SerialPort getSerialPort() {
        return serialPort;
    }

    public SerialPort findSerialPortByName(String name) {
        for (SerialPort port : getSerialPorts()) {
            if (port.getSystemPortName().equals(name)) {
                return port;
            }
        }



        return null;
    }

    public MessageTypes getMessageType() {
        if (messageType == null) {
            messageType = MessageTypes.Text;
        }
        return messageType;
    }

    public boolean isWaitForAcknowledge() {
        return waitForAcknowledge;
    }

    public void setWaitForAcknowledge(boolean waitForAcknowledge) {
        this.waitForAcknowledge = waitForAcknowledge;
    }

    public int getWaitForAcknowledgeTime() {
        return waitForAcknowledgeTime;
    }

    public void setWaitForAcknowledgeTime(int waitForAcknowledgeTime) {
        this.waitForAcknowledgeTime = waitForAcknowledgeTime;
    }

    public void setMessageType(MessageTypes messageType) {
        this.messageType = messageType;
    }

    public void clearRxMessages() {
        rxSerialMessageList.clear();
    }

    public void clearTxMessages() {
        txSerialMessageList.clear();
    }

    public void addSerialListener(SerialListener serialListener) {
        if (!serialListenerList.contains(serialListener)) {
            serialListenerList.add(serialListener);
        }
    }

    public void removeSerialListener(SerialListener serialListener) {
        if (serialListenerList.contains(serialListener)) {
            serialListenerList.remove(serialListener);
        }
    }

    public SerialListener getMainSerialListener() {
        if (serialListenerList.size() > 0) {
            return serialListenerList.get(0);
        }
        return null;
    }

    private void close() {
        if (serialPort != null) {
            try {
                serialPort.removeDataListener();
                serialPort.closePort();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        writeQueue.stop();

        if (writeQueueWorker != null) {
            writeQueueWorker.keepRunning = false;
        }

        writeToLogFile("Closing down serial");
    }

    private void writeToLogFile(String line) {
        List<String> lines = Collections.singletonList(DateUtils.formatDateTime(DateUtils.now()) + " - " + line);
        Path file = Paths.get("SerialLog.txt");
        try {
            Files.write(file, lines, Charset.forName("UTF-8"));
        } catch (IOException e) {
            //
        }
    }

    public boolean open(SerialPort port) {
        boolean result = false;
        if (port != null) {
            if (serialPort != null) {
                try {
                    serialPort.removeDataListener();
                    serialPort.closePort();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            this.serialPort = port;
            addDataAvailableEvent(this.serialPort);
            if (!this.serialPort.openPort()) {
                onError("Failed to open port: " + port.getDescriptivePortName());
            } else {
                result = true;
                writeToLogFile("Opening port " + serialPort.getSystemPortName());
            }
            port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 200, 50);

            // Start worker
            if (writeQueueWorker == null) {
                writeQueueWorker = new WriteQueueWorker();
                writeQueueWorker.execute();
            } else {
                writeQueueWorker.keepRunning = false;
                writeQueueWorker.cancel(true);
                SwingUtilities.invokeLater(() -> {
                    //while (!writeQueueWorker.done) {};
                    writeQueueWorker = new WriteQueueWorker();
                    writeQueueWorker.execute();
                });
            }

        }
        return result;
    }

    public void initSerialPort(SerialPort port, int baud, int bits, int stopBits, int parity) {
        port.setComPortParameters(baud, bits, stopBits, parity);
        open(port);
    }

    public List<SerialMessage> getTxSerialMessageList() {
        return txSerialMessageList;
    }

    public List<SerialMessage> getRxSerialMessageList() {
        return new ArrayList<>(rxSerialMessageList);
    }

    public void write(final SerialMessage message) {
        try {
            if (serialPort != null) {
                if (serialPort.isOpen()) {
                    writeQueue.put(message);
                } else {
                    onError("COM port is closed..");
                }
            } else {
                onError("No COM port available..");
            }
        } catch (Exception e) {
            onError(e);
        }
    }

    private void addToMessageList(SerialMessage serialMessage) {
        txSerialMessageList.add(serialMessage);
    }

    private void onError(Throwable throwable) {
        onError(throwable.getMessage(), throwable);
    }

    private void onError(String error) {
        onError(error, null);
    }

    private void onError(String error, Throwable throwable) {
        for (SerialListener listener : serialListenerList) {
            listener.onSerialError(error, throwable);
        }
    }

    private void onReceived(SerialMessage serialMessage) {
        for (SerialListener listener : serialListenerList) {
            listener.onReceived(serialMessage);
        }
    }

    private void onTransmitted(SerialMessage serialMessage) {
        for (SerialListener listener : serialListenerList) {
            listener.onTransmitted(serialMessage);
        }
    }

    private void addDataAvailableEvent(SerialPort serialPort) {
        serialPort.addDataListener(new SerialPortDataListener() {
            @Override
            public int getListeningEvents() {
                return SerialPort.LISTENING_EVENT_DATA_AVAILABLE;
            }

            @Override
            public void serialEvent(SerialPortEvent event) {
                if (event.getEventType() == SerialPort.LISTENING_EVENT_DATA_AVAILABLE) {
                    SerialPort comPort = event.getSerialPort();
                    byte[] bytes = new byte[comPort.bytesAvailable()];
                    comPort.readBytes(bytes, bytes.length);

                    if (!writing) {
                        inputMessage += new String(bytes);
                        newDataAvailable();
                    }
                }
            }
        });
    }

    private void tryAcknowledge(SerialMessage acknowledgeMessage) {
        for (SerialMessage tx : txSerialMessageList) {
            if (!tx.isAcknowledged()) {
                if (tx.getAckId() == acknowledgeMessage.getAckId()) {
                    tx.setAcknowledged(true);
                    return;
                }
            }
        }
    }


    private String inputMessage = "";
    private void newDataAvailable() {
        if (!inputMessage.isEmpty()) {
            try {
                SerialMessage m = SerialMessage.createRx(messageType, inputMessage);
                if (m != null && m.isConverted()) {
                    if (m.getMessageType() == MessageTypes.PICAcknowledge) {
                        tryAcknowledge(m);
                    } else {
                        rxSerialMessageList.add(m);
                        onReceived(m);
                    }
                    int n = inputMessage.indexOf(m.getInput());
                    try {
                        inputMessage = inputMessage.substring(n + m.getInput().length(), inputMessage.length());

                        if (!inputMessage.isEmpty()) {
                            newDataAvailable();
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            } catch (ParseException e) {
                onError(e);
                e.printStackTrace();
            }
        }
    }


    private class WriteQueueWorker extends SwingWorker<Integer, String> {

        volatile boolean keepRunning = true;
        volatile boolean done = false;

        private void throwError(final Throwable throwable, final String error) {
            SwingUtilities.invokeLater(() -> onError(error, throwable));
        }

        @Override
        protected Integer doInBackground() {
            System.out.println("WriteQueueWorker started");
            try {
                while (keepRunning) {
                    SerialMessage queueObject = writeQueue.take();
                    if (queueObject != null) {
                        writing = true;
                        if (serialPort != null && serialPort.isOpen()) {
                            try {
                                String data = queueObject.getInput();
                                data += "\0";

                                byte[] writeBuffer = new byte[data.length()];
                                for (int c = 0; c < data.length(); c++) {
                                    writeBuffer[c] = (byte) (data.charAt(c) & 0xFF);
                                }

                                serialPort.writeBytes(writeBuffer, writeBuffer.length);

                                byte[] readBuffer = new byte[4];
                                int numRead = serialPort.readBytes(readBuffer, readBuffer.length);
                                if (numRead > 0) {
                                    // Check input
                                    String input = new String(readBuffer);
                                    SerialMessage message = SerialMessage.createRx(messageType, input);
                                    if (message.getMessageType() == MessageTypes.PICAcknowledge) {
                                        if (queueObject.getAckId() == message.getAckId()) {
                                            queueObject.setAcknowledged(true);
                                        }
                                    }
                                }

                                SwingUtilities.invokeLater(() -> onTransmitted(queueObject));
                            } catch (Exception e) {
                                throwError(e, "");
                            } finally {
                                writing = false;
                            }
                        }
                    }
                }
            } catch (InterruptedException e) {
                System.out.println("Stop worker");
                keepRunning = false;
            }
            return 0;
        }


        @Override
        protected void done() {
            System.out.println("WriteQueueWorker done");
            done = true;
        }
    }


    public interface HandshakeListener {
        void onHandshakeStarted();
        void onHandshakeSuccess(SerialPort serialPort);
        void onHandshakeFailed(String reason);
    }

    private class HandshakeWorker extends SwingWorker<SerialPort, String> {

        private final HandshakeListener handshakeListener;
        private final BaudRateValues baudRate;
        private final MessageTypes messageType;

        HandshakeWorker(HandshakeListener handshakeListener, BaudRateValues baudRate, MessageTypes messageType) {

            this.handshakeListener = handshakeListener;
            this.baudRate = baudRate;
            this.messageType = messageType;
        }

        @Override
        protected SerialPort doInBackground() {
            handshakeListener.onHandshakeStarted();
            SerialPort serialPort = null;

            List<SerialPort> serialPortList = getSerialPorts();
            if (serialPortList != null && serialPortList.size() > 0) {
                try {
                    for (SerialPort port : serialPortList) {
                        port.setBaudRate(baudRate.getBaud());
                        port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 200, 50);
                        port.openPort();

                        SerialMessage handShakeMessage = SerialFactory.handshakeMessage();
                        String data = handShakeMessage.getInput();
                        data += "\0";
                        port.writeBytes(data.getBytes(), data.length());

                        byte[] readBuffer = new byte[4];
                        int numRead = port.readBytes(readBuffer, readBuffer.length);
                        if (numRead > 0) {
                            // Check input
                            String input = new String(readBuffer);
                            SerialMessage message = SerialMessage.createRx(messageType, input);
                            if (message.getMessageType() == MessageTypes.PICAcknowledge) {
                                if (handShakeMessage.getAckId() == message.getAckId()) {
                                    serialPort = port;
                                    serialPort.closePort();
                                    break;
                                }
                            }
                        }

                        port.closePort();
                    }
                } catch (Exception e) {
                    handshakeListener.onHandshakeFailed(e.toString());
                }
            } else {
                handshakeListener.onHandshakeFailed("No serial ports available..");
            }

            return serialPort;
        }

        @Override
        protected void done() {
            try {
                SerialPort serialPort = get();
                if (serialPort != null) {
                    handshakeListener.onHandshakeSuccess(serialPort);
                } else {
                    handshakeListener.onHandshakeFailed("No serial ports found..");
                }
            } catch (InterruptedException | ExecutionException e) {
                e.printStackTrace();
            }
        }
    }

}
