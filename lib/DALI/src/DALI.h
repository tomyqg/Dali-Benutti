#ifndef __DALI_H__
#define __DALI_H__

#include <Arduino.h>

class DALI {
	private:
		enum {
			COMMAND_BROADCAST	= 0b11111111,
			COMMAND_GRUPO		= 0b10000001,
			RESET				= 0b00100000,
			INITIALISE			= 0b10100101,
			RANDOMISE			= 0b10100111,
			SEARCHADDRH			= 0b10110001,
			SEARCHADDRM			= 0b10110011,
			SEARCHADDRL			= 0b10110101,
			COMPARE				= 0b10101001,
			WITHDRAW			= 0b10101011,
			TERMINATE			= 0b10100001,
			SHORT_POWER			= 0b00000000,
			PROGRAM_SHORT_ADDRESS	= 0b10110111,
			VERIFY_SHORT_ADDRESS	= 0b10111001,

			CONTROL_BROADCAST	= 0b11111110,
		};

	public:
#if defined(DALI_TX) && defined(DALI_RX)
		explicit DALI(int txPin = DALI_TX, int rxPin = DALI_RX);
#else
		explicit DALI(int txPin, int rxPin);
#endif

	public:
		void begin();
		int initNodes(const uint8_t* addresses, uint8_t numAddresses);
		int initNodes(uint8_t address);

		void turnOff(uint8_t nodeAddress);
		void setMax(uint8_t nodeAddress);
		void setValue(uint8_t nodeAddress, uint8_t value);

	private:
		void sendZero(void);
		void sendOne(void);
		void sendCommand(uint8_t command, uint8_t data);
		bool sendSearchAddr(uint32_t addr);
		bool sendProgramShortAddr(uint8_t nodeNumber);
		void withdrawNode(uint32_t addr);

	private:
		int txPin;
		int rxPin;
};

#endif // __DALI_H__
