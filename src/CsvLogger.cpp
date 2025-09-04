#include "logger/CsvLogger.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

// --- CsvLogger 方法实现 ---

CsvLogger::CsvLogger() = default;

CsvLogger& CsvLogger::getInstance() {
    // C++11保证了静态局部变量的初始化是线程安全的。
    static CsvLogger instance;
    return instance;
}

void CsvLogger::setCsvPath(const std::string& path) {
    csv_path_ = path;
}

void CsvLogger::update(double time, const std::string& name, double value) {
    timestamps_set_.insert(time);
    data_[name][time] = value;
}

void CsvLogger::update(double time, const std::string& name, int value) {
    update(time, name, static_cast<double>(value));
}

void CsvLogger::update(double time, const std::string& name, const Eigen::Vector3d& vec) {
    timestamps_set_.insert(time); // 只需插入一次时间戳
    data_[name + "_x"][time] = vec.x();
    data_[name + "_y"][time] = vec.y();
    data_[name + "_z"][time] = vec.z();
}
    
void CsvLogger::update(double time, const std::string& name, const Eigen::VectorXd& vec) {
    timestamps_set_.insert(time); // 只需插入一次时间戳
    for (int i = 0; i < vec.size(); ++i) {
        data_[name + "_" + std::to_string(i)][time] = vec(i);
    }
}

void CsvLogger::save() {
    if (csv_path_.empty()) {
        std::cerr << "错误: CSV文件路径未设置。请先调用 setCsvPath()。" << std::endl;
        return;
    }

    std::ofstream file(csv_path_);
    if (!file.is_open()) {
        std::cerr << "错误: 无法打开文件: " << csv_path_ << std::endl;
        return;
    }

    // --- 1. 创建并写入表头 ---
    std::vector<std::string> headers;
    headers.push_back("time");
    for (const auto& pair : data_) {
        headers.push_back(pair.first);
    }

    for (size_t i = 0; i < headers.size(); ++i) {
        file << headers[i] << (i == headers.size() - 1 ? "" : ",");
    }
    file << "\n";

    // --- 2. 遍历所有时间戳并写入数据行 ---
    for (const double time : timestamps_set_) {
        file << std::fixed << std::setprecision(6) << time;

        for (size_t i = 1; i < headers.size(); ++i) {
            const std::string& header = headers[i];
            const auto& var_data = data_.at(header);
            auto it = var_data.find(time);

            file << ",";
            if (it != var_data.end()) {
                file << std::fixed << std::setprecision(4) << it->second;
            }
        }
        file << "\n";
    }

    file.close();
    std::cout << "数据成功保存到: " << csv_path_ << std::endl;
}
    
void CsvLogger::clear() {
    timestamps_set_.clear();
    data_.clear();
}

