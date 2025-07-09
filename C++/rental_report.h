#ifndef RENTAL_REPORT_H
#define RENTAL_REPORT_H

#include <string>
#include <unordered_map>
#include <iostream>

// Structures
struct EquipmentPricing {
    std::string description;
    float oneDayPrice;
    float oneWeekPrice;
    float fourWeekPrice;
};

struct JobInfo {
    std::string foreman;
    std::unordered_map<std::string, int> equipment; //equipment description 
    float totalCost = 0.0f; //total cost for the job

    //default constructor (helps handle map initialization)
    JobInfo():foreman(""){}

    //constructor when foreman is passed
    JobInfo(std::string foreman):foreman(foreman){}
};

// Functions

//delete invoices from csv file
void removeInvoiceEntries(const std::string& inputFile);

//delete non job# lines from foreman contacts csv
void cleanJobNum(const std::string& inputFile);

//calculate rental costs based on how many days onsite
float calculateRentalCost(int daysOnSite, const EquipmentPricing& pricing);

// Reads job foreman details from a CSV file and returns a map
std::unordered_map<std::string, std::string> readJobForemanDetails(const std::string& foremanFile);

// Reads pricing data details from a CSV file and returns a map
std::unordered_multimap<std::string, EquipmentPricing> readPricingData(const std::string& pricingFile);

// Processes rental data and returns a map with job descriptions and days onsite
std::unordered_map<std::string, JobInfo> processRentalData(
    const std::string& equip_data, 
    std::unordered_map<std::string, std::string>& jobForemanMap,
    const std::unordered_multimap<std::string, EquipmentPricing>& pricingMap
);

// Function to generate a rental report, calculating days onsite for each equipment type
void generateRentalReport(
    const std::string& date, 
    const std::unordered_map<std::string, JobInfo>& rentalReport
);

#endif