#ifndef __BDALI_H__
#define __BDALI_H__

#include <Wire.h>
#include <Arduino.h>
#include "iec_62386.h"
#include <vector>

/*!ATTENTION!*/
#define NOP() asm volatile ("nop") //Workaround for delayMicrosecondeExt function

//ESP32 SDA AND SCL PIN where LW14 is connected to 
#define SDA_PIN 21
#define SCL_PIN 22

//LW14 I2C address 7Bit
#define LW14_I2C 0x23 // 8Bit: 0x46

//LW14 I2C command register
#define I2C_REG_STATUS          0x00   //read only, 1 byte
#define I2C_REG_COMMAND         0x01   //read/write, 2 bytes
#define I2C_REG_CONFIG          0x02   //write only, 1 byte
#define I2C_REG_SIGNATURE       0xF0   //read only, 6 bytes
#define I2C_REG_ADDRESS         0xFE   //write only, 2 bytes


// # 0x00 STATUS REGISTER bits:
// #   7 - Bus Error Status, 0 = OK, 1 = Bus fault
// #   6 - Busy, 0 = ready, 1 = Busy
// #   5 - Overrun
// #   4 - Frame Error
// #   3 - Valid REPLY (reset by reading 0x01)
// #   2 - Reply Timeframe, <22 Te since last command
// #   1 - 2 Bytes telegram received (reset by reading 0x01)
// #   0 - 1 Byte telegram received (reset by reading 0x01)
//Status bits for register I2C_REG_STATUS
#define STATUS_1BYTE            0x01
#define STATUS_2BYTE            0x02
#define STATUS_3BYTE            0x03
#define STATUS_TIMEFRAME        0x04
#define STATUS_VALID            0x08
#define STATUS_FRAME_ERROR      0x10
#define STATUS_OVERRUN          0x20
#define STATUS_BUSY             0x40      //0: ready, 1: busy
#define STATUS_BUS_FAULT        0x80      //0: OK, 1: bus fault

class BDali {
private:
    BDali() {}
    static BDali* instance;
public:
    static BDali* getInstance() {
        if (instance == nullptr) {
            instance = new BDali();
        }
        return instance;
    }
    void begin(int SDA_pin, int SCL_pin);
    std::vector<uint8_t> findLights();
    std::vector<uint8_t> findGroups();
    void setLightLevel(uint8_t lightNumber, uint8_t level);
    void setLightOn(uint8_t lightNumber);
    void setLightOff(uint8_t lightNumber);
    void setLightUp(uint8_t lightNumber);
    void setLightDown(uint8_t lightNumber);
    void setGroupLevel(uint8_t groupNumber, uint8_t level);
    void setFailLevel(uint8_t lightNumber, uint8_t level);
    void setMinLevel(uint8_t lightNumber, uint8_t level);
    void setMaxLevel(uint8_t lightNumber, uint8_t level);
    void setPowerOnLevel(uint8_t lightNumber, uint8_t level);
    void setGroupMemberships(uint8_t lightNumber, bool membership[16]);
    void setSceneLevels(uint8_t lightNumber, uint8_t sceneLevel[16]);
    void setGroupMembership(uint8_t lightNumber, bool membership, uint8_t group);
    void setSceneLevel(uint8_t lightNumber, uint8_t sceneLevel, uint8_t scene);
    void setFadeTime(uint8_t lightNumber, uint8_t fadeTime);
    void setFadeRate(uint8_t lightNumber, uint8_t fadeRate);
    uint8_t getLightLevel(uint8_t lightNumber);
    uint8_t getGroupLevel(uint8_t groupNumber);
    uint8_t getFailLevel(uint8_t lightNumber);
    uint8_t getMinLevel(uint8_t lightNumber);
    uint8_t getMaxLevel(uint8_t lightNumber);
    uint8_t getPowerOnLevel(uint8_t lightNumber);
    bool* getGroupMembership(uint8_t lightNumber);
    uint8_t* getSceneLevels(uint8_t lightNumber);
    uint8_t getPhysMinLevel(uint8_t lightNumber);
    uint8_t getFadeTime(uint8_t lightNumber);
    uint8_t getFadeRate(uint8_t lightNumber);
    void set_searchaddr(uint32_t adr);
    void set_searchaddr_diff(uint32_t adr_new,uint32_t adr_current);
    uint8_t compare();
    void program_short_address(uint8_t shortadr);
    uint8_t query_short_address();
    uint32_t find_addr();
    uint8_t commission(uint8_t onlyNew);
};
#endif // __BDALI_H__