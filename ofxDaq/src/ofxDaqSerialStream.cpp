#include "ofxDaqSerialStream.h"

//--------------------------------------------------------------
ofxDaqSerialStream::ofxDaqSerialStream(ofxXmlSettings settings){
    this->loadSettings(settings);    
    fifo = new CircularFifo(blockSize,N);
	dataBlock = (char*)malloc(blockSize);
	running = false;
	deviceError = false;
}

ofxDaqSerialStream::~ofxDaqSerialStream() {
	daqLog.logNotice(name,"Deleting data stream...");
	if (running) {
		this->stop();
    }
	delete fifo;
    writer->stop();
    delete writer;
    free(dataBlock);
}
	
//-------------------------------------------------------------
bool ofxDaqSerialStream::loadSettings(ofxXmlSettings settings){

    // Try to load settings from file and use
    // defaults if not found
    name = settings.getValue("name","SERIAL DEVICE");
	id = settings.getValue("id",1);
    baudRate = settings.getValue("settings:baudrate",9600);
	commPort = settings.getValue("settings:commport","/dev/ttyS0");
	N = settings.getValue("settings:blocks",512);
	blockSize = settings.getValue("settings:blocksize",256);
	filePrefix = settings.getValue("settings:fileprefix","TEST");
	filePostfix = settings.getValue("settings:filepostfix","SERIAL");
	fileExt = settings.getValue("settings:fileext","txt");
	
}

//-------------------------------------------------------------
void ofxDaqSerialStream::threadedFunction() {

    unsigned char * data;
    float tmp = 0;
    unsigned int byteCount = 0;
    int bytesRead = 0;
    int count = 0;

    // Preallocate data buffer    
    data = (unsigned char*)malloc(blockSize);

    // Setup the serial port
    serialPort.setup(commPort,baudRate);
	serialPort.setVerbose(true);
    
    // Conitune while the thread is running
    while(isThreadRunning()) {

        // Check for available data
		int bytesToRead = 0;
		int bytesAvailable = serialPort.available();
        if (bytesAvailable > 0){
			// Figure out how much we can read into the buffer
            //cout << "Bytes Available: " << bytesAvailable << endl;
			//cout << "ByteCount " << byteCount << endl;
			// Check we don't read more bytes than will fit in block.
            if (bytesAvailable > (blockSize - byteCount - 1)){
				//cout << "Limited bytesToRead" << endl;
				bytesToRead = blockSize - byteCount -1;
                
            }else{
                bytesToRead = bytesAvailable;
            }
			bytesRead = serialPort.readBytes(data+byteCount,bytesToRead);
            byteCount += bytesRead;
			//cout << "Bytes Read: " << bytesRead << endl;
			//cout << "Free Space: " << blockSize - byteCount - 1 << endl;
        }
        if (byteCount >= blockSize - 2){
			//cout << "writing buffer" << endl;
            fifo->writeBlock((char*)data,blockSize); 
			byteCount = 0;
        }

        ofSleepMillis(50);
        if (count < 10) {
            tmp += ((float)byteCount)/((float)ofGetElapsedTimef()-startTime)/1e6;
            count++;
        } else {
            dataRate = tmp/count;
            tmp = 0;
            count = 0;
        }
        startTime = (float)ofGetElapsedTimef();
    }

    //Close the port
    serialPort.close();

    // Free the Data buffer
    free(data);
}

//-------------------------------------------------------------
bool ofxDaqSerialStream::start(int elapsedTime){
	// Create the file writer
	startTime = ofGetElapsedTimef();
	writer = new ofxDaqWriter(dataDirectoryPath,filePrefix,filePostfix,fileExt);
    this->startThread(true,false);
	running = true;
    bool success = writer->start(elapsedTime);
	return success;
}

//-------------------------------------------------------------
bool ofxDaqSerialStream::stop(){
    this->stopThread(true);
	running = false;
	return writer->stop();
}

//-------------------------------------------------------------
float ofxDaqSerialStream::getDataRate() {
    return dataRate;
}

