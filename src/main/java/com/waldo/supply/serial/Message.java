package com.waldo.supply.serial;

import java.util.ArrayList;
import java.util.List;

public class Message {

    public static final char START_CHAR = '&';
    public static final char STOP_CHAR = '$';
    public static final char SEP_CHAR = ':';

    public static final char ACK_CHAR = 'A';

    public static byte writeAckId = 0;

    public byte id;
    public byte blockLength;
    public List<CommandAndMessage> commandAndMessageList;
    public byte ackId;
    public byte check;

    public Message() {
        commandAndMessageList = new ArrayList<>();
    }

    public static Message decode(String data) {
        Message message = new Message();
        String tortured;
        if (data != null && !data.isEmpty()) {
            try {
                if (!data.contains(String.valueOf(START_CHAR))) return null;
                if (!data.contains(String.valueOf(STOP_CHAR))) return null;

                // Find data
                int start = data.indexOf(START_CHAR);
                int stop = data.indexOf(STOP_CHAR);
                if (stop < start) {
                    return null;
                }
                // Cut start and stop character
                tortured = data.substring(start + 1, stop);

                if (tortured.charAt(0) == ACK_CHAR) {
                    message.id = ACK_CHAR;
                    message.ackId = Byte.valueOf(String.valueOf(tortured.charAt(1)));
                } else {
                    String[] parts = tortured.split(String.valueOf(SEP_CHAR));
                    message.id = Byte.valueOf(String.valueOf(parts[0]));
                    message.blockLength = Byte.valueOf(String.valueOf(parts[1]));
                    for (int i = 0; i < message.blockLength; i++) {
                        String com = String.valueOf(parts[i + 2]);
                        String mes = String.valueOf(parts[i + 3]);
                        CommandAndMessage cm = new CommandAndMessage(com, mes);
                        message.commandAndMessageList.add(cm);
                    }
                    message.ackId = Byte.valueOf(parts[message.blockLength + 3]);
                    message.check = Byte.valueOf(parts[message.blockLength + 4]);
                }
                return message;

            } catch (Exception e) {
                e.printStackTrace();
            }

        }
        return null;
    }

    public static Message encode(String command, String message) {
        List<CommandAndMessage> list = new ArrayList<>(1);
        list.add(new CommandAndMessage(command, message));
        Message m = new Message();
        writeAckId ++;

        m.id = SerialManager.id;
        m.blockLength = 1;
        m.commandAndMessageList = list;
        m.ackId = writeAckId;
        m.check = 3;

        return m;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();

        sb.append(START_CHAR);

        sb.append(id);
        sb.append(SEP_CHAR).append(blockLength);
        for (int i = 0; i < blockLength; i++) {
            sb.append(SEP_CHAR).append(commandAndMessageList.get(i).command);
            sb.append(SEP_CHAR).append(commandAndMessageList.get(i).message);
        }
        sb.append(SEP_CHAR).append(ackId);
        sb.append(SEP_CHAR).append(check);

        sb.append(STOP_CHAR);

        return sb.toString();
    }

    public static class CommandAndMessage {

        private String command;
        private String message;

        public CommandAndMessage(String command, String message) {
            this.command = command;
            this.message = message;
        }

        public CommandAndMessage() {
            this("", "");
        }

        public String getCommand() {
            return command;
        }

        public void setCommand(String command) {
            this.command = command;
        }

        public String getMessage() {
            return message;
        }

        public void setMessage(String message) {
            this.message = message;
        }
    }
}
