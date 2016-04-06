/*
 * ErrorLogsWriter.h
 *
 *  Created on: Apr 5, 2016
 *      Author: luciano
 */

#ifndef ERRORLOGSWRITER_H_
#define ERRORLOGSWRITER_H_

#include "Constants.h"

class ErrorLogsWriter {
public:
	ErrorLogsWriter();
	virtual ~ErrorLogsWriter();

private:
	ofstream errorsLogFile;
	void writeErrorInFile(string error);
};

#endif /* ERRORLOGSWRITER_H_ */
