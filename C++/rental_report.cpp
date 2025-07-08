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

//helper function print out pricing data
void printPricingMap(const std::unordered_multimap<std::string, EquipmentPricing>& pricingMap) {
    // Iterate through the map
    for (const auto& entry : pricingMap) {
        const std::string& company = entry.first;  // The key (company)
        const EquipmentPricing& pricing = entry.second;  // The value (equipment pricing)

        std::cout << "Company: " << company << "\n";  // Print the key (company)
        std::cout << "Description: " << pricing.description << "\n";
        std::cout << "1 Day Price: " << pricing.oneDayPrice << "\n";
        std::cout << "1 Week Price: " << pricing.oneWeekPrice << "\n";
        std::cout << "4 Week Price: " << pricing.fourWeekPrice << "\n";
        std::cout << "----------------------------\n";
    }
}

//reads the pricing data from EquipmentPrice sheet
unordered_multimap<string, EquipmentPricing> readPricingData(const string& pricingFile){
    unordered_multimap<string, EquipmentPricing> pricingMap;
    ifstream file(pricingFile);
    string line;

    for (int i = 0; i < 19; i++){
        getline(file, line); //skips header line and unnecessary lines
    }
    // cout << line << endl;
    while (getline(file, line)){
        string company, description;
        float oneDayPrice, oneWeekPrice, fourWeekPrice;
        string temp;
        stringstream ss(line);

        getline(ss, temp, ','); //skip updated date
        getline(ss, temp, ','); //skip type
        getline(ss, company, ',');
        getline(ss, description, ','); 
        getline(ss, temp, ','); //skip make
        getline(ss, temp, ','); //skip model

        ss >> oneDayPrice;
        ss.ignore();
        ss >> oneWeekPrice;
        ss.ignore();
        ss >> fourWeekPrice;

        description = trimWhiteSpace(description);

        pricingMap.insert({company, {description, oneDayPrice, oneWeekPrice, fourWeekPrice}});
    }

    // printPricingMap(pricingMap);

    return pricingMap;
}

//gets job number and foreman name
unordered_map<string, string> readJobForemanDetails(const string& foremanFile){
    unordered_map<string, string> jobForemanMap;
    ifstream file(foremanFile);
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

// Function to read out job data into the rentalReport map and return it
unordered_map<string, unordered_map<string, int>> processRentalData(const string& equip_data, const unordered_map<string, string>& jobForemanMap) {
    unordered_map<string, unordered_map<string, int>> rentalReport;
    ifstream file(equip_data);
    string line;

    // Skip the header line
    getline(file, line);

    while(getline(file, line)){
        string job, foreman, type, description, startDate, endDate;

        // Split the line into the above
        stringstream ss(line);
        getline(ss, job, ',');
        getline(ss, type, ',');

        // Skip unnecessary columns
        string temp;
        getline(ss, temp, ','); // Skip PO#
        getline(ss, temp, ','); // Skip Supplier
        getline(ss, startDate, ',');

        // Skip unnecessary columns
        getline(ss, temp, ','); // Skip Delivery Time
        getline(ss, description, ',');
        description = trimWhiteSpace(description);

        // Skip unnecessary columns
        getline(ss, temp, ','); // Skip notes
        getline(ss, temp, ','); // Skip status/call off number
        getline(ss, endDate, ',');

        // Get the foreman from the job map
        foreman = jobForemanMap.count(job) ? jobForemanMap.at(job) : "N/A";

        // Process only rental jobs with valid dates
        if (type == "Rental" && !startDate.empty() && !endDate.empty()) {
            int daysOnSite = dateDifference(startDate, endDate);
            rentalReport[job][description] += daysOnSite;
        }
    }

    return rentalReport;
}


// Function to generate the rental report using the rentalReport map and pricing data
void generateRentalReport(const string& date, const unordered_map<string, unordered_map<string, int>>& rentalReport) {
    // Create reports folder if it doesn't exist
    string reportsDir = "reports";
    if (!fs::exists(reportsDir)) {
        fs::create_directory(reportsDir);
    }

    // Create output file in reports folder
    string outputFile = reportsDir + "/report-" + date + ".txt";
    ofstream reportFile(outputFile);

    if (!reportFile.is_open()) {
        cerr << "Error: Unable to create the report file!" << endl;
        return;
    }

    // Print the rental report
    reportFile << "Rental Report\n";
    reportFile << "--------------------------------\n";

    // Iterate over each job
    for (const auto& jobEntry : rentalReport) {
        string job = jobEntry.first;
        string foreman = "N/A"; // Default to "N/A"
        // Use jobForemanMap to look up the foreman (optional, you can pass it into this function if needed)
        reportFile << "Job #" << job << "- Foreman: " << foreman << "\n";

        // Iterate over each equipment description for this job
        for (const auto& descriptionEntry : jobEntry.second) {
            reportFile << "  " << descriptionEntry.first << ": " << descriptionEntry.second << " days onsite\n";
        }

        reportFile << "\n";
    }

    cout << "Rental report has been saved to: " << outputFile << endl;

    reportFile.close();
}
