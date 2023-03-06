#include "bendali.h"

// create dummy frame
uint16_t Mockup(uint8_t param1,uint8_t param2)
{
    uint16_t value;
    value= param1*0x100+param2;
    return value;
}
//delayMicroseconds on ESP32 faulty, workaround 
void IRAM_ATTR delayMicrosecondsExt(uint32_t us)
{
    uint32_t m = micros(); 
    if(us)
    {    
        while((uint32_t)(micros() - m) < us) 
        {
            NOP();
        }
    }
}

//Set I2C register to read from
void SetI2CReadRegister(uint8_t reg)
{
    Wire.beginTransmission(LW14_I2C);
    Wire.write(reg);
    Wire.endTransmission();
}

//Clear some flags by reading Command Register
void clear_status()
{
    SetI2CReadRegister(I2C_REG_COMMAND);    //Set I2C register to read from
    Wire.requestFrom(LW14_I2C, 3);          //Read N-Bytes from I2C register
    Wire.read();
}

// bool status(){
//     bool result;
//     uint8_t receipt;
//     SetI2CReadRegister(I2C_REG_COMMAND);    //Set I2C register to read from
//     Wire.requestFrom(LW14_I2C, 3);          //Read N-Bytes from I2C register
//     receipt = Wire.read();
// }

//Generate DALI short address, DALI address format: 0 AAA AAA S
uint8_t GetShortAddress(uint8_t dali_adr, uint8_t mode)
{
    return ((DA_SHORT_BIT | ((dali_adr & DA_MAX_SHORT) << 1)) | mode);
}

//Generate DALI group address, DALI address format: 0 AAA AAA S
uint8_t GetGroupAddress(uint8_t dali_adr, uint8_t mode)
{
    return ((DA_GROUP_BIT | ((dali_adr & DA_MAX_GROUP) << 1)) | mode);
}

//Generate DALI broadcast address, DALI address format: 0 AAA AAA S
uint8_t GetBroadcastAddress(uint8_t mode)
{
    return ((DA_GROUP_BIT | (0x3F << 1)) | mode);
}

//Wait until DALI bus is ready to send next data. DALI is slow!
void WaitForReady()
{
    uint8_t result = 0x00;

    while (true)
    {
        SetI2CReadRegister(I2C_REG_STATUS);

        Wire.requestFrom(LW14_I2C, 1);
        result = Wire.read();

        if ((result & STATUS_BUSY) == 0x00) //exit if busyflag is gone
            break;

        delayMicrosecondsExt(DA_TE);
    }
}

//Wait for query result from slave device
uint8_t WaitForReply()
{
    uint8_t result = 0x00;

    while (true)
    {
        SetI2CReadRegister(I2C_REG_STATUS);

        // Wire.requestFrom(LW14_I2C, 1, false);
        Wire.requestFrom(static_cast<uint16_t>(LW14_I2C), static_cast<size_t>(1), false);
        result = Wire.read();

        //Continue if bus is not ready
        if((result & (STATUS_BUSY)) == STATUS_BUSY)
        {
            delayMicrosecondsExt(DA_TE);
            continue;
        }

        if ((result & (STATUS_VALID | STATUS_1BYTE)) == (STATUS_VALID | STATUS_1BYTE))
            return result;
        else if (result == 0x00) //No device
            return 0;

        delayMicrosecondsExt(DA_TE);
    }

    return result;
}

//Send 8bit command into the DALI bus
void SendCommand(uint8_t dali_adr, uint8_t value)
{
    Wire.beginTransmission(LW14_I2C);
    Wire.write(I2C_REG_COMMAND); //Register: command
    Wire.write((uint8_t)dali_adr);            //DALI address
    Wire.write((uint8_t)value);  //Value
    Wire.endTransmission();
}


//Send value to all DALI devices (for special commands like 'TERMINATE', 'DTR', etc)
void SendTwice(uint8_t byte1, uint8_t byte2)
{
    Wire.beginTransmission(LW14_I2C);
    Wire.write(I2C_REG_COMMAND);        //Register: command
    Wire.write((uint8_t)byte1);         //DALI address
    Wire.write((uint8_t)byte2);         //Value
    Wire.endTransmission();
    WaitForReady();
    Wire.beginTransmission(LW14_I2C);
    Wire.write(I2C_REG_COMMAND);        //Register: command
    Wire.write((uint8_t)byte1);         //DALI address
    Wire.write((uint8_t)byte2);         //Value
    Wire.endTransmission();

}

//Store value into DTR1 or DTR2
void SendToDTR(uint8_t dtr, uint8_t value)
{
    Wire.beginTransmission(LW14_I2C);
    Wire.write(I2C_REG_COMMAND);        //Register: command
    Wire.write((uint8_t) dtr);          // DTR, DTR1 or DTR2
    Wire.write((uint8_t) value);        //Value
    Wire.endTransmission();
}

//Get value from QUERY command by DALI slave address
uint8_t GetQuery(uint8_t dali_adr, uint8_t cmd)
{
    WaitForReady();                     //Wait for bus ready
    SendCommand(dali_adr, cmd);         //Send query command
    uint8_t res = WaitForReply();       //Wait for data

    //No one resonse, wrong DALI address
    if(res == 0x00)
        return res;

    //Set register to read from
    Wire.beginTransmission(LW14_I2C);
    Wire.write(I2C_REG_COMMAND);
    Wire.endTransmission();

    //Read from register
    Wire.requestFrom((uint8_t)LW14_I2C, (uint8_t)1);
    return Wire.read();
}

BDali::BDali() {
     // constructor code
}

void BDali::begin(int SDA_pin, int SCL_pin) {
        Wire.begin(SDA_pin,SCL_pin);
    // initialization code
}


std::vector<uint8_t> BDali::findLights() {
// searches for lights already commissioned with a short address
  std::vector<uint8_t> shortAddresses;
  uint8_t query;
  SendTwice(DA_EXT_INITIALISE,0);
  delay(100);

  for(uint8_t i=0x01; i<=0x7F; i+=0x02) {
    query = GetQuery(i,DA_QUERY_CONTROL_GEAR);
    if (query == 0xFF) {
      shortAddresses.push_back(i>>1);
    }
  }

  SendCommand(DA_EXT_TERMINATE,0);

  return shortAddresses;
};

   // code to find groups on dali bus
std::vector<uint8_t> BDali::findGroups() {
    std::vector<uint8_t> groupNumbers;
    std::vector<uint8_t> shortAddresses = this->findLights();
    
    // Loop through each light and check its group membership
    for (int i = 0; i < shortAddresses.size(); i++) {
        uint8_t lightNumber = shortAddresses[i];
        bool* membership = this->getGroupMembership(lightNumber);
        
        // Check each group number to see if the light is a member
        for (int j = 0; j < 16; j++) {
            if (membership[j]) {
                groupNumbers.push_back(j);
            }
        }
        
        // Free the memory used by the membership array
        delete[] membership;
    }

    // Remove duplicates from the groupNumbers vector
    std::sort(groupNumbers.begin(), groupNumbers.end());
    auto last = std::unique(groupNumbers.begin(), groupNumbers.end());
    groupNumbers.erase(last, groupNumbers.end());

    // Return the list of unique group numbers
    return groupNumbers;
}


void BDali::setLightLevel(uint8_t lightNumber, uint8_t level) {
    // code to set the level of a specific light
    uint8_t shortAddress;
    shortAddress = GetShortAddress(lightNumber,DA_MODE_DACP);
    SendCommand(shortAddress,level);
};


void BDali::setLightOn(uint8_t lightNumber) {
    // code to turn a specific light back on to last known level
    uint8_t shortAddress;
    shortAddress = GetShortAddress(lightNumber,DA_MODE_COMMAND);
    SendCommand(shortAddress,DA_RECALL_LAST_LEVEL);
};

void BDali::setLightOff(uint8_t lightNumber) {
    // code to set the level of a specific light
    uint8_t shortAddress;
    shortAddress = GetShortAddress(lightNumber,DA_MODE_COMMAND);
    SendCommand(shortAddress,DA_OFF);
};

void BDali::setLightUp(uint8_t lightNumber) {
    // code to turn a specific light on and step up
    uint8_t shortAddress;
    shortAddress = GetShortAddress(lightNumber,DA_MODE_COMMAND);
    SendCommand(shortAddress,DA_ON_STEP_UP);
};

void BDali::setLightDown(uint8_t lightNumber) {
    // code to step down a specific light and turn off
    uint8_t shortAddress;
    shortAddress = GetShortAddress(lightNumber,DA_MODE_COMMAND);
    WaitForReady();
    SendCommand(shortAddress,DA_STEP_DOWN_OFF);
};

void BDali::setGroupLevel(uint8_t groupNumber, uint8_t level) {
    // code to set the level of a specific group of lights
    uint8_t shortAddress;
    shortAddress = GetGroupAddress(groupNumber,DA_MODE_DACP);
    SendCommand(shortAddress,level);
};

void BDali::setFailLevel(uint8_t lightNumber, uint8_t level) {
  // set the fail level for the specified light number to the specified level
  SendTwice(DA_EXT_INITIALISE,0);
  delay(100);
    // Set the value of DTR0 to the desired level
    SendToDTR(DA_EXT_DTR0, level);

    // Save the value of DTR0 as the failure level for the specified light
    uint8_t shortAddress = GetShortAddress(lightNumber, DA_MODE_COMMAND);
    SendTwice(shortAddress, DA_DTR_SYS_FAIL_LEVEL);
    SendCommand(DA_EXT_TERMINATE,0);
};

void BDali::setMinLevel(uint8_t lightNumber, uint8_t level) {
  // set the minimum level for the specified light number to the specified level
  SendTwice(DA_EXT_INITIALISE,0);
  delay(100);
    // Set the value of DTR0 to the desired level
    SendToDTR(DA_EXT_DTR0, level);

    // Save the value of DTR0 as the minimum level for the specified light
    uint8_t shortAddress = GetShortAddress(lightNumber, DA_MODE_COMMAND);
    SendTwice(shortAddress, DA_DTR_MIN_LEVEL);
    SendCommand(DA_EXT_TERMINATE,0);
};

void BDali::setMaxLevel(uint8_t lightNumber, uint8_t level) {
  // set the maximum level for the specified light number to the specified level
  SendTwice(DA_EXT_INITIALISE,0);
  delay(100);
    // Set the value of DTR0 to the desired level
    SendToDTR(DA_EXT_DTR0, level);

    // Save the value of DTR0 as the maximum level for the specified light
    uint8_t shortAddress = GetShortAddress(lightNumber, DA_MODE_COMMAND);
    SendTwice(shortAddress, DA_DTR_MAX_LEVEL);
    SendCommand(DA_EXT_TERMINATE,0);
};

void BDali::setPowerOnLevel(uint8_t lightNumber, uint8_t level) {
  // set the power-on level for the specified light number to the specified level
  SendTwice(DA_EXT_INITIALISE,0);
  delay(100);
    // Set the value of DTR0 to the desired level
    SendToDTR(DA_EXT_DTR0, level);

    // Save the value of DTR0 as the minimum level for the specified light
    uint8_t shortAddress = GetShortAddress(lightNumber, DA_MODE_COMMAND);
    SendTwice(shortAddress, DA_DTR_MIN_LEVEL);
    SendCommand(DA_EXT_TERMINATE,0);
};

void BDali::setGroupMembership(uint8_t lightNumber, bool membership[16]) {
  // set the group membership values for the specified light number to the values in the specified array
  SendTwice(DA_EXT_INITIALISE,0);
  delay(100);
  uint8_t shortAddress = GetShortAddress(lightNumber, DA_MODE_COMMAND);
  for (int i = 0; i < 16; i++) {
    if (membership[i]) {
      SendTwice(shortAddress, DA_ADD_TO_GROUP + i);
    } else {
      SendTwice(shortAddress, DA_REMOVE_FROM_GROUP + i);
    };
  };
  SendCommand(DA_EXT_TERMINATE,0);
}

void BDali::setSceneLevels(uint8_t lightNumber, uint8_t sceneLevel[16]) {
  // set the scene levels for the specified light number to the values in the specified array
  SendTwice(DA_EXT_INITIALISE, 0);
  delay(100);
  uint8_t shortAddress = GetShortAddress(lightNumber, DA_MODE_COMMAND);
  for (int i = 0; i < 16; i++) {
    if (sceneLevel[i] == 0xFF) {
      SendTwice(shortAddress, DA_REMOVE_FROM_SCENE + i);
    } else {
      SendToDTR(DA_EXT_DTR0, sceneLevel[i]);
      SendTwice(shortAddress, DA_DTR_AS_SCENE + i);
    }
  }
  SendCommand(DA_EXT_TERMINATE, 0);
}

void BDali::setFadeTime(uint8_t lightNumber, uint8_t fadeTime) {
  // set the fade time for the specified light number to the specified value
  SendTwice(DA_EXT_INITIALISE, 0);
  delay(100);
  uint8_t shortAddress = GetShortAddress(lightNumber, DA_MODE_COMMAND);
  SendToDTR(DA_EXT_DTR0, fadeTime);
  SendTwice(shortAddress, DA_DTR_FADE_TIME);
  SendCommand(DA_EXT_TERMINATE, 0);
}

void BDali::setFadeRate(uint8_t lightNumber, uint8_t fadeRate) {
  // set the fade rate for the specified light number to the specified value
  SendTwice(DA_EXT_INITIALISE, 0);
  delay(100);
  uint8_t shortAddress = GetShortAddress(lightNumber, DA_MODE_COMMAND);
  SendToDTR(DA_EXT_DTR0, fadeRate);
  SendTwice(shortAddress, DA_DTR_FADE_RATE);
  SendCommand(DA_EXT_TERMINATE, 0);
}


// GETTERZ

uint8_t BDali::getLightLevel(uint8_t lightNumber) {
    uint8_t level;
    uint8_t shortAddress;
    shortAddress = GetShortAddress(lightNumber,DA_MODE_COMMAND);
    WaitForReady();
    level = GetQuery(shortAddress,DA_QUERY_ACTUAL_LEVEL);
    return level;
}

uint8_t BDali::getGroupLevel(uint8_t groupNumber) {
    uint8_t level;
    uint8_t shortAddress;
    shortAddress = GetGroupAddress(groupNumber,DA_MODE_COMMAND);
    level = GetQuery(shortAddress,DA_QUERY_ACTUAL_LEVEL);
    return level;
}

uint8_t BDali::getFailLevel(uint8_t lightNumber) {
  // return the fail level for the specified light number
    uint8_t level;
    uint8_t shortAddress;
    shortAddress = GetShortAddress(lightNumber,DA_MODE_COMMAND);
    WaitForReady();
    level = GetQuery(shortAddress,DA_QUERY_SYSTEM_FAILURE_LEVEL);
    return level;
}

uint8_t BDali::getMinLevel(uint8_t lightNumber) {
  // return the minimum level for the specified light number
    uint8_t level;
    uint8_t shortAddress;
    shortAddress = GetShortAddress(lightNumber,DA_MODE_COMMAND);
    WaitForReady();
    level = GetQuery(shortAddress,DA_QUERY_MIN_LEVEL);
    return level;
}

uint8_t BDali::getMaxLevel(uint8_t lightNumber) {
  // return the maximum level for the specified light number
    uint8_t level;
    uint8_t shortAddress;
    shortAddress = GetShortAddress(lightNumber,DA_MODE_COMMAND);
    WaitForReady();
    level = GetQuery(shortAddress,DA_QUERY_MAX_LEVEL);
    return level;
}

uint8_t BDali::getPowerOnLevel(uint8_t lightNumber) {
  // return the power-on level for the specified light number
    uint8_t level;
    uint8_t shortAddress;
    shortAddress = GetShortAddress(lightNumber,DA_MODE_COMMAND);
    WaitForReady();
    level = GetQuery(shortAddress,DA_QUERY_POWER_ON_LEVEL);
    return level;
}

bool* BDali::getGroupMembership(uint8_t lightNumber) {
  // return a pointer to the array of group membership values for the specified light number
  // ...
    uint8_t shortAddress = GetShortAddress(lightNumber, DA_MODE_COMMAND);
    uint8_t groupMembership[2];

    groupMembership[0] = GetQuery(shortAddress, DA_QUERY_GROUPS_0_TO_7);
    groupMembership[1] = GetQuery(shortAddress, DA_QUERY_GROUPS_8_TO_15);

    bool* result = new bool[16];
    for (int i = 0; i < 16; i++) {
        uint8_t byte = groupMembership[i / 8];
        uint8_t bit = 1 << (i % 8);
        result[i] = (byte & bit) != 0;
    }
    return result;
}

uint8_t* BDali::getSceneLevels(uint8_t lightNumber) {
  // return a pointer to the array of scene levels for the specified light number
  // ...
    uint8_t shortAddress = GetShortAddress(lightNumber, DA_MODE_COMMAND);
    uint8_t* levels = new uint8_t[16];

    for (int i = 0; i < 16; i++) {
        levels[i] = GetQuery(shortAddress, DA_QUERY_SCENE_LEVEL + i);
    }

    return levels;
}

    /** next address to program on commissioning. When commissioning finished, reflects number of ballasts found. */
    byte nextShortAddress;
    
    /** When true, only ballasts without short address set are commissioned. */
    bool commissionOnlyNew;

    /** commissioning state machine states */
    enum commissionStateEnum { 
      COMMISSION_OFF, COMMISSION_INIT, COMMISSION_INIT2, COMMISSION_WRITE_DTR, COMMISSION_REMOVE_SHORT, COMMISSION_REMOVE_SHORT2, COMMISSION_RANDOM, COMMISSION_RANDOM2, COMMISSION_RANDOMWAIT,
      COMMISSION_STARTSEARCH, COMMISSION_SEARCHHIGH, COMMISSION_SEARCHMID, COMMISSION_SEARCHLOW,
      COMMISSION_COMPARE, COMMISSION_CHECKFOUND, COMMISSION_PROGRAMSHORT,
      COMMISSION_VERIFYSHORT, COMMISSION_VERIFYSHORTRESPONSE, COMMISSION_QUERYDEVICETYPE, COMMISSION_QUERYDEVICETYPERESPONSE,
      COMMISSION_WITHDRAW, COMMISSION_TERMINATE
    };
    commissionStateEnum commissionState = COMMISSION_OFF; /**< current state of commissioning state machine */


void BDali::commission(byte startAddress = 0, bool onlyNew = false) {
  nextShortAddress = startAddress;
  commissionOnlyNew = onlyNew;
  
  // start commissioning
  commissionState = COMMISSION_INIT;
}
//for testing purposes:
// enum CommissionState {
//   COMMISSION_INIT,
//   COMMISSION_INIT2,
//   COMMISSION_SEARCHHIGH,
//   COMMISSION_SEARCHMID,
//   COMMISSION_SEARCHLOW,
//   COMMISSION_COMPARE,
//   COMMISSION_CHECKFOUND,
//   COMMISSION_OFF
// };
#define MAX_DALI_ADDRESSES 64 // maximum number of Dali addresses

void BDali::commission_tick() {
  static byte searchIterations;
  static unsigned long currentSearchAddress;
  static unsigned long daliAddresses[MAX_DALI_ADDRESSES];
  static int numAddressesFound = 0;

  while (true)
   {  // wait until bus is idle
    switch (commissionState) {
      case COMMISSION_INIT:
        sendSpecialCmd(CMD_INITIALISE, 0);
        commissionState = COMMISSION_INIT2;
        break;
      case COMMISSION_INIT2:
        sendSpecialCmd(CMD_INITIALISE, 0);
        commissionState = COMMISSION_SEARCHHIGH;
        break;
      case COMMISSION_SEARCHHIGH:
        // Try each possible value of the high byte (B1)
        for (byte b1 = 0x00; b1 <= 0xFF; b1++) {
          currentSearchAddress = (b1 << 16) | 0x00FF00;
          sendSpecialCmd(CMD_SEARCHADDRH, b1);
          commissionState = COMMISSION_SEARCHMID;
          break;
        }
      case COMMISSION_SEARCHMID:
        // Try each possible value of the middle byte (B2)
        for (byte b2 = 0x00; b2 <= 0xFF; b2++) {
          currentSearchAddress = (currentSearchAddress & 0xFF00FF) | (b2 << 8);
          sendSpecialCmd(CMD_SEARCHADDRM, b2);
          commissionState = COMMISSION_SEARCHLOW;
          break;
        }
      case COMMISSION_SEARCHLOW:
        // Try each possible value of the low byte (B3)
        for (byte b3 = 0x00; b3 <= 0xFF; b3++) {
          currentSearchAddress = (currentSearchAddress & 0xFFFF00) | b3;
          sendSpecialCmd(CMD_SEARCHADDRL, b3);
          commissionState = COMMISSION_COMPARE;
          break;
        }
      case COMMISSION_COMPARE:
        sendSpecialCmd(CMD_COMPARE);
        commissionState = COMMISSION_CHECKFOUND;
        break;
      case COMMISSION_CHECKFOUND:
        {  // create scope for response variable
        int response = DaliBus.getLastResponse();
        if (response != DALI_RX_EMPTY) {
          daliAddresses[numAddressesFound++] = currentSearchAddress;
        }
        if (searchIterations >= 0xFFFFFF) { // all addresses searched
          commissionState = COMMISSION_OFF;
        } else {
          searchIterations++;
          currentSearchAddress = searchIterations;
          commissionState = COMMISSION_SEARCHHIGH;
        }
        break;
        }
      case COMMISSION_OFF:
        for (int i = 0; i < numAddressesFound; i++) {
          Serial.print("Dali device found with address 0x");
          Serial.print(daliAddresses[i], HEX);
          Serial.println();
        }
        numAddressesFound = 0;
        break;
    }
  }
}
