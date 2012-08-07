#include "ofxDaqStream.h"

//---------------------------------------------------------------
ofxDaqStream::ofxDaqStream(){

}

//---------------------------------------------------------------
ofxDaqStream::~ofxDaqStream(){

}

//--------------------------------------------------------------
void ofxDaqStream::setFilePrefix(string prefix){
    filePrefix = prefix;
}

//--------------------------------------------------------------
void ofxDaqStream::setDataDirectory(string dataDir){
    dataDirectoryPath = dataDir;
}

//-------------------------------------------------------------
bool ofxDaqStream::writeData(){
    
	// If no data is ready then just return true (no error)
	if (!dataReady()) {
		return true;
	}
	
	// Is data is ready then read in into the dataBlock
	// If no error reading from fido then write to disk.
	if (fifo->readBlock(dataBlock,blockSize)){  
        return writer->writeData(dataBlock,blockSize);
    } else {
        return false;
    }
}

//-------------------------------------------------------------
bool ofxDaqStream::update(int startTime, bool newFile){

    if(deviceError){
        return false;
    }

    if(!this->writeData()) {
		daqLog.logError(DAQERR_FILE_WRITE_ERROR,id,name,1);
		return false;
	}
    if (newFile){
        if (!this->nextFile(startTime)){
            daqLog.logError(DAQERR_FILE_CREATE_ERROR,id,name,1);
            return false;
        }
    
    }
    return true;
}

//-------------------------------------------------------------  
bool ofxDaqStream::nextFile(int elapsedTime){
    return writer->nextFile(elapsedTime);
}

//-------------------------------------------------------------
bool ofxDaqStream::dataReady() {
    return !fifo->isEmpty();
}

//-------------------------------------------------------------
float ofxDaqStream::percentBufferFree() {
    return 100.0*(N - fifo->blocksUsed() + 0.0)/N;
}

