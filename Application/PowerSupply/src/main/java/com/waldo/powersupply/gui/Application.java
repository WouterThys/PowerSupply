package com.waldo.powersupply.gui;

import com.fazecast.jSerialComm.SerialPort;
import com.waldo.powersupply.Main;
import com.waldo.powersupply.managers.serialmanager.SerialEnums.BaudRateValues;
import com.waldo.powersupply.managers.serialmanager.SerialEnums.MessageTypes;
import com.waldo.powersupply.managers.serialmanager.SerialFactory;
import com.waldo.powersupply.managers.serialmanager.SerialListener;
import com.waldo.powersupply.managers.serialmanager.SerialManager;
import com.waldo.powersupply.managers.serialmanager.SerialMessage;
import com.waldo.utils.ResourceManager;
import com.waldo.utils.icomponents.IFrame;

import javax.swing.*;
import java.awt.*;

import static com.waldo.powersupply.managers.serialmanager.SerialManager.serMgr;
import static com.waldo.powersupply.managers.settingsmanager.SettingsManager.settingsMgr;

public class Application extends IFrame implements SerialListener {

    private static final Application INSTANCE = new Application();
    private Application() {
    }
    public static Application app() {
        return INSTANCE;
    }

    public static String startUpPath;
    public static ResourceManager imageResource;

    public void init(String startUpPath) {
        startSerial();
        settingsMgr().loadSettings();

        Application.startUpPath = startUpPath;
        try {
            Application.imageResource = new ResourceManager("settings/", "Icons.properties");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void startSerial() {
        SerialManager.HandshakeListener handshakeListener = new SerialManager.HandshakeListener() {
            @Override
            public void onHandshakeStarted() {
                beginWait(Application.this);
            }

            @Override
            public void onHandshakeSuccess(SerialPort serialPort) {
                System.out.println("Serial port found: " + serialPort.getSystemPortName());
                SwingUtilities.invokeLater(() -> {
                    serMgr().init(Application.this);
                    serMgr().open(serialPort);
                    serMgr().registerShutDownHook();
                });
                endWait(Application.this);
            }

            @Override
            public void onHandshakeFailed(String reason) {
                JOptionPane.showMessageDialog(
                        Application.this,
                        "Failed to open Serial port: " + reason,
                        "Serial error",
                        JOptionPane.ERROR_MESSAGE
                );
                endWait(Application.this);
                Main.closeApplication(-1);
            }
        };

        serMgr().startHandshake(handshakeListener, BaudRateValues._57600, MessageTypes.PICMessageVariable);
    }

    public static boolean isUpdating(Component component) {
        return component.getCursor().getType() == Cursor.WAIT_CURSOR;
    }

    public static void beginWait(Component component) {
        component.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
    }

    public static void endWait(Component component) {
        component.setCursor(Cursor.getDefaultCursor());
    }

    //
    // GUI events
    //
    @Override
    public void initializeComponents() {
        setJMenuBar(new MenuBar());
    }

    @Override
    public void initializeLayouts() {

        JButton button = new JButton("Test");
        button.addActionListener(e -> {
            serMgr().write(SerialFactory.setVoltage(5000));
        });

        add(button);

    }

    @Override
    public void updateComponents(Object... objects) {

    }

    //
    // Serial events
    //
    @Override
    public void onSerialError(String error, Throwable throwable) {
        String message = "";
        if (error != null) {
            message = error;
        }
        if (throwable != null) {
            if (!message.isEmpty()) {
                message += "\r\n Exception: \n";
            }
            message += throwable.toString();
        }

        JOptionPane.showMessageDialog(
                Application.this,
                message,
                "Serial error",
                JOptionPane.ERROR_MESSAGE
        );
    }

    @Override
    public void onTransmitted(SerialMessage serialMessage) {

    }

    @Override
    public void onReceived(SerialMessage serialMessage) {

    }
}
