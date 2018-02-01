package com.waldo.powersupply;

import javax.swing.*;
import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

public class Main {


    public static void main(String[] args) {

        JFrame frame = new JFrame("Power supply");
        Application application = new Application();

        frame.getContentPane().add(application, BorderLayout.CENTER);
        frame.setVisible(true);
        frame.pack();

        frame.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                System.exit(0);
            }
        });

    }

}
