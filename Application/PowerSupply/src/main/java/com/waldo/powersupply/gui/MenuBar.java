package com.waldo.powersupply.gui;

import com.waldo.powersupply.Main;
import com.waldo.powersupply.gui.dialogs.logsdialog.LogsDialog;
import com.waldo.powersupply.gui.dialogs.settingsdialog.SettingsDialog;

import javax.swing.*;
import java.awt.event.ActionListener;

import static com.waldo.powersupply.gui.Application.app;
import static com.waldo.powersupply.gui.Application.imageResource;

public class MenuBar extends JMenuBar {

    MenuBar() {
        super();

        // File menu
        JMenu fileMenu = new JMenu("File");

        JMenuItem settingsItem = new JMenuItem("Settings", imageResource.readImage("Gears.s"));
        settingsItem.addActionListener(settingsSelected());
        JMenuItem logItem = new JMenuItem("Logs", imageResource.readImage("History.s"));
        logItem.addActionListener(logSelected());

        JMenuItem closeItem = new JMenuItem("Exit", imageResource.readImage("Exit.s"));
        closeItem.addActionListener(e -> Main.closeApplication(1));

        fileMenu.add(settingsItem);
        fileMenu.add(logItem);
        fileMenu.addSeparator();
        fileMenu.add(closeItem);

        // Other menus
        //..


        add(fileMenu);
        // add others
    }

    private ActionListener settingsSelected() {
        return e -> {
            new SettingsDialog(app()).showDialog();
        };
    }

    private ActionListener logSelected() {
        return e -> {
            new LogsDialog(app()).showDialog();
        };
    }
}
