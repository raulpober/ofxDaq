#ifndef OFXDAQWRITER
#define OFXDAQWRITER

    #define TIMESTAMP_LENGTH 24
    #define FILECOUNT_LENGTH 8 

    #include "ofMain.h"
	#include "ofxDaqLog.h"

    class ofxDaqWriter {

        public:

            ofxDaqWriter();
            ofxDaqWriter(string dir, string pfx, string pstfx, string ext);
            ~ofxDaqWriter();
            bool start(int elapsedTime);
            bool stop();
            bool writeData(char * data, unsigned int size);
            bool nextFile(int elapsedTime);
			unsigned int getBytesWritten();
			bool createHeader(char * headerData,unsigned int headerSize);

        private:

            bool buildFileName(int elapsedTime);

            ofstream file;
            unsigned int fileCount;
            unsigned int bytesWritten;
            float dataRate; // In MB/s
			string fileDir;
            string filePrefix;
            string filePostfix;
            string fileExt;
            string filename;
			char * headerData;
			unsigned int headerSize;


    }; 

#endif    
