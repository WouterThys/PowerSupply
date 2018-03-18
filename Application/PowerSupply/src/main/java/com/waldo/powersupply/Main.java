package com.waldo.powersupply;

import javax.swing.*;
import javax.swing.plaf.nimbus.NimbusLookAndFeel;
import java.awt.*;
import java.io.File;

import static com.waldo.powersupply.gui.Application.app;

public class Main {

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            app().setTitle("Power supply");
            app().setLocationByPlatform(true);
            app().setPreferredSize(new Dimension(1600, 800));
            app().setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
            setLookAndFeel(null);

            String startUpPath = new File("").getAbsolutePath() + File.separator;

            // Init
            app().init(startUpPath);
            app().initializeComponents();
            app().initializeLayouts();
            app().updateComponents();

            app().pack();
            app().setVisible(true);
        });
    }

    public static void closeApplication(int status) {
        System.exit(status);
    }

    public static void setLookAndFeel(String settingsName) {

        try {
            String lfName = "";

            //UIManager.put("nimbusBase", Color.blue);
            //UIManager.put("nimbusBlueGrey", Color.lightGray);
            //UIManager.put("control", Color.darkGray);

            NimbusLookAndFeel nimbusLookAndFeel = new NimbusLookAndFeel();
            if (settingsName != null && !settingsName.isEmpty()) {
                for (UIManager.LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
                    if (info.getName().equals(settingsName)) {
                        lfName = info.getClassName();
                        break;
                    }
                }

                if (!lfName.isEmpty()) {
                    UIManager.setLookAndFeel(lfName);
                } else {
                    UIManager.setLookAndFeel(nimbusLookAndFeel);
                }
            } else {
                UIManager.setLookAndFeel(nimbusLookAndFeel);
            }


            SwingUtilities.updateComponentTreeUI(app());
            UIManager.getLookAndFeelDefaults().put("defaultFont", new Font(Font.SANS_SERIF, Font.PLAIN, 15));
            app().pack();

        } catch (UnsupportedLookAndFeelException | IllegalAccessException | InstantiationException | ClassNotFoundException e) {
            e.printStackTrace();
        }

    }
}
