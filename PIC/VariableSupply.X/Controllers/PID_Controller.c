
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "PID_Controller.h"

float newOutput, lastInput, iTerm;
float Kp, Ki, Kd;
float outMin, outMax;
float sampleTime = 60; // In milli seconds
bool inAuto = true;

void pidCompute(float setPoint, float input, float * output) {
    if (inAuto) {
        // Compute all the working error variables
        float error = setPoint - input;

        iTerm += (Ki * error);
        if (iTerm > outMax) {
            iTerm = outMax;
        }
        if (iTerm < outMin) {
            iTerm = outMin;
        }

        float dInput = (input - lastInput);

        // PID output
        newOutput = (Kp * error) + (iTerm) - (Kd * dInput);
        if (newOutput > outMax) {
            newOutput = outMax;
        }
        if (newOutput < outMin) {
            newOutput = outMin;
        }

        // Remember
        lastInput = input;
        *output = newOutput;
    } else {
        lastInput = input;
        newOutput = input;
    }
}

void pidSetTunings(float st, float kp, float ki, float kd, float min, float max) {
    sampleTime = st;
    
    Kp = kp;
    Ki = ki * sampleTime; // Make dimensionless
    Kd = kd / sampleTime; // Make dimensionless
    
    if (min > max) return;
    outMin = min;
    outMax = max;
}

void pidSetMode(int mode) {
    bool newAuto = (mode == AUTOMATIC);
    if (newAuto && !inAuto) {
        // We went from Manual to Automatic
        iTerm = newOutput;
    }
    inAuto = newAuto;
}
