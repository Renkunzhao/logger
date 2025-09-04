#include "logger/CsvLogger.h"
#include <cmath> // For sin, cos
#include <iostream>

// 为了在示例中使用M_PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 模拟一个高频任务
void fast_loop_task(double time) {
    // 从任何地方获取saver实例
    CsvLogger& saver = CsvLogger::getInstance();

    double fast_signal = sin(2 * M_PI * 10 * time); // 10Hz的正弦信号
    saver.update(time, "fast_signal", fast_signal);

    int status_code = static_cast<int>(time * 10) % 5;
    saver.update(time, "status_code", status_code);
}

// 模拟一个低频任务
void slow_loop_task(double time) {
    // 同样可以获取到同一个saver实例
    CsvLogger& saver = CsvLogger::getInstance();

    Eigen::Vector3d com_position;
    com_position << 0.5 * cos(2 * M_PI * 1 * time),  // 1Hz的圆形轨迹
                    0.5 * sin(2 * M_PI * 1 * time),
                    1.0 + 0.1 * sin(2 * M_PI * 2 * time); // z轴上2Hz的振动
    
    saver.update(time, "com_position", com_position);
}


int main() {
    // 1. 在程序开始时，获取saver实例并设置输出路径。
    // 这一步只需要做一次。
    CsvLogger& saver = CsvLogger::getInstance();
    saver.setCsvPath("controller_data.csv");

    // 2. 模拟一个控制循环，持续1秒，步长为0.001秒 (1000 Hz)
    const double dt = 0.001;
    const double duration = 1.0;
    const int total_steps = static_cast<int>(duration / dt);

    std::cout << "开始模拟数据记录..." << std::endl;

    for (int i = 0; i <= total_steps; ++i) {
        double current_time = i * dt;

        // --- 高频任务 ---
        // 假设这个任务在每个循环都会执行 (1000 Hz)
        fast_loop_task(current_time);

        // --- 低频任务 ---
        // 假设这个任务每10个循环执行一次 (100 Hz)
        if (i % 10 == 0) {
            slow_loop_task(current_time);
        }
    }
    
    std::cout << "模拟结束，正在保存数据..." << std::endl;

    // 3. 在程序结束时，调用save()函数将所有记录的数据写入文件。
    saver.save();
    
    return 0;
}

