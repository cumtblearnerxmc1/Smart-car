/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-10-10 08:23:13
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2025-04-12 11:10:10
 * @FilePath: /smartcar/main/main.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <atomic>
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "Timer.h"
#include "camera.h"
#include "control.h"
#include "global.h"
#include "vl53l0x.h"
#include "wonderEcho.h"

std::atomic<bool> running(true);
void signalHandler(int signal)
{
    running.store(false);
}

int main(void)
{
    std::signal(SIGINT, signalHandler);
    double dest_fps = readDoubleFromFile(destfps_file);
    int dest_frame_duration = CameraInit(0, dest_fps, 320, 240);
    printf("%d\n", dest_frame_duration);
    servo_mid = readDoubleFromFile(servo_mid_file);
    if (dest_frame_duration != -1) {
        wonderEchoInit();
        vl53l0xInit();
        std::cout << "Vl53l0x Service started!\n";
        streamCaptureRunning = true;
        std::thread camworker = std::thread(streamCapture);
        std::cout << "Stream Capture Service started!\n";
        ControlInit();
        std::cout << "Control Initialized!\n";

        Timer CameraTimer(dest_frame_duration, std::bind(CameraHandler));
        Timer MortorTimer(8, std::bind(ControlMain));
        CameraTimer.start();
        std::cout << "Camera Service started!\n";
        MortorTimer.start();
        std::cout << "Control Service started!\n";
        // 主循环，直到用户输入 Ctrl+C
        while (running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            target_speed = readDoubleFromFile(speed_file);
            servo_mid = readDoubleFromFile(servo_mid_file);

            mortor_kp = readDoubleFromFile(mortor_kp_file);
            mortor_ki = readDoubleFromFile(mortor_ki_file);
            mortor_kd = readDoubleFromFile(mortor_kd_file);

            kp = readDoubleFromFile(kp_file);
            ki = readDoubleFromFile(ki_file);
            kd = readDoubleFromFile(kd_file);
            // vofa_image(1, 160*120, 160, 120, Format_Grayscale8, (char*)IMG);
        }
        std::cout << "Stopping!\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        CameraTimer.stop();
        std::cout << "Camera Timer stopped!\n";
        MortorTimer.stop();
        std::cout << "Control Timer stopped!\n";

        ControlExit();
        std::cout << "Control Service stopped!\n";

        streamCaptureRunning = false;
        if (camworker.joinable()) {
            camworker.join();
        }

        cameraDeInit();
        std::cout << "Camera Service stopped!\n";

        vl53l0xStop();
        std::cout << "Vl53l0x Service stopped!\n";
    }
    return 0;
}