#ifndef MAIN_H
#define MAIN_H

// void stepperDispense(long uL, bool forward, long uLsPerRev, int speed);
void setupAllSteppers();
void gitPrint();
void calibrateSumpPump();
void timeout();
void exhibitRoutine();
void sumpPumpDispense(int mLs);
void drainTanks();
//void calibratePump();


#endif // MAIN_H