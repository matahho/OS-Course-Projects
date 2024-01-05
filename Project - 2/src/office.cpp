#include <iostream>
#include "officeCsvReader.h"
#include "utils.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>



#define ELECFILE "Electricity"
#define GASFILE "Gas"
#define WATERFILE "Water" 


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"


#define USAGE_TO_REWARD 8662

struct BuildingRecord{
    string name ;
    string comodity;
    int year;
    int month;
    int peakHour;
    int usageInPeakHour;
    int totalUsage;
    int payment;
};


void calculatePayment(vector<BuildingRecord> &records ,vector<Record> taxes ){
    for (auto &rec :records){
        Record matchTax ;
        for ( auto t : taxes)
            if (t.year == rec.year && t.month == rec.month){
                matchTax = t;
                break;
            }
        
        if (rec.comodity == WATERFILE)
            rec.payment = (rec.totalUsage + 0.25 * rec.usageInPeakHour ) * matchTax.waterCoeff;
        if (rec.comodity == ELECFILE){
            rec.payment = (rec.totalUsage + 0.25 * rec.usageInPeakHour ) * matchTax.elecCoeff;
            if (rec.totalUsage < USAGE_TO_REWARD)
                rec.payment *= 0.75;
        }
        if (rec.comodity == GASFILE)
            rec.payment = (rec.totalUsage) * matchTax.gasCoeff;
    }

}



vector<BuildingRecord> parseMessage(string message, string name) {
    stringstream iss(message);
    vector<BuildingRecord> buildingRecords;

    string line;
    string resource;  // Added variable to store resource name

    while (std::getline(iss, line)) {
        // Check if it's the first line containing the resource name
        if (line == "Electricity" || line == "Gas" || line == "Water") {
            resource = line;
            continue;  // Skip processing this line as it's not a record
        }

        BuildingRecord record;
        istringstream lineStream(line);

        lineStream >> record.year >> record.month >> record.peakHour >> record.usageInPeakHour >> record.totalUsage;
        record.month = -record.month;
        record.comodity = resource;  // Set the resource from the first line
        record.name = name;

        buildingRecords.push_back(record);
    }
    return buildingRecords;
}

string createMessageFromRecords(vector<BuildingRecord> records){
    string text ;
    for (auto r:records)
        text += r.name + " " + r.comodity + " " + to_string(r.year) + "-" + to_string(r.month) + " " + to_string(r.payment) + "\n"; 
    return text;
}



void addPipesFds(vector<string> fifos , vector<pair<int , string>>&pipefds){

    for (const auto& pipeName : fifos) {
        int fd = open((pipeName).c_str(), O_RDONLY | O_NONBLOCK);
        if (fd == -1) {
            perror(("Error opening pipe " + pipeName).c_str());
            exit(EXIT_FAILURE);
        }
        pipefds.push_back(make_pair(fd , pipeName));
    }

}





void readFromAPipe(vector<Record> taxes , vector<string> reqs ){
    for (auto req: reqs){
        cout << ("+++/tmp/"+req) << endl ;
        int fd = open(("/tmp/"+req).c_str(), O_RDONLY);
        char val[1024];
        ssize_t bytesRead;

        while ((bytesRead = read(fd, val, sizeof(val) - 1)) > 0)
            val[bytesRead] = '\0'; 
        close(fd);
        
        vector<BuildingRecord>records =  parseMessage(val , req);

        calculatePayment(records , taxes);
        for (auto a :records)
            cout << a.name << " " << a.comodity << " " << a.month << " " << a.totalUsage << " " << a.payment << endl;
    }
    
}  




int main(int argc , char* argv[]){
    string officeCsvPath = argv[1] + string("/bills.csv");
    vector<Record> taxes = readCsvFile(officeCsvPath);
    string builingPath = string(argv[1]);
    vector<string> fifos;
    getFifosNames( getAllDirectories(string(argv[1])) , fifos);
    vector<pair<int , string>> pipefds;
    addPipesFds(fifos , pipefds);




    readFromAPipe(taxes , getAllDirectories(string(argv[1])));
    
        
}