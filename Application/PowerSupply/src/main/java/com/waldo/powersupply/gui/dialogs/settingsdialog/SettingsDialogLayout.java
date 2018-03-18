package com.waldo.powersupply.gui.dialogs.settingsdialog;

import com.waldo.powersupply.gui.dialogs.settingsdialog.panels.LcdSettingsPanel;
import com.waldo.utils.icomponents.IDialog;

import javax.swing.*;
import java.awt.*;

abstract class SettingsDialogLayout extends IDialog {

    /*
     *                  COMPONENTS
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    private JTabbedPane tabbedPane;
    private LcdSettingsPanel lcdSettingsPanel;

    /*
     *                  VARIABLES
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


    /*
     *                  CONSTRUCTOR
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    SettingsDialogLayout(Window window) {
        super(window, "Settings");

    }

    /*
     *                   METHODS
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


    /*
     *                  LISTENERS
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    @Override
    public void initializeComponents() {
        tabbedPane = new JTabbedPane();
        lcdSettingsPanel = new LcdSettingsPanel();

        tabbedPane.addTab("Lcd", lcdSettingsPanel);

    }

    @Override
    public void initializeLayouts() {
        getContentPanel().setLayout(new BorderLayout());

        getContentPanel().add(tabbedPane, BorderLayout.CENTER);

        pack();
    }

    @Override
    public void updateComponents(Object... args) {
        if (args.length > 0 && args[0] != null) {

        } else {

        }
    }
}