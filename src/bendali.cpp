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
    WaitForReady();
    Wire.beginTransmission(LW14_I2C);
    Wire.write(I2C_REG_COMMAND); //Register: command
    Wire.write((uint8_t)dali_adr);            //DALI address
    Wire.write((uint8_t)value);  //Value
    Wire.endTransmission();
}


//Send value to all DALI devices (for special commands like 'TERMINATE', 'DTR', etc)
void SendTwice(uint8_t byte1, uint8_t byte2)
{
    WaitForReady();
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
    // WaitForReady();                     //Wait for bus ready implemented in sendcmd
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

BDali* BDali::instance = nullptr;

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

uint8_t BDali::getPhysMinLevel(uint8_t lightNumber) {
  // return the physical minimum level for the specified light number
    uint8_t level;
    uint8_t shortAddress;
    shortAddress = GetShortAddress(lightNumber,DA_MODE_COMMAND);
    WaitForReady();
    level = GetQuery(shortAddress,DA_QUERY_PHYSICAL_MIN_LEVEL);
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

uint8_t BDali::getFadeRate(uint8_t lightNumber){
  uint8_t shortAddress = GetShortAddress(lightNumber, DA_MODE_COMMAND);
  uint8_t result = GetQuery(shortAddress,DA_QUERY_FADE_TIME_RATE)& 0x0F;
  return result;
}

uint8_t BDali::getFadeTime(uint8_t lightNumber){
  uint8_t shortAddress = GetShortAddress(lightNumber, DA_MODE_COMMAND);
  uint8_t result = ((GetQuery(shortAddress,DA_QUERY_FADE_TIME_RATE)& 0xF0) >> 4);
  return result;
};

//======================================================================
// Commissioning short addresses
//======================================================================

//Sets the slave Note 1 to the INITIALISE status for15 minutes.
//Commands 259 to 270 are enabled only for a slave in this
//status.

//set search address
void BDali::set_searchaddr(uint32_t adr) {
  SendCommand(DA_EXT_SEARCHADDRH,adr>>16);
  SendCommand(DA_EXT_SEARCHADDRM,adr>>8);
  SendCommand(DA_EXT_SEARCHADDRL,adr);
}

//set search address, but set only changed bytes (takes less time)
void BDali::set_searchaddr_diff(uint32_t adr_new,uint32_t adr_current) {
  if( (uint8_t)(adr_new>>16) !=  (uint8_t)(adr_current>>16) ) SendCommand(DA_EXT_SEARCHADDRH,adr_new>>16);
  if( (uint8_t)(adr_new>>8)  !=  (uint8_t)(adr_current>>8)  ) SendCommand(DA_EXT_SEARCHADDRM,adr_new>>8);
  if( (uint8_t)(adr_new)     !=  (uint8_t)(adr_current)     ) SendCommand(DA_EXT_SEARCHADDRL,adr_new);
}

//Is the random address smaller or equal to the search address?
//as more than one device can reply, the reply gets garbled
uint8_t BDali::compare() {
  uint8_t retry = 2;
  while(retry>0) {
    //compare is true if we received any activity on the bus as reply.
    //sometimes the reply is not registered... so only accept retry times 'no reply' as a real false compare
    int16_t rv = GetQuery(DA_EXT_COMPARE,0x00);
    if(rv == -STATUS_FRAME_ERROR) return 1;
    if(rv == -STATUS_VALID) return 1;
    if(rv == 0xFF) return 1;

    retry--;
  }
  return 0;
}

//The slave shall store the received 6-bit address (AAAAAA) as a short address if it is selected.
void BDali::program_short_address(uint8_t shortadr) {
  SendCommand(DA_EXT_PROGRAMM_SHORT_ADDRESS, (shortadr << 1) | 0x01);
}

//What is the short address of the slave being selected?
uint8_t BDali::query_short_address() {
  return GetQuery(DA_EXT_QUERY_SHORT_ADDRESS, 0x00) >> 1;
}

//find addr with binary search
uint32_t BDali::find_addr() {
  uint32_t adr = 0x800000;
  uint32_t addsub = 0x400000;
  uint32_t adr_last = adr;
  set_searchaddr(adr);
  
  while(addsub) {
    set_searchaddr_diff(adr,adr_last);
    adr_last = adr;
    //Serial.print("cmpadr=");
    //Serial.print(adr,HEX);
    uint8_t cmp = compare(); //returns 1 if searchadr > adr
    //Serial.print("cmp ");
    //Serial.print(adr,HEX);
    //Serial.print(" = ");
    //Serial.println(cmp);
    if(cmp) adr-=addsub; else adr+=addsub;
    addsub >>= 1;
  }
  set_searchaddr_diff(adr,adr_last);
  adr_last = adr;
  if(!compare()) {
    adr++;
    set_searchaddr_diff(adr,adr_last);
  }
  return adr;
}

//onlyNew=DA_YES : all without short address
//onlyNew=DA_NO : all 
//returns number of new short addresses assigned
uint8_t BDali::commission(uint8_t onlyNew) {
  uint8_t cnt = 0;
  uint8_t arr[64];
  uint8_t sa;
  for(sa=0; sa<64; sa++) arr[sa]=0;

//  find lights already on the bus and tags them as used in arr
  std::vector<uint8_t> shortAddresses = findLights();
      for (int i = 0; i < shortAddresses.size(); i++) {
        if (onlyNew == DA_YES){
        arr[shortAddresses[i]]=1;
        }
    }

  // TODO: delete existing shorts with their scenes and groups if onlyNew is DA_NO

  //start commissioning
  SendTwice(DA_EXT_INITIALISE,onlyNew);

  SendTwice(DA_EXT_RANDOMISE,0x00);
  //need 100ms pause after RANDOMISE, scan takes care of this...
  WaitForReady();
  //find random addresses and assign unused short addresses
  while(1) {
    uint32_t adr = find_addr();
    if(adr>0xffffff) break; //no more random addresses found -> exit
    //find first unused short address
    for(sa=0; sa<64; sa++) {
      if(arr[sa]==0) break;
    }
    if(sa>=64) break; //all 64 short addresses assigned -> exit

    //mark short address as used
    arr[sa] = 1;
    cnt++;
 
    //assign short address
    program_short_address(sa);

    //Serial.println(query_short_address()); //TODO check read adr, handle if not the same...

    //remove the device from the search
    SendCommand(DA_EXT_WITHDRAW,0x00);
  }

  //terminate the DALI_INITIALISE command
  SendCommand(DA_EXT_TERMINATE,0x00);
  return cnt;
}

