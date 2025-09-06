clear;clc;close all;
% 读取 CSV 文件
data = readtable('dapc_data.csv');  % 替换成你的文件路径

t = data.time_s-data.time_s(1);

%% Time
% 提取列
controller_time = data.controller_time-data.controller_time(1);
sim_time = data.sim_time-data.sim_time(1);

% 画图
figure;
plot(t, controller_time, 'b-', 'LineWidth', 1.5); hold on;
plot(t, sim_time, 'r--', 'LineWidth', 1.5);
grid on;

xlabel('Time [s]');
ylabel('Time (s)');
legend('Controller Time', 'Sim Time');
title('Controller vs Simulation Time');

%% State
state = data.state;
figure
% 提取并画出状态变量
plot(t, state, 'g-', 'LineWidth', 1.5);
grid on;
xlabel('Time [s]');
ylabel('State Variable');
title('State Variable Over Time');
legend('State');

%% Height
des_base_z = data.des_base_pos_z;
real_base_z = data.real_base_pos_z;
com_z = data.com_z;

figure
% 画出期望与实际基座高度
plot(t, des_base_z, 'm-', 'LineWidth', 1.5); hold on;
plot(t, real_base_z, 'c--', 'LineWidth', 1.5);
plot(t, com_z, 'k-', 'LineWidth', 1.5);
grid on;
xlabel('Time [s]');
ylabel('Base Height [m]');
title('Desired vs Real Base Height');
legend('Desired Base Height', 'Real Base Height', 'Real COM Height');


