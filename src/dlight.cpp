#include "dlight.h"

DLight::DLight(BDali* bdali, uint8_t shortAddress, String name, String room, uint8_t minLevel, uint8_t maxLevel, bool groups[], uint8_t sceneLevels[], uint8_t failLevel, uint8_t powerOnLevel, uint8_t physmin, uint8_t fadeTime, uint8_t fadeRate,uint8_t level)
    : bdali(bdali), shortAddress(shortAddress), name(name), room(room), minLevel(minLevel), maxLevel(maxLevel), failLevel(failLevel), powerOnLevel(powerOnLevel), physmin(physmin), fadeTime(fadeTime), fadeRate(fadeRate),level(level) {
      for (int i = 0; i < 16; i++) {
        this->groups[i] = groups[i];
        this->sceneLevels[i] = sceneLevels[i];
      }
}

DLight::DLight() : bdali(BDali::getInstance()), shortAddress(0), name(""), room(""), minLevel(0), maxLevel(0), failLevel(0), powerOnLevel(0), physmin(0), fadeTime(0), fadeRate(0), level(0) {
    for (int i = 0; i < 16; i++) {
      this->groups[i] = 0;
      this->sceneLevels[i] = 0;
    }
}

uint8_t DLight::getShortAddress() {
    return shortAddress;
}

String DLight::getName() {
    return name;
}

String DLight::getRoom() {
    return room;
}

uint8_t DLight::getMinLevel() {
    return minLevel;
}

uint8_t DLight::getMaxLevel() {
    return maxLevel;
}

bool* DLight::getGroups() {
    return groups;
}

uint8_t* DLight::getSceneLevels() {
    return sceneLevels;
}

uint8_t DLight::getFailLevel() {
    return failLevel;
}

uint8_t DLight::getPowerOnLevel() {
    return powerOnLevel;
}

uint8_t DLight::getPhysmin() {
    return physmin;
}

uint8_t DLight::getFadeTime() {
    return fadeTime;
}

uint8_t DLight::getFadeRate() {
    return fadeRate;
}

uint8_t DLight::getLevel() {
    return level;
}

uint8_t DLight::getMinLevelD() {
    minLevel = bdali->getMinLevel(shortAddress);
    return minLevel;
}

uint8_t DLight::getMaxLevelD() {
    maxLevel = bdali->getMaxLevel(shortAddress);
    return maxLevel;
}

bool* DLight::getGroupsD() {
         memcpy(groups, bdali->getSceneLevels(shortAddress), 16);
    return groups;
}

uint8_t* DLight::getSceneLevelsD() {
     memcpy(sceneLevels, bdali->getSceneLevels(shortAddress), 16);
    return sceneLevels;
}

uint8_t DLight::getFailLevelD() {
    failLevel = bdali->getFailLevel(shortAddress);
    return failLevel;
}

uint8_t DLight::getPowerOnLevelD() {
    powerOnLevel = bdali->getPowerOnLevel(shortAddress);
    return powerOnLevel;
}

uint8_t DLight::getPhysminD() {
    physmin = bdali->getPhysMinLevel(shortAddress);
    return physmin;
}

uint8_t DLight::getFadeTimeD() {
    fadeTime = bdali->getFadeTime(shortAddress);
    return fadeTime;
}

uint8_t DLight::getFadeRateD() {
    fadeRate = bdali->getFadeRate(shortAddress);
    return fadeRate;
}

uint8_t DLight::getLevelD() {
    level = bdali->getLightLevel(shortAddress);
    return level;
}

// setters

void DLight::setName(String newName) {
    name = newName;
}

void DLight::setRoom(String newRoom) {
    room = newRoom;
}

void DLight::setMinLevel(uint8_t newMinLevel) {
    bdali->setMinLevel(shortAddress, newMinLevel);
    minLevel = newMinLevel;
}

void DLight::setMaxLevel(uint8_t newMaxLevel) {
    bdali->setMaxLevel(shortAddress, newMaxLevel);
    maxLevel = newMaxLevel;
}

void DLight::setGroup(uint8_t index, uint8_t value) {
    bdali->setGroupMembership(shortAddress, value, index);
    groups[index] = value;
}

void DLight::setSceneLevel(uint8_t index, uint8_t value) {
    bdali->setSceneLevel(shortAddress, value, index);
    sceneLevels[index] = value;
}

void DLight::setFailLevel(uint8_t newFailLevel) {
    bdali->setFailLevel(shortAddress, newFailLevel);
    failLevel = newFailLevel;
}

void DLight::setPowerOnLevel(uint8_t newPowerOnLevel) {
    bdali->setPowerOnLevel(shortAddress, newPowerOnLevel);
    powerOnLevel = newPowerOnLevel;
}

void DLight::setPhysmin(uint8_t newPhysmin) {
    physmin = newPhysmin;
}

void DLight::setFadeTime(uint8_t newFadeTime) {
    bdali->setFadeRate(shortAddress, newFadeTime);
    fadeTime= newFadeTime;
}

void DLight::setFadeRate(uint8_t newFadeRate) {
    bdali->setFadeRate(shortAddress, newFadeRate);
    fadeRate = newFadeRate;
}

void DLight::setLevel(uint8_t newLevel) {
    bdali->setLightLevel(shortAddress, newLevel);
    level = newLevel;
}
