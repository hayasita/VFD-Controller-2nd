/**
 * @file MockSerialMonitorIO.h
 * @author hayasita04@gmail.com
 * @brief シリアル送受信処理 Mock
 * @version 0.1
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MOCK_SERIAL_MONITOR_IO_H_
#define MOCK_SERIAL_MONITOR_IO_H_

#include <cstdint>
#include <gmock/gmock.h>
#include "../../src/SerialCommandProcessor.h"

class MockSerialMonitorIO : public MonitorDeviseIo
{
  public:
    MOCK_METHOD(std::string , rsv, ());
    MOCK_METHOD(uint8_t , send, (std::string data));
};

#endif // MOCK_SERIAL_MONITOR_IO_H_