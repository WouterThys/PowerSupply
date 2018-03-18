package com.waldo.powersupply.gui.dialogs.logsdialog;

import java.awt.*;

public class LogsDialog extends LogsDialogLayout {


    public LogsDialog(Window window) {
        super(window);

        initializeComponents();
        initializeLayouts();
        updateComponents();

    }

}