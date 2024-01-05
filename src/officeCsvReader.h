#ifndef OFFICECSVREADER_h
#define OFFICECSVREADER_h

#include <string>
#include <vector>



#define MAX_FIELDS 9
using namespace std;

struct Record {
    int year;
    int month;
    int waterCoeff;
    int gasCoeff;
    int elecCoeff;
};

vector<Record> readCsvFile(string path);


#endif 
