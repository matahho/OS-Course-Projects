#include "csvReader.h"
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
        record.day = stoi(fields[2]);

        for (int i = 3; i < fieldCount; i++) {
            record.consumption[i - 3] = stoi(fields[i]);
        }
        rec.push_back(record);
    }

    file.close();
    return rec;
}


vector<vector<Record> > separateByMonth(const vector<Record>& records) {

    vector<vector<Record> > months(12); // Initialize with 12 empty vectors

        for (const auto& record : records) {
            months[record.month - 1].push_back(record); // Adjust for 0-based indexing
        }

        return months;
}
