#ifndef OFXDAQMANAGER
#define OFXDAQMANAGER

    #include "ofMain.h"
    #include "ofxDaqStream.h"

    class ofxDaqManager : public ofThread {

        public:

            ofxDaqManager();
            ofxDaqManager(unsigned int delay);
            ofxDaqManager(unsigned int delay, unsigned int fileTime);
			ofxDaqManager(ofxXmlSettings settings);
            ~ofxDaqManager();
			bool loadSettings(ofxXmlSettings settings);
			void setGlobalFilePrefix(string prefix);
			void setGlobalDataDirectory(string dataDir);
            bool addStream(ofxDaqStream * newStream);
            void threadedFunction();
            bool load();
            bool start(int elapsedTime);
            bool stop();
            float getTotalDataRate();
            float percentBuffersFree();
			string getDataDirectory();
        
        private:

            vector<ofxDaqStream*> streams;
            unsigned int delayTime;
            unsigned int nextFileTime;
            unsigned int startTime;
			string globalFilePrefix;
			string dataRoot;
			string dataDirectoryPath;
			string logFilePath;
			ofDirectory dataDir;
			ofxDaqLog daqLog;
			
            

    };

#endif

            
