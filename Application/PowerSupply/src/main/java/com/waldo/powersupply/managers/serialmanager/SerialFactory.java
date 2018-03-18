package com.waldo.powersupply.managers.serialmanager;

import com.waldo.powersupply.managers.serialmanager.SerialEnums.MessageTypes;

import java.util.Arrays;

public class SerialFactory {

    private static final char startChar = MessageTypes.PICMessageVariable.getStartChar().charAt(0);
    private static final char stopChar = MessageTypes.PICMessageVariable.getStopChar().charAt(0);
    private static final char sepChar = MessageTypes.PICMessageVariable.getSeparator().charAt(0);



    private static final byte C_START = 0x01;
    private static final byte C_HANDSHAKE = 0x02;

    private static final byte C_SET_VOLTAGE = 0x10;
    private static final byte C_SET_CURRENT = 0x11;
    private static final byte C_GET_VOLTAGE = 0x12;
    private static final byte C_GET_CURRENT = 0x13;

    private static final byte C_LCD_BRIGHTNESS = 0x70;
    private static final byte C_LCD_CONTRAST = 0x71;



    private static int ackId = 0;
    private static int getNextAckId() {
        ackId++;
        if (ackId > 255) {
            ackId = 0;
        }
        return ackId;
    }

    private static SerialMessage create(byte command, byte... message) {
        SerialMessage m = new SerialMessage(MessageTypes.PICMessageVariable, false);
        m.setCommand(String.valueOf(command));
        m.setMessage(Arrays.toString(message));
        m.setAckId(getNextAckId());

        StringBuilder asString = new StringBuilder();
        asString.append(startChar);
        asString.append((char) command);
        asString.append(sepChar);
        if (message != null) {
            asString.append((char) message.length);
        } else {
            asString.append((char) 0);
        }
        asString.append(sepChar);
        if (message != null && message.length > 0) {
            for (byte b : message) {
                asString.append((char) (b & 0xFF));
            }
        }
        asString.append(sepChar);
        asString.append((char) m.getAckId());
        asString.append(stopChar);

        m.setInput(asString.toString());

        return m;
    }

    public static SerialMessage handshakeMessage() {
        return create(C_HANDSHAKE);
    }

    public static SerialMessage setLcdContrast(int contrast) {
        if (contrast > 50) contrast = 50;
        if (contrast < 1) contrast = 1;
        return create(C_LCD_CONTRAST, (byte) contrast);
    }

    public static SerialMessage setLcdBrightness(int brightness) {
        if (brightness > 8) brightness = 8;
        if (brightness < 1) brightness = 1;
        return create(C_LCD_BRIGHTNESS, (byte) brightness);
    }

    public static SerialMessage setVoltage(int voltage) {
        if (voltage > 12000) voltage = 12000;
        if (voltage < 0) voltage = 0;

        byte lsb = (byte) (voltage & 0xFF);
        byte msb = (byte) ((voltage >> 8) & 0xFF);

        return create(C_SET_VOLTAGE, msb, lsb);
    }
}
