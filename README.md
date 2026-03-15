# 第十六届蓝桥杯单片机备赛工程

<p align="center">
  <img alt="MCU" src="https://img.shields.io/badge/MCU-STC15F2K61S2-1f6feb">
  <img alt="Language" src="https://img.shields.io/badge/Language-C-2ea44f">
  <img alt="Status" src="https://img.shields.io/badge/Status-Training-orange">
  <img alt="Contest" src="https://img.shields.io/badge/Contest-蓝桥杯省赛-blueviolet">
</p>

> 这是我的蓝桥杯单片机备赛主工程，用于题目复现、模块联调、评测纠错与最终冲刺。

---

## 1. 项目目标

- 快速复现历届真题核心功能
- 搭建稳定的模块化驱动框架
- 形成可复用的评测排错流程
- 在赛前完成高频错误归档与修正

---

## 2. 当前工程概览

### 2.1 主控与平台

- 芯片: STC15F2K61S2
- 开发方向: 蓝桥杯单片机省赛
- 工程类型: C + Keil 工程

### 2.2 功能模块

- 温度采集: DS18B20
- 超声波测距: superwave
- 光敏采样: AD/DA
- 数码管显示: digital_tube
- 按键输入: kbd / kdb_nb
- 串口调试: uart
- 基础外设: delay / init / iic / onewire

---

## 3. 目录结构（核心）

```text
16届蓝桥杯真题/
├─ main.c                 # 主流程与状态机
├─ superwave.c            # 超声波测距
├─ DS18B20.c              # 温度采集
├─ AD_DA.c                # AD/DA 读写
├─ digital_tube.c         # 数码管显示
├─ kbd.c / kdb_nb.c       # 按键处理
├─ init.c                 # 上电初始化
├─ uart.c                 # 串口模块
├─ one_wire.c / onewire.c # 单总线实现（历史版本）
├─ project.uvproj         # Keil 工程文件
├─ Listings/              # 编译列表文件
└─ Objects/               # 编译产物
```

---

## 4. 构建与下载

1. 使用 Keil 打开 `project.uvproj`
2. 确认目标芯片与时钟配置
3. 全编译并检查 `Objects/` 产物
4. 通过 STC-ISP 下载到开发板

建议:

- 每次改动后先做单模块验证，再做整题联调
- 保留一份“可过评测”的稳定版本标签

---

## 5. 训练节奏建议

- 每日 1 题模块复盘（20-40 分钟）
- 每日 1 次完整流程仿真（含边界条件）
- 每周 1 次全真评测回归
- 每周更新一次错题与坑点记录

---

## 6. 评测排错清单

- 继电器位定义是否与板卡映射一致
- 超声波换算系数和超时逻辑是否统一
- 运动状态判定周期与采样周期是否匹配
- 参数页面锁定逻辑是否符合题面
- 统计显示是否要求前导 0
- 阈值判断是 `<` 还是 `<=` 是否一致

---

## 7. 版本记录模板

### YYYY-MM-DD

- 调整模块:
- 变更说明:
- 评测结果:
- 遗留问题:
- 下一步计划:

---

## 8. 个人备赛宣言

稳住节奏，先保正确，再提速度。
每个错误都复盘成经验，每次评测都比上一次更接近满分。

---

如果未来要扩展为多题仓库，可将每一届题目拆分到独立目录，并统一 README 风格与排错模板。