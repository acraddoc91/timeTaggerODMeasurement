// timeTaggerODMeasurement.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TTMLib.h"
#include "TTMLib.hpp"
#include <fstream>
#include <string>
#include <vector>
#include "H5Cpp.h"
#include <iostream>

struct countData {
	uint8_t windowNum;
	uint32_t highWord;
	bool windowStatus;
	std::vector<std::vector<uint32_t>> windowedTags;
	std::vector<uint32_t> windowStartTags;
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

int processTags(TTMDataPacket_t *tagBuffer, countData *countData)
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
			(*countData).highWord = payload;
			//Write the highWord to the table if the window is open
			if ((*countData).windowStatus) {
				(*countData).windowedTags[(*countData).windowNum].push_back((payload << 1) | highLow);
			}
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
					//If slope is positive set the window open and record the low and high word of the start of the window
					if (slope == 1) {
						(*countData).windowStatus = true;
						(*countData).windowStartTags[(*countData).windowNum*2] = ((*countData).highWord << 1) | 1;
						(*countData).windowStartTags[(*countData).windowNum*2+1] = (payload << 1) | 0;

					}
					//If the slope is negative the window is closed so increment the window number and set the windowStatus to false
					else {
						(*countData).windowStatus = false;
						//Increment window number
						(*countData).windowNum++;
					}
				}
				//If we're using channel number 1 write the low words to the appropriate vector
				else if(channelNum == 0){
					if ((*countData).windowStatus){
						(*countData).windowedTags[(*countData).windowNum].push_back((payload << 1)| highLow);
					}
				}
			}
		}
	}
	//Delete the tag buffer to clear up memory space on the heap
	delete tagBuffer;
	return 0;
}

//Write the collected tags in countData to file
void tagsToHDF5(countData *cntData, std::string filename, std::string groupName, std::string datasetName, std::string startDataSetName) {
	//First let's create a file with the given filename
	H5::H5File file(&filename[0u], H5F_ACC_TRUNC);
	//Then create a group for our tags
	H5::Group group(file.createGroup(&groupName[0u]));
	//Dimensions of each vector set in the loop below
	hsize_t dims[1];
	//Loop to write all the windowed tags to file
	for (int i = 0; i < (*cntData).windowedTags.size(); i++) {
		//Determine the total dataset name from the groupname, datasetName and loop iteration
		std::string totDatasetName = groupName + '/' + datasetName + std::to_string(i);
		//Set the length of the dataset to the same length as the current tag vector
		dims[0] = (*cntData).windowedTags[i].size();
		//Create a dataspace to hold our data (cards on the table I'm not sure what a dataspae is but this seems necessary)
		H5::DataSpace dspace(1, dims);
		//Create dataset
		H5::DataSet dset(file.createDataSet(&totDatasetName[0u], H5::PredType::NATIVE_UINT32, dspace));
		//Write our data to the dataset
		dset.write(&(*cntData).windowedTags[i][0], H5::PredType::NATIVE_UINT32);
		(*cntData).windowedTags[i].clear();
	}
	//And write the start tags to file too
	std::string totDatasetName = groupName + '/' + startDataSetName;
	dims[0] = (*cntData).windowStartTags.size();
	H5::DataSpace dspace(1, dims);
	H5::DataSet dset(file.createDataSet(&totDatasetName[0u], H5::PredType::NATIVE_UINT32, dspace));
	dset.write(&(*cntData).windowStartTags[0], H5::PredType::NATIVE_UINT32);
	//Close all the HDF5 related crap to ensure memory gets freed
	dset.close();
	dspace.close();
	group.close();
	file.close();
}

int main(int argc, char* argv[])
{
	//Get time tagger IP from command line argument
	in_addr_t taggerIP = IPV4ToDecimal(argv[1]);
	//Get blackhole location from command line argument
	char* blackhole = argv[2];
	//Get the number of windows from the command line argument
	int numWindows = atoi(argv[3]);
	//All the classes we will need
	TTMCntrl_c *taggerControl = new TTMCntrl_c;
	TTMData_c *taggerDataConnection = new TTMData_c;
	TTMMeasConfig_t *taggerConfig = new TTMMeasConfig_t;
	bool dataAvailable = false;
	countData countData;
	countData.windowNum = 0;
	countData.highWord = 0;
	countData.windowStatus = false;
	bool collectData = true;
	countData.windowedTags.resize(numWindows);
	countData.windowStartTags.resize(numWindows*2);

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
	//Process data until escape file is updated
	while (collectData){
		//Loop while data is available
		while (dataAvailable) {
			TTMDataPacket_t *tagBuffer = new TTMDataPacket_t;
			taggerDataConnection->FetchData(tagBuffer, 100);
			processTags(tagBuffer, &countData);
			taggerDataConnection->DataAvailable(&dataAvailable, 250);
			//If we have acquired absorption, probe and background print the resulting counts to file
			if (countData.windowNum == numWindows) {
				tagsToHDF5(&countData, blackhole, "/Tags", "TagWindow", "StartTag");
				countData.windowNum = 0;
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
	delete taggerControl;

    return 0;
}

