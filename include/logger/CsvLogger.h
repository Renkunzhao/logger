#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <Eigen/Dense>

/**
 * @class CsvLogger
 * @brief 一个单例类，用于在程序的不同位置记录数据，并最终保存为一个时间对齐的CSV文件。
 *
 * 这个类被设计用来解决在实时控制系统中，来自不同频率的模块的数据记录问题。
 */
class CsvLogger {
public:
    /**
     * @brief 获取CsvLogger的全局唯一实例。
     * @return 对CsvLogger实例的引用。
     */
    static CsvLogger& getInstance();

    // 删除拷贝构造函数和赋值运算符，以保证单例的唯一性。
    CsvLogger(const CsvLogger&) = delete;
    void operator=(const CsvLogger&) = delete;

    /**
     * @brief 设置最终输出的CSV文件路径。
     * @param path 文件路径字符串。
     */
    void setCsvPath(const std::string& path);

    /**
     * @brief 更新一个double类型的数据。
     * @param time 当前的时间戳。
     * @param name 数据的名称，将作为CSV的列标题。
     * @param value 数据的值。
     */
    void update(double time, const std::string& name, double value);

    /**
     * @brief 更新一个int类型的数据（会被转换为double存储）。
     * @param time 当前的时间戳。
     * @param name 数据的名称。
     * @param value 数据的值。
     */
    void update(double time, const std::string& name, int value);

    /**
     * @brief 更新一个Eigen::Vector3d类型的数据。
     * 它会被自动展开为三列：name_x, name_y, name_z。
     * @param time 当前的时间戳。
     * @param name 向量的名称。
     * @param vec 3D向量。
     */
    void update(double time, const std::string& name, const Eigen::Vector3d& vec);
    
    /**
     * @brief 更新一个动态大小的Eigen::VectorXd类型的数据。
     * 它会被自动展开为多列：name_0, name_1, ...
     * @param time 当前的时间戳。
     * @param name 向量的名称。
     * @param vec 动态大小的向量。
     */
    void update(double time, const std::string& name, const Eigen::VectorXd& vec);

    /**
     * @brief 将所有缓冲的数据保存到指定的CSV文件中。
     * 该函数会将不同频率记录的数据在时间轴上对齐。
     */
    void save();
    
    /**
     * @brief 清空所有已记录的数据和时间戳。
     */
    void clear();

private:
    // 私有构造函数，确保只能通过getInstance()创建实例
    CsvLogger();

    std::string csv_path_;
    std::set<double> timestamps_set_; // 使用set来存储唯一且自动排序的时间戳
    
    // 核心数据结构: map<变量名, map<时间戳, 值>>
    std::map<std::string, std::map<double, double>> data_;
};
