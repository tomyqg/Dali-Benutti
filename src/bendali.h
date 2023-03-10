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
    // uint32_t commission(bool onlyNew = true);
};

class DLight {
public:
// DLight() {};

  DLight(BDali& bdali, uint8_t shortAddress, String name, String room, uint8_t minLevel, uint8_t maxLevel, uint8_t groups[], uint8_t sceneLevels[], uint8_t failLevel, uint8_t powerOnLevel, uint8_t physmin, uint8_t fadeTime, uint8_t fadeRate,uint8_t level)
    : bdali(bdali), shortAddress(shortAddress), name(name), room(room), minLevel(minLevel), maxLevel(maxLevel), failLevel(failLevel), powerOnLevel(powerOnLevel), physmin(physmin), fadeTime(fadeTime), fadeRate(fadeRate),level(level) {
      for (int i = 0; i < 16; i++) {
        this->groups[i] = groups[i];
        this->sceneLevels[i] = sceneLevels[i];
      }
  }
  DLight() : bdali(), shortAddress(0), name(""), room(""), minLevel(0), maxLevel(0), failLevel(0), powerOnLevel(0), physmin(0), fadeTime(0), fadeRate(0), level(0) {
    for (int i = 0; i < 16; i++) {
      this->groups[i] = 0;
      this->sceneLevels[i] = 0;
    }
  }


  // Getter functions
  uint8_t getShortAddress() {
    return shortAddress;
  }
  String getName() {
    return name;
  }
  String getRoom() {
    return room;
  }
  uint8_t getMinLevel() {
    return minLevel;
  }
  uint8_t getMaxLevel() {
    return maxLevel;
  }
  uint8_t getGroup(int index) {
    return groups[index];
  }
  uint8_t getSceneLevel(int index) {
    return sceneLevels[index];
  }
  uint8_t getFailLevel() {
    return failLevel;
  }
  uint8_t getPowerOnLevel() {
    return powerOnLevel;
  }
  uint8_t getPhysmin() {
    return physmin;
  }
  uint8_t getFadeTime() {
    return fadeTime;
  }
  uint8_t getFadeRate() {
    return fadeRate;
  }
  uint8_t getLevel() {
    level = bdali.getLightLevel(shortAddress);
    return level;
  }
  // Setter functions
  void setName(String newName) {
    name = newName;
  }
  void setRoom(String newRoom) {
    room = newRoom;
  }
  void setMinLevel(uint8_t newMinLevel) {
    minLevel = newMinLevel;
  }
  void setMaxLevel(uint8_t newMaxLevel) {
    maxLevel = newMaxLevel;
  }
  void setGroup(int index, uint8_t value) {
    groups[index] = value;
  }
  void setSceneLevel(int index, uint8_t value) {
    sceneLevels[index] = value;
  }
  void setFailLevel(uint8_t newFailLevel) {
    failLevel = newFailLevel;
  }
  void setPowerOnLevel(uint8_t newPowerOnLevel) {
    powerOnLevel = newPowerOnLevel;
  }
  void setPhysmin(uint8_t newPhysmin) {
    physmin = newPhysmin;
  }
  void setFadeTime(uint8_t newFadeTime) {
    fadeTime = newFadeTime;
  }
  void setFadeRate(uint8_t newFadeRate) {
    fadeRate = newFadeRate;
  }
  void setLevel(uint8_t newLevel) {
    level = newLevel;
  };

private:
  BDali& bdali;
  uint8_t shortAddress;
  String name;
  String room;
  uint8_t minLevel;
  uint8_t maxLevel;
  uint8_t groups[16];
  uint8_t sceneLevels[16];
  uint8_t failLevel;
  uint8_t powerOnLevel;
  uint8_t physmin;
  uint8_t fadeTime;
  uint8_t fadeRate;
  uint8_t level;
};


#endif // __BENDALI_H__