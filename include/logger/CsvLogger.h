#pragma once

#include <string>
#include <map>
#include <set>
#include <cstdint>
#include <Eigen/Dense>

/**
 * @class CsvLogger
 * @brief 单例类，用于记录程序中的数据，并最终保存为时间对齐的CSV文件。
 *
 * 内部时间戳使用 int64_t（毫秒）存储，保证不同模块的数据对齐不会受浮点误差影响。
 */
class CsvLogger {
public:
    static CsvLogger& getInstance();

    CsvLogger(const CsvLogger&) = delete;
    void operator=(const CsvLogger&) = delete;

    void setCsvPath(const std::string& path);

    /// 初始化，记录开始时间
    void init();

    // ----------------------------
    // 使用系统时间的接口
    // ----------------------------
    void update(const std::string& name, double value);
    void update(const std::string& name, int value);
    void update(const std::string& name, const Eigen::Vector3d& vec);
    void update(const std::string& name, const Eigen::VectorXd& vec);

    // ----------------------------
    // 支持 double 秒的接口
    // ----------------------------
    void update(double time_sec, const std::string& name, double value);
    void update(double time_sec, const std::string& name, int value);
    void update(double time_sec, const std::string& name, const Eigen::Vector3d& vec);
    void update(double time_sec, const std::string& name, const Eigen::VectorXd& vec);

    // ----------------------------
    // 支持 int64_t 毫秒的接口
    // ----------------------------
    void update(int64_t time_ms, const std::string& name, double value);
    void update(int64_t time_ms, const std::string& name, int value);
    void update(int64_t time_ms, const std::string& name, const Eigen::Vector3d& vec);
    void update(int64_t time_ms, const std::string& name, const Eigen::VectorXd& vec);

    void save();
    void clear();

private:
    CsvLogger();

    // 辅助函数：double 秒 -> int64 毫秒
    static int64_t timeToInt(double time_sec) { return static_cast<int64_t>(time_sec * 1e3); }

    std::string csv_path_;
    std::set<int64_t> timestamps_set_; // 使用整数毫秒时间戳
    std::map<std::string, std::map<int64_t, double>> data_;
    int64_t start_time_ms_{0}; // 记录开始时间
};
