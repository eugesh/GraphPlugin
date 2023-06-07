#pragma once

inline double fitAngleIn360(double angle) {
    double outAngle = angle;
    while (outAngle > 360) {
        outAngle -= 360;
    }

    while (outAngle < 0) {
        outAngle += 360;
    }

    return outAngle;
}
