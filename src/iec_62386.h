/* IEC62386-102:2009 command list
 *
 * Address pattern:
 * Address byte: YAAA AAAS
 * Y: 0 = short address, 1 = group address
 * AAA AAA = Address 0...63
 * S: 0 = DACP mode, 1 = Command mode

 * Timings
 * 1 Te  = 416,67 µs
 * 22 Te = 9,166 mS
 */


//Some basic defines 
#define DA_YES					0xFF
#define DA_NO					0x00
#define DA_MODE_DACP 			0x00	//Mode for direct output (0...100%)
#define DA_MODE_COMMAND 		0x01 	//Mode for DALI commands (MAX, MIN, OFF, etc.)
#define DA_GROUP_BIT 			0x80	//Address bit for group/boadcast
#define DA_SHORT_BIT 			0x00	//Address bit for short address

//Timing
#define DA_TE (1 / (2 * 1.2)) * 1000 			// ~416.67 us
#define DA_FORWARD_FRAME_TIME 			(38 * DA_TE)
#define DA_BACKWARD_FRAME_TIME 			(22 * DA_TE)
#define DA_SETTLING_TIME_FF_TO_FF 		(22 * DA_TE)
#define DA_SETTLING_TIME_BF_TO_FF 		(22 * DA_TE)
#define DA_SETTLING_TIME_FF_TO_BF_MIN 	(7 * DA_TE)
#define DA_SETTLING_TIME_FF_TO_BF_MAX 	(22 * DA_TE)

//Some mask values
#define DA_MAX_SHORT 			0x3F //0...63 -> 64 devices
#define DA_MAX_GROUP 			0x0F //0...15 -> 16 groups
#define DA_MAX_SCENE 			0x0F //0...15 -> 16 scenes

//Command 0..9 (YAAA AAAS xxxx xxxx)
#define DA_OFF								0x00
#define DA_UP								0x01
#define DA_DOWN								0x02
#define DA_STEP_UP							0x03
#define DA_STEP_DOWN						0x04
#define DA_MAX								0x05
#define DA_MIN								0x06
#define DA_STEP_DOWN_OFF					0x07
#define DA_ON_STEP_UP						0x08
#define DA_ENABLE_DACP_SEQUENCE				0x09
#define DA_RECALL_LAST_LEVEL                0x0A //only DALI2
//Commands 11..15 Reserved
//Command 16..31 (YAAA AAAS 0001 xxxx)
#define DA_GO_TO_SCENE						0x10 //Call scene
//Commands 32..129 (YAAA AAAS xxxx xxxx) (must be send twice within 100ms !)
#define DA_RESET							0x20
#define DA_DTR_ACTUAL_LEVEL					0x21
//Commands 34..35 Reserved
//Commands 36..39 Reserved
//Commands 40..41 Reserved
//Commands 42..47 (YAAA AAAS xxxx xxxx)
#define DA_DTR_MAX_LEVEL					0x2A
#define DA_DTR_MIN_LEVEL					0x2B
#define DA_DTR_SYS_FAIL_LEVEL				0x2C
#define DA_DTR_POWER_ON_LEVEL				0x2D
#define DA_DTR_FADE_TIME					0x2E
#define DA_DTR_FADE_RATE					0x2F
//Commands 48..63 Reserved
//Commands 64..79 (YAAA AAAS 0100 xxxx)
#define DA_DTR_AS_SCENE						0x40	//Add to scene
//Commands 80..95 (YAAA AAAS 0101 xxxx)
#define DA_REMOVE_FROM_SCENE				0x50	//Remove from scene
//Commands 96..111 (YAAA AAAS 0110 xxxx)
#define DA_ADD_TO_GROUP						0x60	//Add to group
//Commands 112..127 (YAAA AAAS 0111 xxxx)
#define DA_REMOVE_FROM_GROUP				0x70	//Remove from group

//Commands 128..128 (YAAA AAAS xxxx xxxx)
#define DA_DTR_AS_SHORT_ADDRESS				0x80
#define DA_ENABLE_WRITE_MEMORY				0x81
//Commands 130..131 Reserved
//Commands 132..135 Reserved
//Commands 136..143 Reserved
//Commands 144..223 (YAAA AAAS xxxx xxxx)
#define DA_QUERY_STATUS						0x90	//Return : 8 Bit value
/* Description DA_QUERY_STATUS:
BIT 0: Device condition				: 0 = OK
BIT 1: Lamp error					: 0 = OK
BIT 2: Lamp power on				: 0 = OFF
BIT 3: Query: Limit value error		: 0 = Last value between MIN and MAX or OFF
BIT 4: Fade in progress				: 0 = Finished; 1 = in progress
BIT 5: Query: RESET STATE?			: 0 = No
BIT 6: Query: short address missing : 0 = No
BIT 7: Query: POWER FAILURE?		: 0 = No; RESET or COMMAND received after power on
*/

#define DA_QUERY_CONTROL_GEAR					0x91	//Return : Yes or No
#define DA_QUERY_LAMP_FAILURE					0x92	//Return : Yes or No
#define DA_QUERY_LAMP_POWER_ON					0x93	//Return : Yes or No
#define DA_QUERY_LIMIT_ERROR					0x94	//Return : Yes or No
#define DA_QUERY_RESET_STATE					0x95	//Return : Yes or No
#define DA_QUERY_MISSING_SHORT_ADDRESS			0x96	//Return : Yes or No
#define DA_QUERY_VERSION_NUMBER					0x97	//Return : Must be 1
#define DA_QUERY_CONTENT_DTR					0x98	//Return : Content of DTR (8 Bit)
#define DA_QUERY_DEVICE_TYPE					0x99	//Return : Value between 0..255
#define DA_QUERY_PHYSICAL_MIN_LEVEL				0x9A	//Return : 8 Bit value
#define DA_QUERY_POWER_FAILURE					0x9B	//Return : Yes, if after power on no RESET, DAPC, OFF, RECALL MAX LEVEL, RECAL MIN LEVEL, STEP DOWN AND OFF, ON AND STEP UP, TO TO SCENE detected
#define DA_QUERY_CONTENT_DTR1					0x9C	//Return : Content of DTR1 (8 Bit)
#define DA_QUERY_CONTENT_DTR2					0x9D	//Return : Content of DTR2 (8 Bit)
//Commands 158..159 Reserved
#define DA_QUERY_ACTUAL_LEVEL					0xA0	//Return : Actual output (0...254)
#define DA_QUERY_MAX_LEVEL						0xA1	//Return : Stored MAX value
#define DA_QUERY_MIN_LEVEL						0xA2	//Return : Stored MIN value
#define DA_QUERY_POWER_ON_LEVEL					0xA3	//Return : Stored POWER ON value
#define DA_QUERY_SYSTEM_FAILURE_LEVEL			0xA4	//Return : Stored SYSTEM FAILURE value
#define DA_QUERY_FADE_TIME_RATE					0xA5	//Return : Stored FADE TIME(X) and  FADE RATE(Y) values (8 Bit: XXXX YYYYY)
//Commands 166..167 Reserved
//Commands 168..175 Reserved
#define DA_QUERY_SCENE_LEVEL					0xB0	//0xB0 - 0xBF Return : Stored value for scene X (8 Bit)
#define DA_QUERY_GROUPS_0_TO_7					0xC0	//Return : Bitmask GROUP 0..7 (8 Bit value; 1 = in group)
#define DA_QUERY_GROUPS_8_TO_15					0xC1	//Return : Bitmask GROUP 8..15 (8 Bit value; 1 = in group)
#define DA_QUERY_RANDOM_ADDRESS_H				0xC2	//Return : Upper 8 Bit of 24 Bit random address
#define DA_QUERY_RANDOM_ADDRESS_M				0xC3	//Return : Middle 8 Bit of 24 Bit random address
#define DA_QUERY_RANDOM_ADDRESS_L				0xC4	//Return : Lower 8 Bit of 24 Bit random address
#define DA_READ_MEMORY_LOCATION					0xC5
//Commands 198..199 Reserved
//Commands 200..207 Reserved
//Commands 208..223 Reserved
//Commands 224..254 - IEC62386-207:2009 (YAAA AAAS xxxx xxxx)
#define DA_REFERENCE_SYSTEM_POWER				0xE0
#define DA_ENABLE_CURRENT_PROTECTOR				0xE1
#define DA_DISABLE_CURRENT_PROTECTOR			0xE2
#define DA_SELECT_DIMMINT_CURVE					0xE3	//DTR=1 -> Linear, DTR=0 -> default (logarithmic)
#define DA_QUERY_GEAR_TYPE						0xED	//Return : 8 Bit value
/* Description DA_QUERY_GEAR_TYPE:
BIT 0: LED-Stromversorgung integriert	-> "0" = No
BIT 1: LED-Modul integriert				-> "0" = No
BIT 2: AC-Versorgung möglich			-> "0" = No
BIT 3: DC-Versorgung möglich			-> "0" = No
BIT 4: unused							-> "0" = default
BIT 5: unused							-> "0" = default
BIT 6: unused							-> "0" = default
BIT 7: unused							-> "0" = default
*/

#define DA_QUERY_DIMMING_CURVE					0xEE	//Return : 1 or 0
#define DA_QUERY_POSSIBLE_OPERATION_MODES		0xEF	//Return : 8 Bit value
/* Description DA_QUERY_POSSIBLE_OPERATION_MODES:
BIT 0: PWM operation possible			-> "0" = No
BIT 1: AM operation possible			-> "0" = No
BIT 2: Output is current controlled		-> "0" = No
BIT 3: High-current pulsed operation	-> "0" = No
BIT 4: unused							-> "0" = default
BIT 5: unused							-> "0" = default
BIT 6: unused							-> "0" = default
BIT 7: unused							-> "0" = default
*/

#define DA_QUERY_FEATURES						0xF0	//Return : 8 Bit value
/* Description DA_QUERY_FEATURES:
BIT 0: Short circuit detection can be queried									-> "0" = No
BIT 1: Idle detection can be queried										-> "0" = No
BIT 2: Detektion der Lastabnahme kann abgefragt werden								-> "0" = No
BIT 3: Detektion des Lastanstiegs kann abgefrag werden								-> "0" = No
BIT 4: Stromschutzeinrichtung vorhanden und kann abgefragt werden					-> "0" = No
BIT 5: Thermische Anschaltrung kann abgefragt werden								-> "0" = No
BIT 6: Lichtstromreduzierung aufgrund thermischer Überlast kann abgefragt werden	-> "0" = No
BIT 7: Physikalisch Auswahl unterstützt												-> "0" = No
*/

#define DA_QUERY_FAILURE_STATUS					0xF1	//Return : 8 Bit value
/* Description DA_QUERY_FAILURE_STATUS:
BIT 0: Short													-> "0" = No
BIT 1: Idle														-> "0" = No
BIT 2: Load decrease											-> "0" = No
BIT 3: Load increase											-> "0" = No
BIT 4: Current protection aktiv									-> "0" = No
BIT 5: Thermal shutdown											-> "0" = No
BIT 6: Thermal overload with reduction of the luminous flux		-> "0" = No
BIT 7: Reference measurement failed								-> "0" = No
*/

#define DA_QUERY_SHORT_CIRUIT						0xF2	//Return : Yes or No
#define DA_QUERY_OPEN_CIRCUIT						0xF3	//Return : Yes or No
#define DA_QUERY_LOAD_DECREASE						0xF4	//Return : Yes or No
#define DA_QUERY_LOAD_INCREASE						0xF5	//Return : Yes or No
#define DA_QUERY_CURRENT_PROTECTOR_ACTIVE			0xF6	//Return : Yes or No
#define DA_QUERY_THERMAL_SHUT_DOWN					0xF7	//Return : Yes or No
#define DA_QUERY_THERMAL_OVERLOAD					0xF8	//Return : Yes or No
#define DA_QUERY_REFERENCE_RUNNING					0xF9	//Return : Yes or No
#define DA_QUERY_REFERENCE_MEASUREMENT_FAILED		0xFA	//Return : Yes or No
#define DA_QUERY_CURRENT_PROTECTOR_ENABLED			0xFB	//Return : Yes or No
#define DA_QUERY_OPERATING_MODE						0xFC	//Return : 8 Bit value
/* Description DA_QUERY_OPERATING_MODE:
BIT 0: PWM operation activ							-> "0" = No
BIT 1: AM operation activ							-> "0" = No
BIT 2: Ouput is current regulated					-> "0" = No
BIT 3: High current pulse mode is active			-> "0" = No
BIT 4: Non-logarithmic dimming curve is active		-> "0" = No
BIT 5: unused										-> "0" = default
BIT 6: unused										-> "0" = default
BIT 7: unused										-> "0" = default
*/

#define DA_QUERY_FAST_FADE_TIME					0xFD		//Return : 8 Bit value
#define DA_QUERY_MIN_FADE_TIME					0xFE		//Return : 8 Bit value
#define DA_QUERY_EXTENDED_VERSION_NUMBER		0xFF		//Return : must be 1
//Commands 256..258 (YAAA AAAS xxxx xxxx)
#define DA_EXT_TERMINATE						0xA1
#define DA_EXT_DTR0                 			0xA3
#define DA_EXT_INITIALISE						0xA5
//Commands 259..349 (xxxx xxxx 0000 0000)
#define DA_EXT_RANDOMISE						0xA7
#define DA_EXT_COMPARE							0xA9
#define DA_EXT_WITHDRAW							0xAB
//Commands 262..263 Reserved
//Commands 264..266 (xxxx xxxx yyyy yyyy) -> yyyy yyyy: 8Bit value
#define DA_EXT_SEARCHADDRH						0xB1
#define DA_EXT_SEARCHADDRM						0xB3
#define DA_EXT_SEARCHADDRL						0xB5
//Commands 267..268 (xxxx xxxx 0AAA AAA1)
#define DA_EXT_PROGRAMM_SHORT_ADDRESS			0xB7
#define DA_EXT_VERIFY_SHORT_ADDRESS				0xB9
//Commands 268..270 (xxxx xxxx 0000 0000)
#define DA_EXT_QUERY_SHORT_ADDRESS				0xBB
#define DA_EXT_PHYSICAL_SELECTION				0xBD
//Commands 271 Reserved
//Commands 272 (xxxx xxxx yyyy yyyy) -> yyyy yyyy: 8Bit value for device type
#define DA_EXT_ENABLE_DEVICE_TYPE_X				0xC1 
//Commands 273..275 (xxxx xxxx yyyy yyyy)
#define DA_EXT_DATA_TRANSFER_REGISTER_1			0xC3
#define DA_EXT_DATA_TRANSFER_REGISTER_2			0xC5
#define DA_EXT_WRITE_MEMORY_LOCATION			0xC7
//Commands 276..279 Reserved
//Commands 280..287 Reserved
//Commands 296..299 Reserved
//Commands 300..301 Reserved
//Commands 302 Reserved
//Commands 303..318 Reserved
//Commands 319..334 Reserved
//Commands 335..342 Reserved
//Commands 343..346 Reserved
//Commands 347..348 Reserved
//Commands 349 Reserved



//Special address register for access memory storage

//Memory bank symbols
#define DA_MEMORYBANK_0							0x00
#define DA_MEMORYBANK_1							0x01
//#define DA_MEMORYBANK_N						0xNN

//Memory bank 0, register address
#define DA_MEMORY_0_LAST_PLACE					0x00	//Address for the last accessable memory location
#define DA_MEMORY_0_CHECKSUM					0x01	//checksum memory bank 0
#define DA_MEMORY_0_NUM_LAST_MEMORY				0x02	//Number of last memory bank access
#define DA_MEMORY_0_OEM_GTIN_0					0x03	//OEM GTIN Byte 0 (MSB)
#define DA_MEMORY_0_OEM_GTIN_1					0x04	//OEM GTIN Byte 1
#define DA_MEMORY_0_OEM_GTIN_2					0x05	//OEM GTIN Byte 2
#define DA_MEMORY_0_OEM_GTIN_3					0x06	//OEM GTIN Byte 3
#define DA_MEMORY_0_OEM_GTIN_4					0x07	//OEM GTIN Byte 4
#define DA_MEMORY_0_OEM_GTIN_5					0x08	//OEM GTIN Byte 5
#define DA_MEMORY_0_REVISION_MSB				0x09	//Firmware version (major)
#define DA_MEMORY_0_REVISION_LSB				0x0A	//Firmware version (minor)
#define DA_MEMORY_0_OEM_SERIAL_1				0x0B	//OEM serial number Byte 1 (MSB)
#define DA_MEMORY_0_OEM_SERIAL_2				0x0C	//OEM serial number Byte 2
#define DA_MEMORY_0_OEM_SERIAL_3				0x0D	//OEM serial number Byte 3
#define DA_MEMORY_0_OEM_SERIAL_4				0x0E	//OEM serial number Byte 4
#define DA_MEMORY_0_OEM_EXTRA					0x0F	//additional OEM information

//Memory bank 1, register address
#define DA_MEMORY_1_LAST_PLACE					0x00	//Address for the last accessable memory location
#define DA_MEMORY_1_CHECKSUM					0x01	//Checksum memory bank 1
#define DA_MEMORY_1_LOCK						0x02	//Memory bank 1 lock byte (locked: 0xFF, unlocked: 0x55)
#define DA_MEMORY_1_OEM_GTIN_0					0x03	//OEM GTIN Byte 0 (MSB)
#define DA_MEMORY_1_OEM_GTIN_1					0x04	//OEM GTIN Byte 1
#define DA_MEMORY_1_OEM_GTIN_2					0x05	//OEM GTIN Byte 2
#define DA_MEMORY_1_OEM_GTIN_3					0x06	//OEM GTIN Byte 3
#define DA_MEMORY_1_OEM_GTIN_4					0x07	//OEM GTIN Byte 4
#define DA_MEMORY_1_OEM_GTIN_5					0x08	//OEM GTIN Byte 5
#define DA_MEMORY_1_OEM_SERIAL_1				0x09	//OEM serial number Byte 1 (MSB)
#define DA_MEMORY_1_OEM_SERIAL_2				0x0A	//OEM serial number Byte 2
#define DA_MEMORY_1_OEM_SERIAL_3				0x0B	//OEM serial number Byte 3
#define DA_MEMORY_1_OEM_SERIAL_4				0x0C	//OEM serial number Byte 4 (LSB)
#define DA_MEMORY_1_SUBSYSTEM_DEVNUM			0x0D	//Subsystem (Bit 4 ... Bit 7), Device number (Bit 0 ... Bit 3)
#define DA_MEMORY_1_LIGHT_NUM_LOCK				0x0E	//Lamp number
#define DA_MEMORY_1_LIGHT_NUM					0x0F	//Lamp type (not able to lock)
#define DA_MEMORY_1_OEM_EXTRA					0x10	//additional OEM information

//Special memory bank values
#define DA_MEMORY_LOCK							0xFF	//Value for memory bank 1 to lock
#define DA_MEMORY_UNLOCK						0x55	//Value for memory bank 1 to unlock




