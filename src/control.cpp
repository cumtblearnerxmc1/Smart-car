/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-10-10 09:02:10
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2025-04-05 09:02:37
 * @FilePath: /smartcar/src/control.cpp
 * @Description:
 *
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */
#include "control.h"

#include <iostream>

#include "GPIO.h"
#include "MotorController.h"
#include "global.h"

MotorController* motorController[2] = { nullptr, nullptr };
GPIO mortorEN(73);
double mortor_kp = 1000;
double mortor_ki = 300;
double mortor_kd = 0;

void ControlInit()
{
    mortorEN.setDirection("out");
    mortorEN.setValue(1);
    const int pwmchip[2] = { 8, 8 };
    const int pwmnum[2] = { 2, 1 };
    const int gpioNum[2] = { 12, 13 };
    const int encoder_pwmchip[2] = { 0, 3 };
    const int encoder_gpioNum[2] = { 75, 72 };
    const int encoder_dir[2] = { 1, -1 };
    const unsigned int period_ns = 50000; // 20 kHz

    for (int i = 0; i < 2; ++i) {
        motorController[i] = new MotorController(pwmchip[i], pwmnum[i], gpioNum[i], period_ns,
            mortor_kp, mortor_ki, mortor_kd, 0,
            encoder_pwmchip[i], encoder_gpioNum[i], encoder_dir[i]);
    }
}

void ControlMain()
{
    if (readFlag(start_file)) {
        for (int i = 0; i < 2; ++i) {
            motorController[i]->updateduty(target_speed);

            // motorController[i]->pidController.setPID(mortor_kp, mortor_ki, mortor_kd);
            // motorController[i]->updateTarget(target_speed);
            // motorController[i]->updateSpeed();
        }
        mortorEN.setValue(1);
    } else {
        for (int i = 0; i < 2; ++i) {
            motorController[i]->updateduty(0);
        }
        mortorEN.setValue(0);
    }
    return;
}

void ControlExit()
{
    for (int i = 0; i < 2; ++i) {
        delete motorController[i];
        std::cout << "motor" << i << "deleted\n";
    }
    mortorEN.setValue(0);
}
