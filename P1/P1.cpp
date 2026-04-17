#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int MAX_RECORDS = 100;
const int MAX_BUCKETS = 60;
const int HASH_SIZE = 100;


class Bucket
{
    public:
    int records[MAX_RECORDS];
    int numRecords;
    int nextBucket;
    Bucket() : numRecords(0), nextBucket(-1) {}
    bool insertRecord(int record)
    {
        if (numRecords < MAX_RECORDS)
        {
            records[numRecords++] = record;
            return true;
        }
        return false;
    }

    int operator[](int index);
};

int Bucket::operator[](int index)
{
    if (index >= 0 && index < numRecords)
    {
        return records[index];
    }
    throw out_of_range("Index out of range");
}

class HashFile
{
public:
    Bucket buckets[HASH_SIZE];


    int hash(int key)
    {
        return key % MAX_BUCKETS;
    }

    bool addNewRegister(int record_key){
            int key = hash(record_key);
            if (!buckets[key].insertRecord(record_key)) {
                int nextBucket = buckets[key].nextBucket;
                while (nextBucket != -1) {
                    if (buckets[nextBucket].insertRecord(record_key)) {
                        return true;
                    }
                    nextBucket = buckets[nextBucket].nextBucket;
                }
                //this is for separate the current section and the overflow section for the blocks
                for (int i = MAX_BUCKETS; i < HASH_SIZE; ++i) {
                    if (buckets[i].nextBucket == -1) {
                        buckets[key].nextBucket = i;
                        return buckets[i].insertRecord(record_key);
                    }
                }
                return false;
            }
            return true;
    }

    int searchRegister(int record_key){
        int key = hash(record_key);
        for (int i = 0; i < buckets[key].numRecords; ++i) {
            if (buckets[key][i] == record_key) {
                return key;
            }
        }
        int nextBucket = buckets[key].nextBucket;
        while (nextBucket != -1) {
            for (int i = 0; i < buckets[nextBucket].numRecords; ++i) {
                if (buckets[nextBucket][i] == record_key) {
                    return nextBucket;
                }
            }
            nextBucket = buckets[nextBucket].nextBucket;
        }
        return -1;
    }

    bool deleteRegister(int record_key){
        int key = hash(record_key);
        for (int i = 0; i < buckets[key].numRecords; ++i) {
            if (buckets[key][i] == record_key) {
                //we are gonna move the last record to the current position to fill the gap
                buckets[key].records[i] = buckets[key].records[--buckets[key].numRecords];
                return true;
            }
        }
        int nextBucket = buckets[key].nextBucket;
        while (nextBucket != -1) {
            for (int i = 0; i < buckets[nextBucket].numRecords; ++i) {
                if (buckets[nextBucket][i] == record_key) {
                    //the same o.o
                    buckets[nextBucket].records[i] = buckets[nextBucket].records[--buckets[nextBucket].numRecords];
                    return true;
                }
            }
            nextBucket = buckets[nextBucket].nextBucket;
        }
        return false;
    }
};


ostream& operator<<(ostream& os, const Bucket& bucket)
{
    os << "Bucket with " << bucket.numRecords << " records: ";
    for (int i = 0; i < bucket.numRecords; ++i) {
        os << bucket.records[i] << " ";
    }
    return os;
}

ostream& operator<<(ostream& os, const HashFile& hashFile)
{
    for (int i = 0; i < HASH_SIZE; ++i) {
        if (hashFile.buckets[i].numRecords > 0) {
            os << "Bucket " << i << ": " << hashFile.buckets[i] << endl;
        }
    }
    return os;
}


int main(int argc, char* argv[])
{
    HashFile hashFile;
    string filePath = argv[1];
    ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filePath << endl;
        return 1;
    }

    int record;
    while (inputFile >> record) {
        hashFile.addNewRegister(record);
    }

    cout << hashFile << endl;

    return 0;
}