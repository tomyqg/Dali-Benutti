#ifndef DLIGHT_H
#define DLIGHT_H

#include <Arduino.h>
#include "bdali.h"

/**
 * @brief class that represents a single luminary
 * @param bdali link to the bdali singleton instance
 * @param shortAddress unique id on the dali bus
 * @param name userfriendly name to use in the front-end
 * @param room userfriendly place to use in the front-end
 * @param minLevel minimum level of the luminary (boundaries: physmin - maxLevel)
 * @param maxLevel maximum level of the luminary (boundaries: minLevel - 254)
 * @param groups[] array of 16 bools representing membership of group 0-15
 * @param sceneLevels[] array of 16 uint8_t's representing light levels for scene 0-15 with 255 being masked (0xFF)
 * @param failLevel level luminary has on failure
 * @param powerOnLevel level luminary has on power on
 * @param physmin physical minimum level for luminary (driver)
 * @param fadeTime value between 0-15 representing <0.707s - 90.510s
 * @param fadeRate value between 1-15 representing 357.796 steps/s - 2.795 steps/s
 * @param level current level
 */
class DLight {
public:
  DLight(BDali* bdali, uint8_t shortAddress, String name, String room, uint8_t minLevel, uint8_t maxLevel, bool groups[], uint8_t sceneLevels[], uint8_t failLevel, uint8_t powerOnLevel, uint8_t physmin, uint8_t fadeTime, uint8_t fadeRate,uint8_t level);
  DLight();

  // Getter functions
  uint8_t getShortAddress();
  String getName();
  String getRoom();
  uint8_t getMinLevel();
  uint8_t getMaxLevel();
  bool* getGroups();
  uint8_t* getSceneLevels();
  uint8_t getFailLevel();
  uint8_t getPowerOnLevel();
  uint8_t getPhysmin();
  uint8_t getFadeTime();
  uint8_t getFadeRate();
  uint8_t getLevel();

  // Setter functions
  void setName(String newName);
  void setRoom(String newRoom);
  void setMinLevel(uint8_t newMinLevel);
  void setMaxLevel(uint8_t newMaxLevel);
  void setGroup(uint8_t index, uint8_t value);
  void setSceneLevel(uint8_t index, uint8_t value);
  void setFailLevel(uint8_t newFailLevel);
  void setPowerOnLevel(uint8_t newPowerOnLevel);
  void setPhysmin(uint8_t newPhysmin);
  void setFadeTime(uint8_t newFadeTime);
  void setFadeRate(uint8_t newFadeRate);
  void setLevel(uint8_t newLevel);

private:
  BDali& bdali;
  uint8_t shortAddress;
  String name;
  String room;
  uint8_t minLevel;
  uint8_t maxLevel;
  bool* groups;
  uint8_t* sceneLevels;
  uint8_t failLevel;
  uint8_t powerOnLevel;
  uint8_t physmin;
  uint8_t fadeTime;
  uint8_t fadeRate;
  uint8_t level;
};

#endif // DLIGHT_H
