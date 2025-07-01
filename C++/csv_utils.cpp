#include "csv_utils.h"  // Include the header file
#include <iostream>
#include <fstream>
#include <set>
#include <sstream>

using namespace std;

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
