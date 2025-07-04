#include "rental_report.h"
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

string trimWhiteSpace(const string& str){
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos){
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

//helper function to calculate the difference in days between two dates
int dateDifference(const string& startDate, const string& endDate){
    tm startTm = {};
    tm endTm = {};

    stringstream ssStart(startDate), ssEnd(endDate);
    
    ssStart >> get_time(&startTm, "%Y-%m-%d");
    ssEnd >> get_time(&endTm, "%Y-%m-%d");
    
    //convert from type tm to time_t
    time_t start_time = mktime(&startTm);
    time_t end_time = mktime(&endTm);
    
    return difftime(end_time, start_time) / (60 * 60 * 24) + 1; //convert seconds to days //need to add 1 to represent the days properly
}

//gets job number and foreman name
unordered_map<string, string> readJobForemanDetails(const string& foremanCsvFile){
    unordered_map<string, string> jobForemanMap;
    ifstream file(foremanCsvFile);
    string line;

    //skip header line
    getline(file, line);

    while(getline(file, line)){
        string job, foreman;
        string temp;
        stringstream ss(line);

        getline(ss, job, ',');
        getline(ss, temp, ','); //skip job name
        getline(ss, foreman, ',');

        //Store the job and foreman in the map
        jobForemanMap[job] = foreman;
    }
    return jobForemanMap;
}

//Function to process rental data and generate a rental report
void generateRentalReport(const string& equip_data, const string& date, const unordered_map<string, string>& jobForemanMap){
    //use a map to sum days byt equipment description
    unordered_map<string, unordered_map<string, int>> rentalReport;

    ifstream file(equip_data);
    string line;

    getline(file, line);

    while(getline(file, line)){
        string job, foreman, type, description, startDate, endDate;

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
        description = trimWhiteSpace(description);

        //Skip unncessary columns
        getline(ss, temp, ','); //skip notes
        getline(ss, temp, ','); //skip status/call off number

        getline(ss, endDate, ',');

        if (jobForemanMap.find(job) != jobForemanMap.end()){
            foreman = jobForemanMap.at(job);
        } else {
            foreman = "N/A"; 
        }

        if (type == "Rental" && !startDate.empty() && !endDate.empty()) {
            int daysOnSite = dateDifference(startDate, endDate);
            rentalReport[job][description] += daysOnSite;
        }
    }

    //create reports folder 
    string reportsDir = "reports";
    if(!fs::exists(reportsDir)){
        fs::create_directory(reportsDir);
    }

    //create output file in reports folder
    string outputFile = reportsDir + "/report-" + date + ".txt";
    ofstream reportFile(outputFile);

    if(!reportFile.is_open()){
        cerr << "Error: Unable to create the report file!" << endl;
        return;
    }

    // Print the rental report
    reportFile << "Rental Report\n";
    reportFile << "--------------------------------\n";
    
    // Iterate over each job
    for (const auto& jobEntry : rentalReport) {
        string foreman = jobForemanMap.at(jobEntry.first);
        if (foreman == ""){
            foreman = "N/A";
        }
        reportFile << "Job #" << jobEntry.first << "- Foreman:" << foreman << "\n";
        // reportFile << "Foreman: " << jobEntry.

        // Iterate over each equipment description for this job
        for (const auto& descriptionEntry : jobEntry.second) {
            reportFile << "  " << descriptionEntry.first << ": " << descriptionEntry.second << " days onsite\n";
        }

        reportFile << "\n";
    }

    cout << "Rental report has been saved to: " << outputFile << endl;

    reportFile.close();
}