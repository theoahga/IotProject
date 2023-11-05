package com.irc.iotproject.model;

import androidx.annotation.NonNull;

public class UnitItem {
    private final String unit;
    private Double value;
    private final String title;
    private final int image;
    private final String alias;

    public UnitItem(String title, String unit, Double value, int image, String alias){
        this.title = title;
        this.unit = unit;
        this.value = value;
        this.image = image;
        this.alias = alias;
    }

    public String getTitle() {
        return title;
    }

    @Override
    public String toString() {
        return value + " " + unit;
    }

    public void setValue(Double value){
        this.value = value;
    }

    public int getImage() {
        return image;
    }

    public String getAlias() {
        return alias;
    }
}
