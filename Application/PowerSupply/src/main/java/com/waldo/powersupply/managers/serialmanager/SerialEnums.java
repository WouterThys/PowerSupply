package com.waldo.powersupply.managers.serialmanager;

import com.fazecast.jSerialComm.SerialPort;

public class SerialEnums {


    public enum MessageTypes {
        Text ("Simple text", "", -2, -2, "", "", "", false),
        PICMessageShort("PIC Simple short", "Simple PIC message, short form", 1, 1, "$", "&", ":", false),
        PICMessageLong("PIC Simple long", "Simple PIC message, long form", 1, 2, "$", "&", ":", true),
        PICMessageVariable("PIC Simple variable", "Variable PIC message", 1, -1, "$", "&", ":", true),
        PICAcknowledge("ACK", "ACK", 0, 0, "$", "&", "", false);

        public static final int VARIABLE = -1;
        public static final int NONE = -2;

        private final String name;
        private final String description;

        private final int commandBytes;
        private final int messageBytes;
        private final String startChar;
        private final String stopChar;
        private final String separator;

        private final boolean acknowledge;

        MessageTypes(String name, String description, int commandBytes, int messageBytes, String startChar, String stopChar, String separator, boolean acknowledge) {
            this.name = name;
            this.description = description;
            this.commandBytes = commandBytes;
            this.messageBytes = messageBytes;
            this.startChar = startChar;
            this.stopChar = stopChar;
            this.separator = separator;
            this.acknowledge = acknowledge;
        }

        @Override
        public String toString() {
            return name;
        }

        public String getTemplate() {
            StringBuilder builder = new StringBuilder();

            // Start
            builder.append(startChar);

            // Command
            if (commandBytes != VARIABLE) {
                if (commandBytes == 1) {
                    builder.append("C");
                } else {
                    for (int i = 0; i < commandBytes; i++) {
                        builder.append("C").append(i);
                    }
                }
                builder.append(separator);
                if (messageBytes == VARIABLE) {
                    builder.append("L");
                    builder.append(separator);
                }
            }

            // SerialMessage
            if (messageBytes != VARIABLE) {
                if (messageBytes == 1) {
                    builder.append("M");
                } else {
                    for (int i = 0; i < messageBytes; i++) {
                        builder.append("M").append(i);
                    }
                }
            } else {
                if (commandBytes != VARIABLE) {
                    builder.append("M0M1M2..ML");
                }
            }

            // Acknowledge
            if (acknowledge) {
                builder.append(":");
                builder.append("A");
            }

            // Stop
            builder.append(stopChar);

            return builder.toString();
        }

        public String getName() {
            return name;
        }

        public String getDescription() {
            return description;
        }

        public int getCommandBytes() {
            return commandBytes;
        }

        public int getMessageBytes() {
            return messageBytes;
        }

        public String getStartChar() {
            return startChar;
        }

        public String getStopChar() {
            return stopChar;
        }

        public String getSeparator() {
            return separator;
        }

        public boolean isAcknowledge() {
            return acknowledge;
        }
    }


    public enum ParityTypes {
        None  (SerialPort.NO_PARITY),
        Odd   (SerialPort.ODD_PARITY),
        Even  (SerialPort.EVEN_PARITY),
        Mark  (SerialPort.MARK_PARITY),
        Space (SerialPort.SPACE_PARITY);


        private final int intValue;
        ParityTypes(int intValue) {
            this.intValue = intValue;
        }

        public int getIntValue() {
            return intValue;
        }

        public static ParityTypes fromInt(int intValue) {
            switch (intValue) {
                default:
                case SerialPort.NO_PARITY: return None;
                case SerialPort.ODD_PARITY: return Odd;
                case SerialPort.EVEN_PARITY: return Even;
                case SerialPort.MARK_PARITY: return Mark;
                case SerialPort.SPACE_PARITY: return Space;
            }
        }
    }

    public enum FlowControlTypes {
        None
    }

    public enum BaudRateValues {
        _1200  ("1200 bps", 1200),
        _2400  ("2400 bps", 2400),
        _4800  ("4800 bps", 4800),
        _9600  ("9600 bps", 9600),
        _14400 ("14400 bps", 14400),
        _19200 ("19200 bps", 19200),
        _28800 ("28800 bps", 28800),
        _38400 ("38400 bps", 38400),
        _57600 ("57600 bps", 57600),
        _Custom("<Custom>", 0);

        private final int baud;
        private final String string;
        BaudRateValues(String string, int baud) {
            this.string = string;
            this.baud = baud;
        }

        @Override
        public String toString() {
            return string;
        }

        public int getBaud() {
            return baud;
        }

        public static BaudRateValues fromBaud(int baud) {
            switch (baud) {
                case 1200: return _1200;
                case 2400: return _2400;
                case 4800: return _4800;
                case 9600: return _9600;
                case 14400: return _14400;
                case 19200: return _19200;
                case 28800: return _28800;
                case 38400: return _38400;
                case 57600: return _57600;
                default:
                case 0: return _Custom;
            }
        }
    }

    public enum DataBitValues {
        _5 ("5 bits", 5),
        _6 ("6 bits", 6),
        _7 ("7 bits", 7),
        _8 ("8 bits", 8);

        private final int bits;
        private final String string;
        DataBitValues(String string, int bits) {
            this.string = string;
            this.bits = bits;
        }

        @Override
        public String toString() {
            return string;
        }

        public int getBits() {
            return bits;
        }

        public static DataBitValues fromBits(int bits) {
            switch (bits) {
                case 5: return _5;
                case 6: return _6;
                case 7: return _7;
                default:
                case 8: return _8;
            }
        }
    }

    public enum StopBitValues {
        _1 ("1 bit", SerialPort.ONE_STOP_BIT),
        _2 ("2 bits", SerialPort.TWO_STOP_BITS);

        private final int stopBits;
        private final String string;
        StopBitValues(String string, int stopBits) {
            this.string = string;
            this.stopBits = stopBits;
        }

        @Override
        public String toString() {
            return string;
        }

        public int getStopBits() {
            return stopBits;
        }

        public static StopBitValues fromStopBits(int stopBits) {
            switch (stopBits) {
                default:
                case 1: return _1;
                case 2: return _2;
            }
        }
    }

}
