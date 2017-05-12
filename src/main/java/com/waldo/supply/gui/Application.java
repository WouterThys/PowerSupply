package com.waldo.supply.gui;

import com.waldo.supply.serial.Message;
import com.waldo.supply.serial.OnMessageListener;
import com.waldo.supply.serial.SerialManager;

import javax.swing.*;
import java.awt.*;
import java.math.BigDecimal;
import java.util.Date;

import static com.waldo.supply.serial.SerialManager.serial;

public class Application extends JFrame implements OnMessageListener {

    private static final int ADC_BITS = 12;
    private static final double ADC_VREF = 3.35; // V

    private static final double R_TEM = 22000; // Ohm
    private static final double R_MON = 8247; // Ohm
    private static final double A_MON = 5000; // Current monitor amplification

    private static final double ADC_MAX_VAL = (Math.pow(2, ADC_BITS)-1);
    private static final double VOLTAGE_PER_BIT = ADC_VREF / ADC_MAX_VAL; // V


    private static final String ADC_CURRENT = "I";
    private static final String ADC_TEMPERATURE = "T";
    private static final String ADC_VOLTAGE = "V";

    private JPanel inputPanel;
    private JPanel outputPanel;

    private JTextArea inputArea;
    private JSlider voltageSlider;

    public Application() {
        serial().addOnMessageListener(this);


        // Layout
        inputPanel = new JPanel(new BorderLayout());
        outputPanel = new JPanel(new BorderLayout());

        inputArea = new JTextArea(50, 100);
        inputArea.setEditable(false);

        voltageSlider = new JSlider(JSlider.VERTICAL, 0, 120, 0);
        voltageSlider.setMajorTickSpacing(10);
        voltageSlider.setMinorTickSpacing(1);
        voltageSlider.setPaintTicks(true);
        voltageSlider.addChangeListener(e -> {
            JSlider slider = (JSlider) e.getSource();
            if (!slider.getValueIsAdjusting()) {

                Double value = (slider.getValue() * ADC_MAX_VAL) / 120; // Value from 0 -> ADC_MAX_VALUE
                Double dacVoltage = calculateVoltage(value.intValue());
                Double ampVoltage = calculateVoltage(value.intValue() * 3);

                System.out.println("DAC: " + round(dacVoltage, 2) + "V, AMP: " + round(ampVoltage, 2) + "V");

                serial().write("V", String.valueOf(value.intValue()));
            }
        });


        inputPanel.add(new JScrollPane(inputArea), BorderLayout.CENTER);
        outputPanel.add(voltageSlider, BorderLayout.CENTER);

        setLayout(new BorderLayout());
        add (inputPanel, BorderLayout.WEST);
        add(outputPanel, BorderLayout.EAST);
    }

    @Override
    public void onMessageReceived(Message message) {
        StringBuilder sb = new StringBuilder();

        if (message.id == Message.ACK_CHAR) {
            System.out.println("Acknowledge of ack : " + message.ackId);
            return;
        }

        sb.append(new Date()).append(" - ");
        for (Message.CommandAndMessage cm : message.commandAndMessageList) {

            sb.append("\tCommand: ").append(cm.getCommand()).append(" ");

            if (cm.getCommand().contains(ADC_CURRENT)) {
                int val = Integer.valueOf(cm.getMessage());
                sb.append("Current: ").append(toEngineeringString(calculateCurrent(val))).append("A");
            }
            else if (cm.getCommand().contains(ADC_TEMPERATURE)) {
                int val = Integer.valueOf(cm.getMessage());
                sb.append("Temperature: ").append(toEngineeringString(calculateTemperature(val))).append("C");
            }
//            else if (cm.getCommand().contains(ADC_VOLTAGE)) {
//                int val = Integer.valueOf(cm.getMessage());
//                sb.append("Voltage: ").append(toEngineeringString(calculateVoltage(val))).append("V\n");
//            }
            else {
                sb.append("Message: ").append(cm.getMessage()).append("\n");
            }
        }
        sb.append("\n");
        inputArea.append(sb.toString());
        inputArea.setCaretPosition(inputArea.getDocument().getLength());
    }

    private double calculateVoltage(int adcValue) {
        return (adcValue * VOLTAGE_PER_BIT); // Voltage in V
    }

    private double calculateTemperature(int adcValue) {
        double voltage = calculateVoltage(adcValue);
        double current = voltage / R_TEM;
        return (current * 1000000);
    }

    private double calculateCurrent(int adcValue) {
        double voltage = calculateVoltage(adcValue);
        double I_mon = voltage / R_MON;
        return (I_mon * A_MON);
    }

    private String toEngineeringString(double value) {
        value *= 1000000;

        if (value < 1000) {
            return String.valueOf(round(value, 3)) + "µ";
        } else if (value >= 1000 && value < 1000000) {
            value /= 1000;
            return String.valueOf(round(value, 3)) + "m";
        } else {
            value /= 1000000;
            return String.valueOf(round(value, 3)) + "";
        }
    }

    private double round(double value, int places) {
        BigDecimal bd = new BigDecimal(value);
        bd = bd.setScale(places, BigDecimal.ROUND_HALF_UP);
        return bd.doubleValue();
    }
}
