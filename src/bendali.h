#ifndef __BENDALI_H__
#define __BENDALI_H__

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
public:
    BDali();
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
    void setGroupMembership(uint8_t lightNumber, bool membership[16]);
    void setSceneLevels(uint8_t lightNumber, uint8_t sceneLevel[16]);
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
    void commission(byte startAddress = 0, bool onlyNew = false);
    void commission_tick();
};

class DLight {
  public:
    DLight() {} // Default constructor

    DLight(short shortAddress, String name, String room, int minLevel, int maxLevel, int groups[], int sceneLevels[], int failLevel, int powerOnLevel)
      : shortAddress(shortAddress), name(name), room(room), minLevel(minLevel), maxLevel(maxLevel), failLevel(failLevel), powerOnLevel(powerOnLevel) {
        for (int i = 0; i < 16; i++) {
          this->groups[i] = groups[i];
          this->sceneLevels[i] = sceneLevels[i];
        }
    }

    short shortAddress;
    String name;
    String room;
    int minLevel;
    int maxLevel;
    int groups[16];
    int sceneLevels[16];
    int failLevel;
    int powerOnLevel;
};
#endif // __BENDALI_H__