#include "rental_report.h"
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <filesystem>
#include <regex>

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

//helper function to remove lines that dont start with job # for foremancontacts.csv
void cleanJobNum(const string& inputFile){
    ifstream file(inputFile);
    if (!file.is_open()) {
        cerr << "Error: Unable to open the file " << inputFile << endl;
        return;
    }

    string tempFile = inputFile + ".temp";
    ofstream outFile(tempFile);
    if (!outFile.is_open()){
        cerr << "Error: Unable to create temporary file." << endl;
        return;
    }

    string line;
    bool isFirstLine = true;

    regex timePattern(R"(^\d{1,2}:\d{2} (AM|PM))");

    while (getline(file, line)){
        stringstream ss(line);
        string job;

        getline(ss, job, ',');
        job = trimWhiteSpace(job);

        if (isFirstLine){
            outFile << line << endl;
            isFirstLine = false;
        } else if (!job.empty() && isdigit(job[0]) && !regex_search(job, timePattern)){
            outFile << line << endl;
        }
    }

    file.close();
    outFile.close();

    // Replace the original file with the new one
    if (remove(inputFile.c_str()) != 0) {
        cerr << "Error: Unable to delete the original file." << endl;
    } else {
        if (rename(tempFile.c_str(), inputFile.c_str()) != 0) {
            cerr << "Error: Unable to rename temporary file." << endl;
        }
    }

    cout << "Non-numeric job entries have been removed from " << inputFile << endl;
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

//helper function print out jobforemanmap
void printJobForemanMap(const std::unordered_map<std::string, std::string>& jobForemanMap) {
    std::cout << "JobForemanMap Contents:" << std::endl;
    for (const auto& entry : jobForemanMap) {
        std::cout << "Job: " << entry.first << " => Foreman: " << entry.second << std::endl;
    }
    std::cout << "--------------------------------" << std::endl;
}


//helper function to delete invoice entries in the csv
void removeInvoiceEntries(const string& inputFile){
    ifstream file(inputFile);
    if (!file.is_open()){
        cerr << "Error: Unable to open the file" << inputFile << endl;
        return;
    }

    string tempFile = inputFile + ".temp";
    ofstream outFile(tempFile);
    if(!outFile.is_open()){
        cerr << "Error: Unable to create temporary file." << endl;
        return;
    } 

    string line;
    bool isFirstLine = true;
    while (getline(file, line)){
        stringstream ss(line);
        string job, type;

        //read job and type columns
        getline(ss, job, ',');
        getline(ss, type, ',');

        //skip invoice entries
        if (type != "Invoice"){
            if (isFirstLine){
                outFile << line << endl;
                isFirstLine = false;
            } else {
                outFile << line << endl;
            }
        }
    }

    file.close();
    outFile.close();

    //Replace the original file with the new one
    if (remove(inputFile.c_str()) != 0){
        cerr << "Error: Unable to delete original file" << endl;
    } else {
        if (rename(tempFile.c_str(), inputFile.c_str()) != 0){
            cerr << "Error: Unable to rename temporary file." << endl;
        }
    }

    cout << "Invoice entries have been removed from " << inputFile << endl; 
}

//helper function to calculate rental costs and check if for example 4 days is greater than 1 weekthen swap to 1 week price
float calculateRentalCost(int daysOnSite, const EquipmentPricing& pricing){
    //calculate # of 4-week blocks
    int fourWeeks = daysOnSite / 28;
    daysOnSite = daysOnSite - (fourWeeks * 28);

    //calculate # of 1-week blocks
    int weeks = daysOnSite / 7;
    daysOnSite = daysOnSite - (weeks * 7);

    //calculate remaining days after full weeks
    int days = daysOnSite;

    //Calculate the price for 1-day, 1-week, & 4-week based on the daysOnSite
    float fourWeekCost = fourWeeks * pricing.fourWeekPrice;
    float oneWeekCost = weeks * pricing.oneWeekPrice;
    float oneDayCost = days * pricing.oneDayPrice; 

    //if the cost of the days exceeds one-week cost, replace with one-week price
    if (oneDayCost > pricing.oneWeekPrice){
        oneDayCost = pricing.oneWeekPrice;
    }

    oneWeekCost += oneDayCost;

    //If the cost of the weeks exceeds the four-week cost, replace with the four-week price
    if (oneWeekCost > pricing.fourWeekPrice){
        oneWeekCost = pricing.fourWeekPrice;
    }

    //Calculate total cost by summing up the costs
    float total = oneWeekCost + fourWeekCost;

    return total;
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

        if (foreman == ""){
            foreman = "N/A";
        }

        //Store the job and foreman in the map
        jobForemanMap[job] = foreman;
    }

    // printJobForemanMap(jobForemanMap);

    return jobForemanMap;
}

// unoredered_map<string, unordered_map<>

// Function to read out job data into the rentalReport map and return it
unordered_map<string, JobInfo> processRentalData(const string& equip_data, unordered_map<string, string>& jobForemanMap, const unordered_multimap<string, EquipmentPricing>& pricingMap) {
    unordered_map<string, JobInfo> rentalReport;
    ifstream file(equip_data);
    string line;

    // Skip the header line
    getline(file, line);

    while(getline(file, line)){
        string job, foreman, type, supplier, description, startDate, endDate;

        // Split the line into the above
        stringstream ss(line);
        getline(ss, job, ',');
        getline(ss, type, ',');

        // Skip unnecessary columns
        string temp;
        getline(ss, temp, ','); // Skip PO#
        getline(ss, supplier, ',');
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
        foreman = jobForemanMap[job];
        // if (foreman.empty()) {
        //     cout << "Foreman for job " << job << " is empty!" << endl;
        // }
        // else {
        //     cout << "Foreman for job " << job << " is: " << foreman << endl;
        // }

        // Process only rental jobs with valid dates
        if (type == "Rental" && !startDate.empty() && !endDate.empty()) {
            int daysOnSite = dateDifference(startDate, endDate);

            if (rentalReport.count(job) == 0){
                rentalReport[job] = JobInfo(foreman); //create a new jobinfo if the job doesn't exist
            }

            auto range = pricingMap.equal_range(supplier);
            float equipmentCost = 0.0f;

            //Find the corresponding equipment pricing for this description
            for (auto it = range.first; it != range.second; it++){
                if (it->second.description == description){
                    equipmentCost = calculateRentalCost(daysOnSite, it->second);
                    break;
                }
            }

            rentalReport[job].equipment[description] += daysOnSite;
            rentalReport[job].totalCost += equipmentCost;

            // cout << "Job: " << job 
            // << " description: " << description
            // <<  " Total Cost: $" << rentalReport[job].totalCost 
            // << endl;
        }
    }
    return rentalReport;
}


// Function to generate the rental report using the rentalReport map and pricing data
void generateRentalReport(const string& date, const unordered_map<string, JobInfo>& rentalReport) {
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
        const string& job = jobEntry.first;
        const JobInfo& jobInfo = jobEntry.second;
        
        reportFile << "Job #" << job << "- Foreman: " << jobInfo.foreman << ", Total Cost: $ " << jobInfo.totalCost << "\n";

        // Iterate over each equipment description for this job
        for (const auto& equipmentEntry : jobInfo.equipment) {
            reportFile << "  " << equipmentEntry.first << ": " << equipmentEntry.second << " days onsite\n";
        }

        reportFile << "\n";
    }
    reportFile << "--------------------------------\n";
    reportFile << "Note: Older jobs may have innacurate sum totals due to missing/incorrect data entry.";

    cout << "Rental report has been saved to: " << outputFile << endl;

    reportFile.close();
}
