package com.example.doa_crnn_android_app;

/**
 * Created by Abdullah Kucuk on 10/4/18.
 */

import android.view.animation.Animation;
import android.view.animation.Transformation;



public class Circleanimation extends Animation {

    private Circle circle;

    private float oldAngle;
    private float newAngle;

    public Circleanimation(Circle circle, float newAngle) {
        this.oldAngle = circle.getAngle();
        this.newAngle = newAngle;
        this.circle = circle;
    }

    @Override
    public void applyTransformation(float interpolatedTime, Transformation transformation) {
        float angle = oldAngle + ((newAngle - oldAngle) * interpolatedTime);

        //circle.;
        circle.setAngle(angle);
        circle.requestLayout();
    }




}
