#include "officeCsvReader.h"
#include <iostream>
#include <fstream>
#include <sstream>

vector<Record> readCsvFile(string path) {
    ifstream file(path);

    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        return vector<Record>(); // Return an empty vector
    }

    string line;
    string fields[MAX_FIELDS];
    vector<Record> rec;

    // Read and discard the header line
    getline(file, line);

    while (getline(file, line)) {
        istringstream iss(line);
        string token;
        int fieldCount = 0;

        // Parse and store each field
        while (getline(iss, token, ',')) {
            fields[fieldCount++] = token;
        }

        // Store fields in the Record struct
        Record record;
        record.year = stoi(fields[0]);
        record.month = stoi(fields[1]);
        record.waterCoeff = stoi(fields[2]);
        record.gasCoeff = stoi(fields[3]);
        record.elecCoeff = stoi(fields[4]);


        rec.push_back(record);
    }

    file.close();
    return rec;
}

