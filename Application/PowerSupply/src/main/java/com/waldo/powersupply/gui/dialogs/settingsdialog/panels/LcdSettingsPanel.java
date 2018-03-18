package com.waldo.powersupply.gui.dialogs.settingsdialog.panels;

import com.waldo.powersupply.managers.serialmanager.SerialFactory;
import com.waldo.powersupply.managers.settingsmanager.LcdSettings;
import com.waldo.utils.GuiUtils;
import com.waldo.utils.GuiUtils.GuiInterface;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import java.awt.*;

import static com.waldo.powersupply.managers.serialmanager.SerialManager.serMgr;
import static com.waldo.powersupply.managers.settingsmanager.SettingsManager.settingsMgr;

public class LcdSettingsPanel extends JPanel implements GuiInterface, ChangeListener {

    /*
     *                  COMPONENTS
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    private JSlider brightnessSl;
    private JSlider contrastSl;

    /*
     *                  VARIABLES
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    private LcdSettings lcdSettings;

    /*
     *                  CONSTRUCTOR
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    public LcdSettingsPanel() {

        lcdSettings = settingsMgr().getLcdSettings();

        initializeComponents();
        initializeLayouts();
        updateComponents();
    }

    /*
     *                  METHODS
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /*
     *                  LISTENERS
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    @Override
    public void initializeComponents() {
        brightnessSl = new JSlider(JSlider.HORIZONTAL, 0, 8, 1);
        brightnessSl.setMajorTickSpacing(4);
        brightnessSl.setMinorTickSpacing(1);
        brightnessSl.setPaintTicks(true);
        brightnessSl.setPaintLabels(true);
        //brightnessSl.setMinimum(1);
        brightnessSl.addChangeListener(this);

        contrastSl = new JSlider(JSlider.HORIZONTAL, 0, 50, 1);
        contrastSl.setMajorTickSpacing(5);
        contrastSl.setMinorTickSpacing(1);
        contrastSl.setPaintTicks(true);
        contrastSl.setPaintLabels(true);
        //contrastSl.setMinimum(1);
        contrastSl.addChangeListener(this);
    }

    @Override
    public void initializeLayouts() {
        setLayout(new BorderLayout());

        JPanel sliderPnl = new JPanel();
        GuiUtils.GridBagHelper gbc = new GuiUtils.GridBagHelper(sliderPnl);
        gbc.addLine("Contrast: ", contrastSl);
        gbc.addLine("Brightness: ", brightnessSl);

        add(sliderPnl, BorderLayout.CENTER);
    }

    @Override
    public void updateComponents(Object... args) {
        brightnessSl.setValue(lcdSettings.getBrightness());
        contrastSl.setValue(lcdSettings.getContrast());
    }


    //
    // Slider changed
    //
    @Override
    public void stateChanged(ChangeEvent e) {
        JSlider source = (JSlider) e.getSource();
        if (!source.getValueIsAdjusting()) {
            int val = source.getValue();
            if (source.equals(brightnessSl) && val != lcdSettings.getBrightness()) {
                lcdSettings.setBrightness(source.getValue());
                settingsMgr().saveSettings(lcdSettings);
                serMgr().write(SerialFactory.setLcdBrightness(val));
            }

            if (source.equals(contrastSl) && val != lcdSettings.getContrast()) {
                lcdSettings.setContrast(source.getValue());
                settingsMgr().saveSettings(lcdSettings);
                serMgr().write(SerialFactory.setLcdContrast(val));
            }
        }
    }
}