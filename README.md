# Qt 数据采集与存储系统

基于 Qt 6 的 TCP 数据采集演示项目：客户端模拟设备定时上报温湿度数据，服务端接收后解析入库（MySQL），并在表格中实时展示最近 100 条记录。

## 结构

```
client/   设备模拟端（TCP 客户端）
server/   上位机端（TCP 服务端 + 数据库）
```

## 技术点

- QTcpServer / QTcpSocket 通信，用 `QList` 管理多客户端连接，`sender()` 区分数据来源
- 自定义文本协议 `device,temp,humi`，服务端按分隔符解析，`toDouble(&ok)` 校验类型
- Qt 官方预编译包不含 MySQL 驱动，改用 **QODBC** 桥接 MySQL
- `QSqlQuery` 参数化查询（`prepare` + `bindValue`）入库，避免字符串拼接与注入
- `QSqlQueryModel` + `LIMIT 100` 展示最近数据，避免全表查询
- `QTimer` 驱动 1 Hz 定时上报，设备名可在界面配置

## 构建

需要 Qt 6.5+（含 Network、Sql 模块）与 CMake 3.19+。

```bash
cd client
cmake -S . -B build -DCMAKE_PREFIX_PATH=<Qt 安装路径>
cmake --build build

cd ../server
cmake -S . -B build -DCMAKE_PREFIX_PATH=<Qt 安装路径>
cmake --build build
```

## 运行

1. 在 MySQL 中建库建表：

```sql
CREATE DATABASE device_data CHARACTER SET utf8;
USE device_data;
CREATE TABLE device (
    Id     INT PRIMARY KEY AUTO_INCREMENT,
    device VARCHAR(50),
    temp   DOUBLE,
    humi   DOUBLE
);
```

2. 修改 `server/widget.cpp` 里的连接串（库名、用户名、密码、ODBC 驱动名）。
3. 启动 server（默认监听 8888），再启动 client，地址填 `127.0.0.1`、端口 `8888`。
4. 在客户端窗口填入设备名，点「开始自动上报」。

## 说明

界面、信号槽与网络收发部分参考 Qt 入门教程；服务端的多客户端管理、协议解析、
数据库持久化与表格展示为自行扩展。
