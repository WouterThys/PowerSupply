package com.waldo.powersupply.gui.dialogs.settingsdialog;


import java.awt.*;

public class SettingsDialog extends SettingsDialogLayout {


    public SettingsDialog(Window window) {
        super(window);

        initializeComponents();
        initializeLayouts();
        updateComponents();

    }

}