package com.waldo.powersupply.managers.settingsmanager;

public class LcdSettings extends Settings {

    private int contrast = 40;
    private int brightness = 3;

    LcdSettings() {

    }

    public int getContrast() {
        return contrast;
    }

    public void setContrast(int contrast) {
        this.contrast = contrast;
    }

    public int getBrightness() {
        return brightness;
    }

    public void setBrightness(int brightness) {
        this.brightness = brightness;
    }
}
