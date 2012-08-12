#ifndef OFXDAQSERIALSTREAM
#define OFXDAQSERIALSTREAM

    #include "ofxDaqStream.h"

    class ofxDaqSerialStream : public ofxDaqStream, public ofThread {

        public:

			ofxDaqSerialStream(ofxXmlSettings settings);
            ~ofxDaqSerialStream();
            bool start(int elapsedTime);
            bool stop();
            bool loadSettings(ofxXmlSettings XML);
            float getDataRate();
            void threadedFunction();    

        protected:

            // The serial comm
            ofSerial serialPort;
            unsigned int baudRate;
            string commPort;

    };

#endif
