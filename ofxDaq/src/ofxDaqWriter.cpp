#include "ofxDaqWriter.h"

//--------------------------------------------------------------
ofxDaqWriter::ofxDaqWriter(){
    
    filePrefix = "default";
    filePostfix = "NA";
    fileExt = ".bin";
    fileCount = -1;
    bytesWritten = 0;
    dataRate = 0.0;
}

//--------------------------------------------------------------
ofxDaqWriter::ofxDaqWriter(string dir, string pfx, string pstfx, string ext){

	fileDir = dir;
    filePrefix = pfx;
    filePostfix = pstfx;
    fileExt = ext;
    fileCount = -1;
    bytesWritten = 0;
    dataRate = 0.0;
}

ofxDaqWriter::~ofxDaqWriter(){
}

//--------------------------------------------------------------
bool ofxDaqWriter::start(int elapsedTime){
    return nextFile(elapsedTime); // This creates the first filename and opens.
}   

//--------------------------------------------------------------
bool ofxDaqWriter::stop(){
    file.close();    
    return !(file.bad());
}

//--------------------------------------------------------------
bool ofxDaqWriter::writeData(char * data, unsigned int size){
               			
    // Write data to file
	file.write(data,size); 
    bytesWritten += size;
    return !(file.bad()); 
}

//--------------------------------------------------------------
unsigned int ofxDaqWriter::getBytesWritten(){
	return bytesWritten;
}

//--------------------------------------------------------------
bool ofxDaqWriter::nextFile(int elapsedTime){

    // Close currently open file unless none have been opened yet.
    if (fileCount >= 0){
        file.close();
		bytesWritten = 0; // reset the byte counter.
        if (file.bad()){
            return false;
            
        }
    }

    // Create new file name, open, and write header if there is one defined
    if (buildFileName(elapsedTime)){
        file.open(ofFilePath::getAbsolutePath(this->fileDir + "/" + filename.c_str()).c_str(), ios::out | ios::binary);
        if (file.bad()){
            return false;
        } else {
            return !file.bad();
        }
    } else {
        return false;
    }
    
    // If made it here, there was an error
    return false;
}

//--------------------------------------------------------------
bool ofxDaqWriter::buildFileName(int elapsedTime){

    char tmpFileName[255]; // This should work on FAT32 and ext file systems
    string format;
    unsigned int length;

    fileCount++; // Make sure we don't use the same counter.

    // Use string methods to check string length, then create with sprintf and assign to filename string.
    length = filePrefix.length()+filePostfix.length()+fileExt.length()+TIMESTAMP_LENGTH+FILECOUNT_LENGTH+3;
    if (length < 255){
        format = filePrefix + "-"
            + ofGetTimestampString()
            + "-%0" + ofToString(FILECOUNT_LENGTH)+ "d-"
            + filePostfix+ "." + fileExt;
        int success = sprintf(tmpFileName,format.c_str(),fileCount);
        if (success > 0){
            filename.assign(tmpFileName);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}
