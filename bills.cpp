#include <iostream>
#include <vector>
#include "csvReader.h"

#define FIRST 1 
#define SECOND 2 
#define HOURSOFDAY 6


struct MonthlyReport
{
    int year ;
    int month ;
    int total ;
    int highHour ;
    int sumHighHour ;
};

vector<int> hourlyConsumption(vector<Record> records) {
    vector<int> sum(HOURSOFDAY, 0); // Initialize sum vector with zeros

    for (int i = 0 ; i < 30 ; i++)
        for (int j =0 ; j < HOURSOFDAY ; j++)
            sum[j] += records[i].consumption[j];
        
    return sum;
}

int findHighestHour (vector<int> sums){
    int max = 0;
    for (int i = 1 ; i < sums.size() ; i++)
        if (sums[i] > sums[max])
            max = i;
    return max ;
}

MonthlyReport MonthlyCalculation (vector<Record> r){
    MonthlyReport m1;

    m1.year = r[0].year;
    m1.month = r[0].month;  

    vector<int> hc = hourlyConsumption(r); 
    
    m1.total = 0 ;
    for (int i = 0 ; i < HOURSOFDAY ; i++)
        m1.total += hc[i];
    
    m1.highHour = findHighestHour(hc);
    m1.sumHighHour = hc[m1.highHour];

    return m1 ;

}
int main(int argc , char* argv[]) {
    string path = argv[1] ;
    vector<Record> allRecords = readCsvFile(path);
    vector<Record> m1 = separateByMonth(allRecords).first;
    vector<Record> m2 = separateByMonth(allRecords).second;


    MonthlyReport m1report = MonthlyCalculation(m1);
    MonthlyReport m2report = MonthlyCalculation(m2);
        

    //if(m1report.month == stoi(argv[2]))
    printf("%d-%d %d %d %d\n" , m1report.year , m1report.month , m1report.highHour , m1report.sumHighHour , m1report.total);
    //else if (m2report.month == stoi(argv[2]))
    printf("%d-%d %d %d %d\n" , m2report.year , m2report.month , m2report.highHour , m2report.sumHighHour , m2report.total);
    // else 
    //     printf ("ERROR : report not found\n");
}
