// A Lockless thread safe circular buffer for a single producer/ single consumer 
class CircularFifo {

    public:

        CircularFifo() {}

        CircularFifo(int blockSize_, int N_) {
            try {
                buffer = (char*)malloc(blockSize_*N_);
                if (buffer == NULL) 
                    throw "Memory allocation error";
                capacity = N_;
                blockSize = blockSize_;
                blocksWritten = 0;
                blocksRead = 0;
            }
            catch(const char* message) {
                cout<<message<<endl;
                capacity = 0;
                blockSize = 0;
            }           
            head = 0;
            tail = 0;
        }

        ~CircularFifo() {
            try {
                if (buffer == NULL)
                    throw "Trying to free null pointer";
                free(buffer);
            }
            catch(const char* message) {
                cout<<message<<endl;
            }
        }

        // Read: can only modify head
        virtual bool readBlock(char * dataBuffer, unsigned int bufferSize) {

            if (!(head == tail)) {
                try {
                    if (bufferSize != blockSize)
                        throw "Mismatch in buffer size to read";
                    memcpy(dataBuffer,buffer + head*blockSize,blockSize);             
                    head = increment(head);
                    blocksRead++;
                }
                catch(const char* message) {
                    cout<<message<<endl;
                    return false;
                }
                return true;
            } else {
                cout << "BUFFER EMPTY" << endl;
                return false;
            }
        }

        // Write: can only modify tail.
        virtual bool writeBlock(char * dataBuffer, unsigned int bufferSize) {  

            unsigned int nextTail = increment(tail);
            if (nextTail != head) {
                try {
                    if (bufferSize != blockSize)
                        throw "Mismatch in buffer size to write";
                    memcpy(buffer + tail*blockSize,dataBuffer,blockSize);
                    tail = nextTail;
                    blocksWritten++;
                }
                catch(const char* message) {
                    cout<<message<<endl;
                    return false;
                }
                return true;
            }
            else {
                cout << "BUFFER FULL" << endl;
                return false;            
            }
        }

        virtual bool isEmpty() {
            return head == tail;
        }

        virtual bool isFull() {
            unsigned int tailCheck = (tail+1) % capacity;
            return (tailCheck == head);
        }

        virtual int blocksUsed() {
            int bf = blocksWritten - blocksRead;
            return bf;
        }

    protected:

        virtual unsigned int increment(unsigned int idx_) {
            idx_ = (idx_+1) % capacity;
            return idx_;
        }
             

        unsigned int blockSize;        // Size of a data block.
        volatile unsigned int head;    // Index of the head block.
        volatile unsigned int tail;    // Index of the tail block.
        unsigned int capacity;         // The number of blocks in buffer.
        int blocksWritten;
        int blocksRead;
        char * buffer;                 // Data buffer.

};        
