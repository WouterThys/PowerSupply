package com.waldo.powersupply.managers.settingsmanager;

public class SettingsManager {

    private static final SettingsManager INSTANCE = new SettingsManager();
    private SettingsManager() {}
    public static SettingsManager settingsMgr() {
        return INSTANCE;
    }

    private LcdSettings lcdSettings;
    private GuiSettings guiSettings;

    public void loadSettings() {
        lcdSettings = new LcdSettings(); // TODO load from db
        guiSettings = new GuiSettings();
    }


    public void saveSettings(Settings settings) {
        settings.save();
    }




    public LcdSettings getLcdSettings() {
        if (lcdSettings == null) {
            loadSettings();
        }
        return lcdSettings;
    }

    public GuiSettings getGuiSettings() {
        if (guiSettings == null) {
            loadSettings();
        }
        return guiSettings;
    }
}
