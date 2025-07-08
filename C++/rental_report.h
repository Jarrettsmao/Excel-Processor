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

// Functions

// Reads job foreman details from a CSV file and returns a map
std::unordered_map<std::string, std::string> readJobForemanDetails(const std::string& foremanFile);

// Reads pricing data details from a CSV file and returns a map
std::unordered_multimap<std::string, EquipmentPricing> readPricingData(const std::string& pricingFile);

// Processes rental data and returns a map with job descriptions and days onsite
std::unordered_map<std::string, std::unordered_map<std::string, int>> processRentalData(
    const std::string& equip_data, 
    const std::unordered_map<std::string, std::string>& jobForemanMap
);

// Function to generate a rental report, calculating days onsite for each equipment type
void generateRentalReport(
    const std::string& date, 
    const std::unordered_map<std::string, std::unordered_map<std::string, int>>& rentalReport
);

#endif