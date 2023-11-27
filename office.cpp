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
        text += r.name + " " + r.comodity + " " + to_string(r.year) + "-" + to_string(r.month) + " " + to_string(r.peakHour) + " " + to_string(r.usageInPeakHour) + " " + to_string(r.totalUsage) + " " + to_string(r.payment) + "\n"; 
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




vector<BuildingRecord> handlePipeData(pair<int , string> pairFdName , vector<Record> &taxes) {
    char buffer[1024];
    ssize_t bytesRead = read(pairFdName.first , buffer, sizeof(buffer) - 1);
    if (bytesRead == -1) {
        perror("Error reading from pipe");
        exit(EXIT_FAILURE);
    } else if (bytesRead == 0) {
        cerr << "Pipe closed" << endl;
        close(pairFdName.first);
        exit(EXIT_SUCCESS);
    }

    buffer[bytesRead] = '\0';
    vector<BuildingRecord> buildingRecords = parseMessage(buffer , pairFdName.second);
    calculatePayment(buildingRecords , taxes );
    

    cout << YELLOW <<"RECEIVED data from building" << RESET << endl;
    
    return buildingRecords;
    
}


int main(int argc , char* argv[]){
    string officeCsvPath = argv[1] + string("/bills.csv");
    vector<Record> taxes = readCsvFile(officeCsvPath);
    string builingPath = string(argv[1]);
    vector<string> fifos;
    //makingFifos(getAllDirectories(string(argv[1])) , fifos);
    getFifosNames( getAllDirectories(string(argv[1])) , fifos);
    vector<pair<int , string>> pipefds;
    addPipesFds(fifos , pipefds);


    string reducedText ;

    while (true) {
        fd_set readSet;
        FD_ZERO(&readSet);

        int maxfd = -1;
        for (const auto& p : pipefds) {
            int fd = p.first;
            FD_SET(fd, &readSet);
            if (fd > maxfd) {
                maxfd = fd;
            }
        }

        int ready = select(maxfd + 1, &readSet, nullptr, nullptr, nullptr);

        if (ready == -1) {
            perror("Error in select");
            exit(EXIT_FAILURE);
        }

        if (ready > 0) {
            for (const auto& p : pipefds) {
                int fd = p.first;
                if (FD_ISSET(fd, &readSet)) {
                    string m = createMessageFromRecords( handlePipeData(p, taxes)) +"\n";
                    printf("%s" , m.c_str());
                }
            }
        }


        FD_ZERO(&readSet);
    }




}