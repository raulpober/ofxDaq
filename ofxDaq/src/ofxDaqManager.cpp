#include "ofxDaqManager.h"

//--------------------------------------------------------------
ofxDaqManager::ofxDaqManager(){
    delayTime = 100;
    nextFileTime = 30000;
	enableNetworkTransfer = false;
}

//--------------------------------------------------------------
ofxDaqManager::ofxDaqManager(unsigned int delay){
    delayTime = delay;
    nextFileTime = 30000;
	enableNetworkTransfer = false;
}

//--------------------------------------------------------------
ofxDaqManager::ofxDaqManager(unsigned int delay, unsigned int fileTime){
    delayTime = delay;
    nextFileTime = fileTime;
	enableNetworkTransfer = false;
}

//--------------------------------------------------------------
ofxDaqManager::ofxDaqManager(ofxXmlSettings settings){
    
	enableNetworkTransfer = false;
	this->loadSettings(settings);
}

//-------------------------------------------------------------
bool ofxDaqManager::loadSettings(ofxXmlSettings settings){

    // Try to load settings from file and use
    // defaults if not found
    delayTime = settings.getValue("settings:delaytime",20);
	nextFileTime = settings.getValue("settings:nextfiletime",30000);
	dataRoot = settings.getValue("settings:dataroot","/data");
	globalFilePrefix = settings.getValue("settings:globalfileprefix","EXP");
	ipAdd = settings.getValue("settings:ipaddress","-1");
	netPort = settings.getValue("settings:netport",0);

	if (netPort != 0){
		enableNetworkTransfer = true;
	}
	
	return true;
}

//--------------------------------------------------------------
string ofxDaqManager::getDataDirectory(){
	return this->dataDirectoryPath;
}

//--------------------------------------------------------------
bool ofxDaqManager::start(int elapsedTime){

	string timestamp;

	// Setup UDP if requested
	if (enableNetworkTransfer){
		udpConnection.Create();
		udpConnection.Connect(ipAdd.c_str(),netPort);
		udpConnection.SetNonBlocking(true);
	}

	// Setup the data directory and logging
	
	// Grab the directory and log file name timestamp
	timestamp = ofGetTimestampString();
	
	// Setup the data directories
	// Create a file name for the new directory and append to the data root
	dataDirectoryPath = ofFilePath::getPathForDirectory(dataRoot + 
		"/" + globalFilePrefix + "-" + timestamp);
	dataDir.createDirectory(dataDirectoryPath,true);
	logFilePath = ofFilePath::getAbsolutePath(dataDirectoryPath + "/" + globalFilePrefix + "-" + timestamp + ".log");
	// Turn on logging to file
	ofLogToFile(logFilePath,false);
	
	// Set the data directory for all the streams
	setGlobalDataDirectory(dataDirectoryPath);
	setGlobalFilePrefix(globalFilePrefix);
	
	// Get the start time and start the streams
	startTime = ofGetElapsedTimeMillis(); 
    // Start the streams, then the monitor thread
    for(unsigned int i=0; i<streams.size(); i++){
		streams[i]->start(elapsedTime);
    }
	// This tread monitors all the streams
    this->startThread(true,false);
	
    return true;
}

//-------------------------------------------------------------
bool ofxDaqManager::stop(){
    this->stopThread(true);
	daqLog.logNotice("DAQ MANAGER","STOPPING DATA STREAMS");
	
	// Setup UDP if requested
	if (enableNetworkTransfer){
		udpConnection.Close();
	}
	
    // Iterate over the streams and 
    for(unsigned int i=0; i<streams.size(); i++){
        streams[i]->stop();
    }
    return true;
}

//-------------------------------------------------------------
ofxDaqManager::~ofxDaqManager() {
	daqLog.logNotice("DAQ MANAGER","DELETING DAQ STREAMS");

    streams.clear();
}

//-------------------------------------------------------------
void ofxDaqManager::setGlobalFilePrefix(string prefix) {
    globalFilePrefix = prefix;
	for(unsigned int i=0; i<streams.size(); i++){
        streams[i]->setFilePrefix(prefix);
    }
}

//-------------------------------------------------------------
void ofxDaqManager::setGlobalDataDirectory(string dataDir){
	dataDirectoryPath = dataDir;
	for(unsigned int i=0; i<streams.size(); i++){
        streams[i]->setDataDirectory(dataDir);
    }
}

//-------------------------------------------------------------
void ofxDaqManager::threadedFunction() {
    
    // Conitune while the thread is running
    while(isThreadRunning()) {
        
        // Save the time to apply to all streams when updating
        // file names.
        int time = ofGetElapsedTimeMillis();

        // Check ellapsed time
        unsigned int elapsedTime = time - startTime;
        bool newFile = elapsedTime >= nextFileTime;
    
        // Iterate over the streams and update (write, etc)
        for(unsigned int i=0; i<streams.size(); i++){
            
            if (newFile){
                // Update the start time
                startTime = time;
            }
            
            streams[i]->update(time,newFile); 
			
			// If network is enabled then tx
			if (enableNetworkTransfer){
				streams[i]->sendDataBlock(&udpConnection);
			}       

        }   

        // Wait
        ofSleepMillis(delayTime);     
        
    }

    // Dealloc anything here
}

//-------------------------------------------------------------
bool ofxDaqManager::addStream(ofxDaqStream * newStream){
    streams.push_back(newStream);
    return true;
}

//-------------------------------------------------------------
float ofxDaqManager::getTotalDataRate() {
    float rate = 0.0;
    for(unsigned int i=0; i<streams.size(); i++){
        rate += streams[i]->getDataRate();
    }
    return rate;
}

//-------------------------------------------------------------
float ofxDaqManager::percentBuffersFree(){
    float free = 0.0;
    for(unsigned int i=0; i<streams.size(); i++){
        free += streams[i]->percentBufferFree();
    }
    return (free/streams.size());
}
