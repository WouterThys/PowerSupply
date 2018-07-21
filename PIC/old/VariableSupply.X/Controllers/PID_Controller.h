#ifndef PID_CONTROLLER_H
#define	PID_CONTROLLER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define MANUAL    0
#define AUTOMATIC 1

void pidCompute(float setPoint, float input, float * output);


/**
 * Set tuning variables of PID
 * @param sampleTime: time between samples [mS]
 * @param kp: proportional 
 * @param ki: integral [1/mS]
 * @param kd: differential [mS]
 * @param min: minimum PID output value
 * @param max: maximum PID output value
 */
void pidSetTunings(float st, float kp, float ki, float kd, float min, float max);

/**
 * Automatic mode: PID calculates output
 * Manual mode: User sets output
 * @param mode AUTOMATIC or MANUAL
 */
void pidSetMode(int mode);


#ifdef	__cplusplus
}
#endif

#endif	/* PID_CONTROLLER_H */

