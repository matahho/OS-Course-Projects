#ifndef CSVREADER_H
#define CSVREADER_H

#include <string>
#include <vector>



#define MAX_FIELDS 9
using namespace std;

struct Record {
    int year;
    int month;
    int day;
    int consumption[6];
};

vector<Record> readCsvFile(string path);

pair<vector<Record>, vector<Record> > separateByMonth(const vector<Record>& records);

#endif 
