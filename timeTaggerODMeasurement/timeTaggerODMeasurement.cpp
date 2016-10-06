// timeTaggerODMeasurement.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TTMLib.h"
#include "TTMLib.hpp"
#include <fstream>
#include <string>

struct countData {
	uint8_t window;
	uint16_t absorption;
	uint16_t probe;
	uint16_t background;
};

//Convert IPV4 in human readable form to decimal form
int IPV4ToDecimal(char* IPV4) 
{
	//First let's split the string using the . delimeter
	char *next_token;
	char *token;
	token = strtok_s(IPV4,".", &next_token);
	int IPV4Nums[4];
	for (int i = 0; i < 4; i++ ) {
		IPV4Nums[i] = atoi(token);
		token = strtok_s(NULL, ".", &next_token);
	}
	//And do some bitwise operations to get the IP in decimal form
	int decimalOut = IPV4Nums[3];
	decimalOut = decimalOut | (IPV4Nums[2] << 8);
	decimalOut = decimalOut | (IPV4Nums[1] << 16);
	decimalOut = decimalOut | (IPV4Nums[0] << 24);
	return decimalOut;
}

//Seperate function for setting config to clean things up
TTMMeasConfig_t* configSetter() 
{
	//Standard things, probably don't want to change these
	TTMMeasConfig_t *configOut = new TTMMeasConfig_t;
	configOut->GPXRefClkDiv = 0;
	configOut->GPXHSDiv = 0;
	configOut->GPXStartTimer = 0;
	configOut->MModeDiv = 0;
	configOut->MModeTimeout = 0;
	configOut->MagicGPXOffset = GPXUnderflowAvoidanceOffset;
	configOut->NetMTU = 0;
	configOut->UserData = 0;
	configOut->DataTargetPort = FlexIODataPort;
	configOut->DataTargetIPAddr = INADDR_ANY;
	configOut->DataIdleTimeout = 125 * 100000;
	configOut->UseLittleEndianByteOrder = true;
	configOut->UseDifferentialInputs = false;
	for (int i = 0; i < 9; i++) {
		//By default turn off edges
		configOut->EnableEdge[i][1] = false;
		configOut->EnableEdge[i][0] = false;
		//LVTTL threshold
		configOut->SignalLevel[i] = 1400;
	}
	//Things that can be changed based on operating mode
	//Set data mode
	configOut->DataFormat = TTFormat_IMode_EXT64_PACK;
	//Set start and first channel rising edge on
	configOut->EnableEdge[0][0] = true;
	configOut->EnableEdge[1][0] = true;
	configOut->EnableEdge[3][0] = true;
	configOut->EnableEdge[3][1] = true;
	//Use the internal pulse as start trigger
	configOut->UsePulseGenStart = true;
	configOut->UsePulseGenStop1 = false;
	configOut->PermitAutoPulseGenStart = true;
	return configOut;
}

int processTags(TTMDataPacket_t *tagBuffer, uint32_t *highWord, bool *windowStatus, countData *countData)
{
	//Determine the number of tags to process
	int numElements = sizeof(tagBuffer->Data.TimetagI64Pack)/sizeof(tagBuffer->Data.TimetagI64Pack[0]);
	//Loop over tags
	for (int i = 0; i < numElements; i++) {
		//Extract the payload and highLow data
		uint32_t payload = tagBuffer->Data.TimetagI64Pack[i].Payload;
		uint32_t highLow = tagBuffer->Data.TimetagI64Pack[i].HighLow;
		//Determine if this is a high word and if so set the new high word
		if (highLow == 1) {
			*highWord = payload;
		}
		//If word is low word
		else if(highLow==0) {
			//See if payload is null
			if (payload != 0) {
				//Check channel number
				uint8_t channelNum = ((payload >> 28) & 7);
				//If channel number is 3 then check whether we're using the rising or falling edge to dictate whether the window is open or closed
				if (channelNum == 2) {
					uint8_t slope = ((payload >> 27) & 1);
					//If slope is positive set the window open
					if (slope == 1) {
						*windowStatus = true;
					}
					else {
						*windowStatus = false;
						//Increment window number
						switch ((*countData).window) {
						case 0:
							(*countData).window = 1;
							break;
						case 1:
							(*countData).window = 2;
							break;
						case 2:
							(*countData).window = 3;
							break;
						}
					}
				}
				else if(channelNum == 0){
					if (*windowStatus){
						switch ((*countData).window) {
						case 0:
							(*countData).absorption++;
							break;
						case 1:
							(*countData).probe++;
							break;
						case 2:
							(*countData).background++;
							break;
						}
					}
				}
			}
		}
	}
	//Delete the tag buffer to clear up memory space on the heap
	delete tagBuffer;
	return 0;
}

int main(int argc, char* argv[])
{
	//Get time tagger IP from command line argument
	in_addr_t taggerIP = IPV4ToDecimal(argv[1]);
	//Get blackhole location from command line argument
	char* blackhole = argv[2];
	//All the classes we will need
	TTMCntrl_c *taggerControl = new TTMCntrl_c;
	TTMData_c *taggerDataConnection = new TTMData_c;
	TTMEventCnt_c *singleEventCounter = new TTMEventCnt_c;
	TTMMeasConfig_t *taggerConfig = new TTMMeasConfig_t;
	bool dataAvailable = false;
	uint32_t highWord = 0;
	bool windowStatus = false;
	struct countData countData;
	countData.absorption = 0;
	countData.probe = 0;
	countData.background = 0;
	countData.window = 0;
	bool collectData = true;

	//Connect and configure the tagger
	taggerControl->Connect(NULL, TTM8ApplCookie, taggerIP, FlexIOCntrlPort, INADDR_ANY, 0, 1000);
	//Buffer size is 8MB
	taggerDataConnection->Connect(taggerIP, FlexIODataPort, INADDR_ANY, 0, 8 * 1024 * 1024, INVALID_SOCKET);
	taggerConfig = configSetter();
	taggerControl->ConfigMeasurement(taggerConfig);
	//Start measurement
	taggerControl->StartMeasurement(true);
	Sleep(100);
	//Check if data is available
	taggerDataConnection->DataAvailable(&dataAvailable, 1000);
	//Process data until escape key is pressed
	while (collectData){
		//Loop while data is available
		while (dataAvailable) {
			TTMDataPacket_t *tagBuffer = new TTMDataPacket_t;
			taggerDataConnection->FetchData(tagBuffer, 100);
			processTags(tagBuffer, &highWord, &windowStatus, &countData);
			taggerDataConnection->DataAvailable(&dataAvailable, 250);
			//If we have acquired absorption, probe and background print the resulting counts to file
			if (countData.window == 3) {
				std::ofstream dataFile;
				dataFile.open(blackhole);
				dataFile << "Absorption\tProbe\tBackground\n";
				dataFile << countData.absorption << "\t" << countData.probe << "\t" << countData.background;
				dataFile.close();
				//Reset window and counts
				countData.window = 0;
				countData.absorption = 0;
				countData.probe = 0;
				countData.background = 0;
			}
			//Check to see if the stopFile has been written to
			std::ifstream stopFile;
			stopFile.open("stopFile.txt");
			std::string stopLine;
			stopFile >> stopLine;
			if (stopLine != "0") {
				collectData = false;
				break;
			}
		}
		//If data isn't available we get kicked out of the above loop, we'll just keep checking if there is new data until the stopfile is updated
		taggerDataConnection->DataAvailable(&dataAvailable, 250);
		//If no data is available check the stop file and take a nap
		if (!dataAvailable) {
			//Check to see if the stopFile has been written to
			std::ifstream stopFile;
			stopFile.open("stopFile.txt");
			std::string stopLine;
			stopFile >> stopLine;
			if (stopLine != "0") {
				collectData = false;
				break;
			}
			Sleep(250);
		}
	}
	//Stop measurement
	taggerControl->StopMeasurement();
	//Disconnect
	taggerControl->Disconnect();
	taggerDataConnection->Disconnect();
	delete taggerConfig;
	delete taggerDataConnection;
	delete singleEventCounter;
	delete taggerConfig;

    return 0;
}

