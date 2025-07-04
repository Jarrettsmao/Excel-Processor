#ifndef RENTAL_REPORT_H
#define RENTAL_REPORT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

// Function to generate a rental report, calculating days onsite for each equipment type
void generateRentalReport(const std::string& equip_data, const std::string& date, const std::unordered_map<std::string, std::string>& jobForemanMap);

std::unordered_map<std::string, std::string> readJobForemanDetails(const std::string& foremanCsvFile);

#endif
