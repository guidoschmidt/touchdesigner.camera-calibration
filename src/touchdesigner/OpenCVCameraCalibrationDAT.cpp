/* Shared Use License: This file is owned by Derivative Inc. (Derivative) and
* can only be used, and/or modified for use, in conjunction with
* Derivative's TouchDesigner software, and only if you are a licensee who has
* accepted Derivative's TouchDesigner license or assignment agreement (which
* also govern the use of this file).  You may share a modified version of this
* file with another authorized licensee of Derivative's TouchDesigner software.
* Otherwise, no redistribution or sharing of this file, with or without
* modification, is permitted.
*/

#include "OpenCVCameraCalibrationDAT.hpp"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <array>
#include <future>
#include <atomic>
#include <chrono>
#include <thread>

// These functions are basic C function, which the DLL loader can find
// much easier than finding a C++ Class.
// The DLLEXPORT prefix is needed so the compile exports these functions from the .dll
// you are creating
extern "C"
{
	DLLEXPORT void FillDATPluginInfo(DAT_PluginInfo* info)
	{
		// Always return DAT_CPLUSPLUS_API_VERSION in this function.
		info->apiVersion = DATCPlusPlusAPIVersion;
		// The opType is the unique name for this TOP. It must start with a
		// capital A-Z character, and all the following characters must lower case
		// or numbers (a-z, 0-9)
		info->customOPInfo.opType->setString("OpenCVCameraCalibrationDAT");
		// The opLabel is the text that will show up in the OP Create Dialog
		info->customOPInfo.opLabel->setString("OpenCV Camera Calibration DAT");
		// Will be turned into a 3 letter icon on the nodes
		info->customOPInfo.opIcon->setString("OCC");
		// Information about the author of this OP
		info->customOPInfo.authorName->setString("Guido Schmidt");
		info->customOPInfo.authorEmail->setString("git@guidoschmidt.cc");
		// This DAT works with 0 or 1 inputs
		info->customOPInfo.minInputs = 0;
		info->customOPInfo.maxInputs = 1;
	}

	DLLEXPORT DAT_CPlusPlusBase* CreateDATInstance(const OP_NodeInfo* info)
	{
		// Return a new instance of your class every time this is called.
		// It will be called once per DAT that is using the .dll
		return new CPlusPlusDATExample(info);
	}

	DLLEXPORT void DestroyDATInstance(DAT_CPlusPlusBase* instance)
	{
		// Delete the instance here, this will be called when
		// Touch is shutting down, when the DAT using that instance is deleted, or
		// if the DAT loads a different DLL
		delete (CPlusPlusDATExample*)instance;
	}

};

CPlusPlusDATExample::CPlusPlusDATExample(const OP_NodeInfo* info) : myNodeInfo(info)
{
	myExecuteCount = 0;
	myOffset = 0.0;
	myChop = "";
	myChopChanName = "";
	myChopChanVal = 0;
	_cameraCalibrator = new CameraCalibrator();
}

CPlusPlusDATExample::~CPlusPlusDATExample()
{
}

void CPlusPlusDATExample::getGeneralInfo(DAT_GeneralInfo* ginfo,
	const OP_Inputs* inputs,
	void* reserved1)
{
	// This will cause the node to cook every frame
	ginfo->cookEveryFrameIfAsked = false;
}

void CPlusPlusDATExample::execute(
	DAT_Output* output,
	const OP_Inputs* inputs,
	void* reserved)
{
	myExecuteCount++;
	if (inputs->getNumInputs() > 0)
	{
		if (_cameraCalibrator->isCalibrated()) {
			inputs->enablePar("Calibrate", false);
			// Status
			output->setTableSize(5, 5);
			output->setCellString(0, 0, "Status");
			output->setCellString(1, 0, "Calibrated");
			output->setCellString(2, 0, "");
			output->setCellString(3, 0, "");
			output->setCellString(4, 0, "");
			// Focal length
			output->setCellString(0, 1, "Focal Length");
			output->setCellDouble(1, 1, _cameraCalibrator->getFocalLength());
			// Principal point
			output->setCellString(0, 2, "Principal Point");
			output->setCellDouble(1, 2, _cameraCalibrator->getPrincipalPointInMillimeter().x);
			output->setCellDouble(2, 2, _cameraCalibrator->getPrincipalPointInMillimeter().y);
			// Distortion coefficients
			double ce0 = _cameraCalibrator->getDistortionCoefficientMatrix().at<double>(0, 0);
			double ce1 = _cameraCalibrator->getDistortionCoefficientMatrix().at<double>(1, 0);
			double ce2 = _cameraCalibrator->getDistortionCoefficientMatrix().at<double>(2, 0);
			double ce3 = _cameraCalibrator->getDistortionCoefficientMatrix().at<double>(3, 0);
			output->setCellString(0, 3, "Distortion Coefficients");
			output->setCellDouble(1, 3, ce0);
			output->setCellDouble(2, 3, ce1);
			output->setCellDouble(3, 3, ce2);
			output->setCellDouble(4, 3, ce3);
			return;
		}
		else {
			inputs->enablePar("Calibrate", true);
			const OP_DATInput* input = inputs->getInputDAT(0);
			int numRows = input->numRows;
			int numCols = input->numCols;
			bool isTable = input->isTable;
			const char* str = input->getCell(0, 0);
			_cameraCalibrator->clearImageFiles();
			for (unsigned int i = 0; i < numRows; ++i) {
				std::string imagePath = input->getCell(i, 1);
				_cameraCalibrator->addImageFile(imagePath);
			}
			std::vector<std::string> imageFiles = _cameraCalibrator->getImageFiles();
			output->setCellString(0, 0, "Input images");
			output->setOutputDataType(DAT_OutDataType::Table);
			output->setTableSize(imageFiles.size() + 1, 1);
			for (int i = 0; i < imageFiles.size(); i++) {
				output->setCellString(i + 1, 0, imageFiles[i].c_str());
			}
		}
	}
	else {
		inputs->enablePar("Calibrate", false);
		output->setTableSize(1, 1);
		output->setCellString(0, 0, "No input data");
	}
}

int32_t CPlusPlusDATExample::getNumInfoCHOPChans(void* reserved1)
{
	// We return the number of channel we want to output to any Info CHOP
	// connected to the CHOP. In this example we are just going to send one channel.
	return 0;
}

void CPlusPlusDATExample::getInfoCHOPChan(int32_t index,
	OP_InfoCHOPChan* chan,
	void* reserved1)
{
	// This function will be called once for each channel we said we'd want to return
	// In this example it'll only be called once.
	if (index == 0)
	{
		chan->name->setString("executeCount");
		chan->value = (float)myExecuteCount;
	}
}

bool CPlusPlusDATExample::getInfoDATSize(OP_InfoDATSize* infoSize, void* reserved1)
{
	infoSize->rows = 3;
	infoSize->cols = 3;
	// Setting this to false means we'll be assigning values to the table
	// one row at a time. True means we'll do it one column at a time.
	infoSize->byColumn = false;
	return true;
}

void CPlusPlusDATExample::getInfoDATEntries(int32_t index,
	int32_t nEntries,
	OP_InfoDATEntries* entries,
	void* reserved1)
{
	char tempBuffer[4096];
	if (index == 0)
	{
		// Set the value for the first column
#ifdef WIN32
		strcpy_s(tempBuffer, "executeCount");
#else // macOS
		strlcpy(tempBuffer, "executeCount", sizeof(tempBuffer));
#endif
		entries->values[0]->setString(tempBuffer);

		// Set the value for the second column
#ifdef WIN32
		sprintf_s(tempBuffer, "%d", myExecuteCount);
#else // macOS
		snprintf(tempBuffer, sizeof(tempBuffer), "%d", myExecuteCount);
#endif
		entries->values[1]->setString(tempBuffer);
	}

	if (index == 1)
	{
		// Set the value for the first column
#ifdef WIN32
		strcpy_s(tempBuffer, "offset");
#else // macOS
		strlcpy(tempBuffer, "offset", sizeof(tempBuffer));
#endif
		entries->values[0]->setString(tempBuffer);

		// Set the value for the second column
#ifdef WIN32
		sprintf_s(tempBuffer, "%g", myOffset);
#else // macOS
		snprintf(tempBuffer, sizeof(tempBuffer), "%g", myOffset);
#endif
		entries->values[1]->setString(tempBuffer);
	}

	if (index == 2)
	{
		// Set the value for the first column
#ifdef WIN32
		strcpy_s(tempBuffer, "DAT input name");
#else // macOS
		strlcpy(tempBuffer, "offset", sizeof(tempBuffer));
#endif
		entries->values[0]->setString(tempBuffer);

		// Set the value for the second column
#ifdef WIN32
		strcpy_s(tempBuffer, myDat.c_str());
#else // macOS
		snprintf(tempBuffer, sizeof(tempBuffer), "%g", myOffset);
#endif
		entries->values[1]->setString(tempBuffer);
	}
}

void CPlusPlusDATExample::setupParameters(OP_ParameterManager* manager, void* reserved1)
{
	// Execute
	{
		OP_NumericParameter	np;
		np.name = "Calibrate";
		np.label = "Calibrate";
		OP_ParAppendResult res = manager->appendPulse(np);
		assert(res == OP_ParAppendResult::Success);
	}
}

void CPlusPlusDATExample::performCalibrationAsync()
{
	_cameraCalibrator->evaluateImageStack();
	_cameraCalibrator->performCalibration();
}

void CPlusPlusDATExample::pulsePressed(const char* name, void* reserved1)
{
	if (!strcmp(name, "Calibrate"))
	{
		auto calculation_thread = std::async(&CPlusPlusDATExample::performCalibrationAsync, this);
	}
}

