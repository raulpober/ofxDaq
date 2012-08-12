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
	
	// If we made it here, then there is fresh data in the dataBlock
	dataBlockFresh = true;
	
    return true;
}

//-------------------------------------------------------------
bool ofxDaqStream::sendDataBlock(ofxUDPManager * udpConnection){

	
	// Prepare the data to send:
	//
	// One byte: TypeID
	// One byte: multiMessage? 0-> all in one message, 1 -> multiple meesages
	// Total Bytes (includes 2 above)
	// Header Bytes
	// Data Bytes
	//
	// Total length (bytes) = 2 + headerLength + dataLength
	
	if (!dataBlockFresh){
		return true;
	}
	
	int maxSize = 65000;
	int sent = 0;
	int totalSize;
	cout << "Total Size: " << totalSize << endl;
	int index = 0;
	totalSize = 2*sizeof(char) + 1*sizeof(int) +  blockSize + headerSize;
	char * msg = (char*)malloc(totalSize);
	if (msg == NULL){
		cout << "Could not create msg" << endl;
		return false;
	}
	if (totalSize > maxSize){
		// Multi-msg
		msg[0] = type;
		index += 1;
		msg[1] = 1; // multi-msg data
		index += 1;
		memcpy(msg+index,(char*)&totalSize,sizeof(totalSize));
		index += sizeof(totalSize);
		if (header != NULL){
			memcpy(msg+index,header,headerSize);
			index += headerSize;
		}
		memcpy(msg+index,dataBlock,blockSize);
		index += blockSize;
		
		index = 0;
		int txSize = maxSize;
		while (txSize > 0){
			sent += udpConnection->Send(msg + index,txSize);
			index += txSize;
			txSize = totalSize - index;
			if (txSize > maxSize){
				txSize = maxSize;
			}
		}
		
	} 
	else{
		msg[0] = type;
		index += 1;
		msg[1] = 0; // single msg data
		index += 1;
		memcpy(msg+index,&totalSize,sizeof(int));
		index += sizeof(int);
		if (header != NULL){
			memcpy(msg+index,header,headerSize);
			index += headerSize;
		}
		memcpy(msg+index,dataBlock,blockSize);
		index += blockSize;
		sent = udpConnection->Send(msg,totalSize);
	}
	
	// Mark the data block as being stale
	dataBlockFresh = false;
	
	free(msg);
	
	return totalSize == sent;
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

