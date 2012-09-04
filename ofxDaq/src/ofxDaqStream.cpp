#include "ofxDaqStream.h"

//---------------------------------------------------------------
ofxDaqStream::ofxDaqStream(){
	headerSize = 0;
	blockSize = 0;
	N = 0;
	dataBlock = NULL;
	header = NULL;
	type = -1;
	dataBlockFresh = false;

}

//---------------------------------------------------------------
ofxDaqStream::~ofxDaqStream(){

}

//
bool ofxDaqStream::defineHeader(){

	header = NULL;
	headerSize = 0;

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
		// Mark that there is new data in dataBlock
		dataBlockFresh = true;
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
		else {
			// new file is okay, then create new header and write it
			if (headerSize > 0 && header != NULL){
				this->defineHeader();
				bool success = writer->writeData(header,headerSize);
				if (!success){
					daqLog.logError(DAQERR_FILE_WRITE_ERROR,id,name,1);
					return false;
				}
			}
			else{
				return true;
			}
		}
    
    }
	
    return true;
}

//-------------------------------------------------------------
bool ofxDaqStream::sendDataBlock(ofxUDPManager * udpConnection){

}

//-------------------------------------------------------------  
bool ofxDaqStream::nextFile(int elapsedTime){
	bool success = writer->nextFile(elapsedTime);
    return success;
}

//-------------------------------------------------------------
bool ofxDaqStream::dataReady() {
    return !fifo->isEmpty();
}

//-------------------------------------------------------------
float ofxDaqStream::percentBufferFree() {
    return 100.0*(N - fifo->blocksUsed() + 0.0)/N;
}

bool ofxDaqStream::checkStatus(){
	return true;
}
