#include "rental_report.h"
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>

using namespace std;

//helper function to calculate the difference in days between two dates
int dateDifference(const string& startDate, const string& endDate){
    tm startTm = {};
    tm endTm = {};

    stringstream ssStart(startDate), ssEnd(endDate);
    
    ssStart >> get_time(&startTm, "%m/%d/%Y");
    ssStart >> get_time(&endTm, "%m/%d/%Y");
    
    //convert from type tm to time_t
    time_t start_time = mktime(&startTm);
    time_t end_time = mktime(&endTm);
    
    return difftime(end_time, start_time) / (60 * 60 * 24); //convert seconds to days
}

//Function to process rental data and generate a rental report
void generateRentalReport(const string& input_csv){
    //use a map to sum days byt equipment description
    unordered_map<string, unordered_map<string, int>> rentalReport;

    ifstream file(input_csv);
    string line;

    getline(file, line);

    while(getline(file, line)){
        string job, type, description, startDate, endDate;

        //Split the line into the above
        stringstream ss(line);
        getline(ss, job, ',');
        getline(ss, type, ',');

        //Skip unncessary columns
        string temp;
        getline(ss, temp, ','); //skip PO#
        getline(ss, temp, ','); //Skip Supplier

        getline(ss, startDate, ',');

        //Skip unncessary columns
        getline(ss, temp, ','); //Skip Delivery Time

        getline(ss, description, ',');

        //Skip unncessary columns
        getline(ss, temp, ','); //skip notes
        getline(ss, temp, ','); //skip status/call off number

        getline(ss, endDate, ',');

        if (type == "Rental" && !startDate.empty() && !endDate.empty()) {
            int daysOnSite = dateDifference(startDate, endDate);
            rentalReport[job][description] += daysOnSite;
        }
    }

    // Print the rental report
    cout << "Rental Report\n";
    cout << "--------------------------------\n";
    
    // Iterate over each job
    for (const auto& jobEntry : rentalReport) {
        cout << "Job #" << jobEntry.first << ":\n";

        // Iterate over each equipment description for this job
        for (const auto& descriptionEntry : jobEntry.second) {
            cout << "  " << descriptionEntry.first << ": " << descriptionEntry.second << " days onsite\n";
        }

        cout << "\n";
    }
}