#include "ofxDaqLog.h"

//---------------------------------------------------------------
ofxDaqLog::ofxDaqLog(){
	errorType = DAQERR_NO_ERROR;
	errorCode = 0;

}

//---------------------------------------------------------------
void ofxDaqLog::logError(DaqErr type, int id, string name, int code){

	string report;
	
	report = ofGetTimestampString() + " : DEVICE = " + name + ", TYPE = ";
	
	switch (type) {
	
		case DAQERR_NO_ERROR:
			report += "NO_ERROR";
			break;
		case DAQERR_DEVICE_ERROR:
			report += "DEVICE_ERROR";
			break;
		case DAQERR_DEVICE_NOT_INTIALIZED:
			report += "DEVICE_NOT_INITIALIZED";
			break;
		case DAQERR_STREAM_BUFFER_FULL:
			report += "STREAM_BUFFER_FULL";
			break;
		case DAQERR_FILE_WRITE_ERROR:
			report += "FILE_WRITE_ERROR";
			break;
		case DAQERR_FILE_CREATE_ERROR:
			report += "FILE_CREATE_ERROR";
			break;
		case DAQERR_FILE_CLOSE_ERROR:
			report += "FILE_CLOSE_ERROR";
			break;
		case DAQERR_USB_ERROR:
			report += "USB_ERROR";
			break;

	}
	
	report += ", DEVICE ID = " + ofToString(id);
	report += ", ERROR CODE = " + ofToString(code);
	
	// Write to the log
	ofLogError() << report;

}

//---------------------------------------------------------------
void ofxDaqLog::logError(DaqErr type, int id, string name, string msg){

	string report;
	
	report = ofGetTimestampString() + " : DEVICE = " + name + ", TYPE = ";
	
	switch (type) {
	
		case DAQERR_NO_ERROR:
			report += "NO_ERROR";
			break;
		case DAQERR_DEVICE_ERROR:
			report += "DEVICE_ERROR";
			break;
		case DAQERR_DEVICE_NOT_INTIALIZED:
			report += "DEVICE_NOT_INITIALIZED";
			break;
		case DAQERR_STREAM_BUFFER_FULL:
			report += "STREAM_BUFFER_FULL";
			break;
		case DAQERR_FILE_WRITE_ERROR:
			report += "FILE_WRITE_ERROR";
			break;
		case DAQERR_FILE_CREATE_ERROR:
			report += "FILE_CREATE_ERROR";
			break;
		case DAQERR_FILE_CLOSE_ERROR:
			report += "FILE_CLOSE_ERROR";
			break;
		case DAQERR_USB_ERROR:
			report += "USB_ERROR";
			break;

	}
	
	report += ", DEVICE ID = " + ofToString(id);
	report += ", ERROR MSG = " + msg;
	
	// Write to the log
	ofLogError() << report;

}

//---------------------------------------------------------------
void ofxDaqLog::logWarning(string name, string msg){

	string report = ofGetTimestampString() + " : DEVICE = " + name + ", MSG: " + msg;
	
	// Write to the log
	ofLogWarning() << report;
	
}

//---------------------------------------------------------------
void ofxDaqLog::logNotice(string name, string msg){

	string report = ofGetTimestampString() + " : DEVICE = " + name + ", MSG: " + msg;
	
	// Write to the log
	ofLogNotice() << report;
	
}