#include <iostream>
#include "officeCsvReader.h"
#include "utils.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#define ELECFILE "Electricity"
#define GASFILE "Gas"
#define WATERFILE "Water" 

struct BuilingRecord{
    string comodity;
    int year;
    int month;
    int peakHour;
    int usageInPeakHour;
    int totalUsage;
};




int main(int argc , char* argv[]){
    string officeCsvPath = argv[1] + string("/bills.csv");
    vector<Record> taxes = readCsvFile(officeCsvPath);
    string builingPath = string(argv[1]);
    vector<string>allComodityName =  {ELECFILE, GASFILE, WATERFILE};
    vector<string>allBuildingName = getAllDirectories(string(argv[1]));
    vector<string>fifos;
    makingFifos(allBuildingName , allComodityName , fifos);

    


}