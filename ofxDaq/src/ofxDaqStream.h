#ifndef OFXDAQSTREAM
#define OFXDAQSTREAM

	#include "ofxDaqLog.h"
    #include "ofxDaqWriter.h"
    #include "ofxXmlSettings.h"
    #include "CircularFifo.h"
	#include "ofxNetwork.h"

    class ofxDaqStream {

        public:

			ofxDaqStream();
            virtual ~ofxDaqStream();
			virtual bool start(int elapsedTime) = 0;
            virtual bool stop() = 0;
            virtual bool writeData();
			virtual bool defineHeader();
            virtual bool nextFile(int elapsedTime);
            virtual bool update(int startTime, bool newFile);
			virtual bool sendDataBlock(ofxUDPManager *udpConnection);
			virtual bool dataReady();
			virtual bool checkStatus();
            virtual bool loadSettings(ofxXmlSettings settings) = 0;
            virtual float percentBufferFree();
            virtual float getDataRate() = 0;
			virtual void setFilePrefix(string prefix);
			virtual void setDataDirectory(string dataDir);

        protected:

            ofxDaqWriter * writer;
            CircularFifo * fifo;
			
			unsigned int blockSize;
			unsigned int headerSize;
            float dataRate;
            float startTime;
            unsigned int dataStartTime;
            unsigned int N;
            char * dataBlock;
			char * header;
			unsigned char type;
			string filePrefix;
			string dataDirectoryPath;
			string filePostfix;
			string fileExt;
			
			bool writeToFile;
			bool dataBlockFresh;
			bool deviceError;
			bool running;
			
			string name;
			int id;
			
			ofxDaqLog daqLog;

    };

#endif    
