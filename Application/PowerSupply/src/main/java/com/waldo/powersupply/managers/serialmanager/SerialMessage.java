package com.waldo.powersupply.managers.serialmanager;


import com.waldo.powersupply.managers.serialmanager.SerialEnums.MessageTypes;

import java.text.ParseException;

public class SerialMessage {

    public static SerialMessage createRx(MessageTypes type, String input) throws ParseException {
        SerialMessage m = new SerialMessage(type, true);
        createFromString(m, input);
        return m;
    }

    public static SerialMessage createTx(MessageTypes type, String... args) throws ParseException {
        SerialMessage m = new SerialMessage(type, false);
//        switch (type) {
        if (args != null && args.length > 0) {
            m.input = args[0];
            m.message = args[0];
        }
//        }

        return m;
    }

    public static SerialMessage createTx(MessageTypes type, char command, int message) {
        SerialMessage m = new SerialMessage(type, false);
        switch (type) {
            case Text: break;
            case PICMessageShort: break;
            case PICMessageLong:
                m.command = String.valueOf(command);
                m.message = String.valueOf(message);
                m.ackId = createNewAckId();
                m.converted = true;

                char mChar1 = (char)(message & 0xFF);
                char mChar2 = (char)((message >> 8) & 0xFF);
                char aChar = (char)(m.ackId & 0xFF);

                m.input = type.getStartChar() +
                        command +
                        type.getSeparator() +
                        "12" + //mChar1 + mChar2 +
                        type.getSeparator() +
                        aChar +
                        type.getStopChar();
                break;
            case PICMessageVariable: break;
        }

        return m;
    }

    private static int createNewAckId() {
        ackIdCreator++;
        if (ackIdCreator > 255) {
            ackIdCreator = 0;
        }
        return ackIdCreator;
    }

    private MessageTypes messageType;
    private boolean isRx;
    private boolean converted = false;
    private boolean acknowledged = false;

    private String input;
    private String command;
    private String message;
    private int ackId;
    private static int ackIdCreator = 0;

    SerialMessage(MessageTypes messageType, boolean isRx) {
        this.messageType = messageType;
        this.isRx = isRx;
        this.input = "";
        this.command = "";
        this.message = "";
    }

    @Override
    public String toString() {
        return "SerialMessage{" +
                "messageType=" + getMessageType() +
                ", converted=" + isConverted() +
                ", input='" + getInput() + '\'' +
                ", command='" + getCommand() + '\'' +
                ", message='" + getMessage() + '\'' +
                ", ackId=" + getAckId() +
                '}';
    }

    public static void createFromString(SerialMessage m, String input) throws ParseException {
        MessageTypes t = m.getMessageType();

        if (input != null && !input.isEmpty()) {
            m.input = input;

            if (t.getStopChar().isEmpty() || t.getStartChar().isEmpty()) {
                m.message = input;
                m.setConverted(true);
            } else {
                if (input.contains(t.getStartChar()) && (input.contains(t.getStopChar()))) {
                    int start = input.indexOf(t.getStartChar());
                    int stop = input.indexOf(t.getStopChar());
                    int messageLength = t.getMessageBytes();

                    if (start < stop) {
                        String data = input.substring(start+1, stop);
                        m.input = input.substring(start, stop+1);

                        if (!data.isEmpty() && data.startsWith("@")) {
                            // Acknowledge
                            m.messageType = MessageTypes.PICAcknowledge;
                            m.isRx = true;
                            char ac = data.charAt(1);
                            byte ab = (byte) ac;
                            m.ackId = (int)ab;
                        } else {

                            String[] split = data.split(t.getSeparator());

                            int l = 2; // Command and message
                            if (t.isAcknowledge() && m.isTx()) {
                                l++; // And ack
                            }
                            if (messageLength == MessageTypes.VARIABLE) {
                                l++; // and L
                                messageLength = Integer.valueOf(split[1]);
                            }

                            if (split.length != l) {
                                throw new ParseException("Not a correct format..", start);
                            }

                            int position = 0;
                            m.command = split[position];
                            position++;
                            if (t.getMessageBytes() == MessageTypes.VARIABLE) {
                                position++;
                            }
                            m.message = split[position];
                            if (m.message.length() != messageLength) {
                                throw new ParseException("Not a correct format..", start);
                            }
                            position++;
                            if (t.isAcknowledge() && m.isTx()) {
                                try {
                                    m.ackId = (int)split[position].toCharArray()[0];
                                } catch (Exception e) {
                                    throw new ParseException("Invalid acknowledge", position);
                                }
                            }
                        }
                        m.setConverted(true);
                    } else {
                        throw new ParseException("Start character before stop character..", start);
                    }
                }
            }
        }
    }



    public MessageTypes getMessageType() {
        if (messageType == null) {
            messageType = MessageTypes.PICMessageVariable;
        }
        return messageType;
    }

    public String getInput() {
        if (input == null) {
            input = "";
        }
        return input;
    }

    void setInput(String input) {
        this.input = input;
    }

    public String getCommand() {
        if (command == null) {
            command = "";
        }
        return command;
    }

    void setCommand(String command) {
        this.command = command;
    }

    public String getMessage() {
        if (message == null) {
            message = "";
        }
        return message;
    }

    void setMessage(String message) {
        this.message = message;
    }

    public int getAckId() {
        return ackId;
    }

    void setAckId(int ackId) {
        this.ackId = ackId;
    }

    public boolean isConverted() {
        return converted;
    }

    public void setConverted(boolean converted) {
        this.converted = converted;
    }

    public boolean isRx() {
        return isRx;
    }

    public boolean isTx() {
        return !isRx;
    }

    public void setRx(boolean rx) {
        this.isRx = rx;
    }

    public void setTx(boolean tx) {
        this.isRx = !tx;
    }

    public boolean isAcknowledged() {
        return acknowledged;
    }

    public void setAcknowledged(boolean acknowledged) {
        this.acknowledged = acknowledged;
    }
}
