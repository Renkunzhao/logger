# CsvLogger 设计思路与使用说明
## 概览
CsvLogger 是一个为实时控制系统设计的、易于使用的、线程安全的C++数据记录库。它的核心目标是解决在不同模块、不同频率下记录数据，并最终生成一个时间对齐的CSV文件的需求。

## 项目结构 (库)
现在，CsvLogger 已经从一个仅头文件的实现重构为一个静态库。这是一种更标准的C++项目组织方式。
- CsvLogger.h: 类的声明（接口）。您的项目代码只需要包含这个文件。
- CsvLogger.cpp: 类的定义（实现）。它会被编译成一个库文件。
- main.cpp: 一个如何使用这个库的示例程序。
- CMakeLists.txt: 用于编译库和示例程序的配置文件。

## 核心设计决策与功能实现
这里详细解答了您在问题中提到的几个关键点。
1. 全局访问 (Singleton模式)为了实现在代码的任何地方都能访问到同一个记录器实例，我们采用了单例（Singleton）设计模式。通过静态方法 CsvLogger::getInstance() 来获取全局唯一的实例。
2. 异步与多频数据处理这是这个记录器设计的核心。通过一种“时间戳-值”的存储方式来完美解决这个问题。
- 内部数据结构
```bash
:std::set<double> timestamps_set_; // 存储所有不重复且自动排序的时间戳
std::map<std::string, std::map<double, double>> data_; // 存储每个变量在特定时间戳下的值
```
- 工作流程:
 - update(): 当您调用 update() 时，数据和时间戳被分别记录。
 - save(): 当您调用 save() 时，函数会遍历所有唯一且有序的时间戳。对于每个时间戳，它会查找所有变量的值。如果某个变量在该时间点没有记录（因为更新频率较低），则对应的单元格会留空。这完美地实现了数据的自动对齐。
3. 灵活的数据类型支持update() 函数通过C++的函数重载，原生支持 double, int 等基本类型，以及 Eigen::Vector3d 和 Eigen::VectorXd。对于向量类型，记录器会自动将其展开为多个带后缀的列（如 _x, _y, _z 或 _0, _1, ...）。

## 如何编译与运行
### 依赖
- C++17 或更高版本的编译器 (例如 g++)
- CMake (版本 3.10 或更高)
- Eigen3 库

### 编译步骤
CMakeLists.txt 文件已经配置好了所有事情。您只需要执行标准的CMake编译流程即可。
1. 将所有文件 (CsvLogger.h, CsvLogger.cpp, main.cpp, CMakeLists.txt) 放在同一个文件夹下。
2. 打开终端，进入该文件夹，并执行以下命令：
```bash
# 创建一个构建目录
mkdir build
cd build

# 运行 CMake 来配置项目
cmake ..

# 编译项目 (这会先编译 CsvLogger 库, 然后编译 main 并链接)
make
```
3. 编译成功后，build 文件夹下会生成一个名为 main 的可执行文件。

### 运行
在 build 目录下运行：
```bash
./main
```
程序会运行 main.cpp 中的模拟代码，并在项目根目录（与 build 目录同级）下生成一个名为 controller_data.csv 的文件。

## 如何在您的项目中使用
1. 将 CsvLogger.h 和 CsvLogger.cpp 文件复制到您的项目源码目录中。
2. 在您项目的 CMakeLists.txt 中，添加这两文件来构建 CsvLogger 库。
3. 让您的主目标链接到 CsvLogger 库。
4. 在需要记录数据的文件中，包含头文件 #include "CsvLogger.h" 即可开始使用。