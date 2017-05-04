package com.waldo.supply;

import com.waldo.supply.gui.Application;

import javax.swing.*;
import java.awt.*;

public class Main {


    public static void main(String[] args) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                Application app = new Application();
                app.setTitle("Inventory");
                app.setMinimumSize(new Dimension(1500,800));
                app.setLocationRelativeTo(null);
                app.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
                app.setVisible(true);
            }
        });
    }

}
