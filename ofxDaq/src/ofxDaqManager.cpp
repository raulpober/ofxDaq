#include "ofxDaqManager.h"

//--------------------------------------------------------------
ofxDaqManager::ofxDaqManager(){
    delayTime = 100;
    nextFileTime = 30000;
	enableNetworkTransfer = false;
	clientConnected = false;
}

//--------------------------------------------------------------
ofxDaqManager::ofxDaqManager(unsigned int delay){
    delayTime = delay;
    nextFileTime = 30000;
	enableNetworkTransfer = false;
	clientConnected = false;
}

//--------------------------------------------------------------
ofxDaqManager::ofxDaqManager(unsigned int delay, unsigned int fileTime){
    delayTime = delay;
    nextFileTime = fileTime;
	enableNetworkTransfer = false;
	clientConnected = false;
}

//--------------------------------------------------------------
ofxDaqManager::ofxDaqManager(ofxXmlSettings settings){
    
	enableNetworkTransfer = false;
	clientConnected = false;
	this->loadSettings(settings);
}

//-------------------------------------------------------------
ofxDaqManager::~ofxDaqManager() {
	daqLog.logNotice("DAQ MANAGER","DELETING DATA STREAMS");
	if (enableNetworkTransfer){
		udpConnection.Close();
	}
    streams.clear();
	daqLog.logNotice("DAQ MANAGER","DELETING DATA STREAMS CLEARED");
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
    
    float timer = ofGetElapsedTimef();
	float clientTimer = timer;
	char udpMessage[100];
	
	
	// Conitune while the thread is running
    while(isThreadRunning()) {
        
        // Save the time to apply to all streams when updating
        // file names.
        int time = ofGetElapsedTimeMillis();
		int recentReceived = 1;
		clientConnected = false;

        // Check ellapsed time
        unsigned int elapsedTime = time - startTime;
        bool newFile = elapsedTime >= nextFileTime;
    
        // Iterate over the streams and update (write, etc)
        bool restart = false;
		for(unsigned int i=0; i<streams.size(); i++){
            
            if (newFile){
                // Update the start time
                startTime = time;
            }
            
            streams[i]->update(time,newFile); 
			
			// If network is enabled then tx
			if (enableNetworkTransfer){
				// Only send data blocks when a client is connected
				if (clientConnected){
					streams[i]->sendDataBlock(&udpConnection);
					string msg = " This is a status msg: " + ofGetTimestampString(); 
					msg[0] = 4;
					udpConnection.Send(msg.c_str(),msg.length());
				}
				
				/*int received = udpConnection.Receive(udpMessage,100);
				
				if (received > 0){
					recentReceived = received;
					if(!clientConnected){
						clientConnected = true;
						daqLog.logNotice("DAQ MANAGER","CLIENT CONNECTED");
					}
				}
				if (((recentReceived <= 0) || (ofGetElapsedTimef() - clientTimer > 10.0)) && clientConnected){
					if (recentReceived <= 0){
						clientConnected = false;
						daqLog.logNotice("DAQ MANAGER","CLIENT DISCONNECTED");
					}
					clientTimer = ofGetElapsedTimef();
					recentReceived = 0;
				}*/
				
			}      
			
			// Check status every 10 seconds
			if (ofGetElapsedTimef() - timer > 10){
				if (!streams[i]->checkStatus()){
					restart = true;
				}
				timer = ofGetElapsedTimef();
			}
			

        }   
		
		if (restart){
			// restart the streams and log it
			daqLog.logNotice("DAQ MANAGER", "RESTARTING STREAMS DUE TO FAILED STATUS CHECK");
			// Iterate over the streams and 
			for(unsigned int i=0; i<streams.size(); i++){
				streams[i]->stop();
			}
			ofSleepMillis(10);
			int startTime = ofGetElapsedTimeMillis(); 
			// Start the streams, then the monitor thread
			for(unsigned int i=0; i<streams.size(); i++){
				streams[i]->start(startTime);
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
