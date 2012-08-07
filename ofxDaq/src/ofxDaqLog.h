#ifndef OFXDAQLOG
#define OFXDAQLOG

	#include "ofMain.h"

	// Enumerator of error types
	enum DaqErr {
		DAQERR_NO_ERROR = 0,
		DAQERR_DEVICE_ERROR = 1,
		DAQERR_DEVICE_NOT_INTIALIZED = 2,
		DAQERR_STREAM_BUFFER_FULL = 3,
		DAQERR_FILE_WRITE_ERROR = 4,
		DAQERR_FILE_CREATE_ERROR = 5,
		DAQERR_FILE_CLOSE_ERROR = 6,
		DAQERR_USB_ERROR = 7
	};

	class ofxDaqLog {
	
		public:
			
			ofxDaqLog();
			void logError(DaqErr type, int id, string name, int code);
			void logError(DaqErr type, int id, string name, string msg);
			void logWarning(string name, string msg);
			void logNotice(string name, string msg);
			
		protected:
		
			DaqErr errorType;
			int errorCode;
			int deviceID;
			string deviceName;
			
			
	};

#endif