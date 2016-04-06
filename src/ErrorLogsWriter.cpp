/*
 * ErrorLogsWriter.cpp
 *
 *  Created on: Apr 5, 2016
 *      Author: luciano
 */

#include "ErrorLogsWriter.h"

#define kErrorsLogName "xmlErrorsLog.txt"

ErrorLogsWriter::ErrorLogsWriter() {
	this->errorsLogFile.open(kErrorsLogName);
}

ErrorLogsWriter::~ErrorLogsWriter() {
	this->errorsLogFile.close();
}

void ErrorLogsWriter::writeErrorInFile(string error) {
	cout << error << endl;
	this->errorsLogFile << error << endl;
}
