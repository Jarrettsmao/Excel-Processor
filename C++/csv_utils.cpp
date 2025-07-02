#include "csv_utils.h"  // Include the header file
#include <iostream>
#include <fstream>
#include <set>
#include <sstream>

using namespace std;

string trimWhiteSpace(const string& str){
    size_t first = str.find_first_not_of("\t\r\n");
    if (first == string::npos){
        return "";
    }
    size_t last = str.find_last_not_of("\t\r\n");
    return str.substr(first, (last - first + 1));
}

string removeIdFromLine(const string& line) {
    stringstream ss(line);
    string temp;
    string result;
    int columnIndex = 0;

    // Read each column and skip the first one (ID)
    while (getline(ss, temp, ',')) {
        if (columnIndex > 0) {  // Skip the first column (ID)
            result += temp + ",";
        }
        columnIndex++;
    }

    // Remove the trailing comma and return the resulting string
    if (!result.empty() && result.back() == ',') {
        result.pop_back();
    }

    return result;
}

void removeDuplicates(const string& input_csv){
    ifstream infile(input_csv);
    if (!infile.is_open()){
        cerr << "Error opening input file!" << endl;
        return;
    }

    //set to store unique rows
    set<string> seen;
    string line;
    ostringstream cleaned_data; //holds cleaned data as a string stream

    //Read the CSV file line by line
    while (getline(infile, line)){
        line = removeIdFromLine(line);
        line = trimWhiteSpace(line);

        // Debugging: Print the line after trimming
        cout << "Trimmed line: '" << line << "'" << endl;

        if (seen.find(line) == seen.end()){
            seen.insert(line);
            cleaned_data << line << endl;
        }
    } 

    infile.close();

    //Open the same file for writing 
    ofstream outfile(input_csv, ofstream::trunc);
    if(!outfile.is_open()){
        cerr << "Error opening output file!" << endl;
        return;
    }

    outfile << cleaned_data.str();
    outfile.close();

    cout << "Duplicates removed. Cleaned data saved to " << input_csv << endl;
}
