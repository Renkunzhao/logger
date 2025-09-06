#include "logger/CsvLogger.h"
#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 高频任务 (例如 1000 Hz)
void fast_loop_task(double t) {
    CsvLogger& saver = CsvLogger::getInstance();

    double fast_signal = sin(2 * M_PI * 10 * t); // 10Hz正弦信号
    saver.update("fast_signal", fast_signal);

    int status_code = static_cast<int>(t * 10) % 5;
    saver.update("status_code", status_code);
}

// 低频任务 (例如 100 Hz)
void slow_loop_task(double t) {
    CsvLogger& saver = CsvLogger::getInstance();

    Eigen::Vector3d com_position;
    com_position << 0.5 * cos(2 * M_PI * 1 * t),  // 1Hz圆形轨迹
                    0.5 * sin(2 * M_PI * 1 * t),
                    1.0 + 0.1 * sin(2 * M_PI * 2 * t); // z轴2Hz振动
    saver.update("com_position", com_position);
}

int main() {
    CsvLogger& saver = CsvLogger::getInstance();
    saver.setCsvPath("plot/controller_data.csv");

    const double dt = 0.001; // 1ms步长 -> 1000Hz
    const int total_steps = 1000;

    std::cout << "开始模拟数据记录..." << std::endl;

    auto start_time = std::chrono::steady_clock::now();

    for (int i = 0; i < total_steps; ++i) {
        // 当前逻辑时间
        double t = i * dt;

        // 高频任务
        fast_loop_task(t);

        // 低频任务每10步执行一次
        if (i % 10 == 0) {
            slow_loop_task(t);
        }

        // --- 控制循环频率 ---
        auto next_time = start_time + std::chrono::duration<double>(dt * (i + 1));
        std::this_thread::sleep_until(next_time);
    }

    std::cout << "模拟结束，正在保存数据..." << std::endl;
    saver.save();

    return 0;
}
