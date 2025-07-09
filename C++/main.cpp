#include <iostream>
#include <filesystem>
#include "rental_report.h"

namespace fs = std::filesystem;
using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Usage: rentalreport <equipmentdata_file> <foremancontacts_file> <equipmentprice_file> <YYYY-MM-DD>" << endl;
        return 1;
    }

    string equip_data = "csv_files/" + string(argv[1]);
    string foreman_contacts = "csv_files/" + string(argv[2]);
    string price_data = "csv_files/" + string(argv[3]);
    string date = argv[4];
    cout << "path: " + equip_data << endl;
    cout << "path: " + foreman_contacts << endl; 
    cout << "path: " + price_data << endl; 

    if (!fs::exists(equip_data) || !fs::exists(foreman_contacts) || !fs::exists(price_data)) {
        cerr << "Error: One of the files not found in the csv_files folder!" << endl;
        return 1;
    }

    removeInvoiceEntries(equip_data);
    cleanJobNum(foreman_contacts);

    // Read data from CSV files
    unordered_map<string, string> jobForemanMap = readJobForemanDetails(foreman_contacts);
    unordered_multimap<string, EquipmentPricing> pricingDataMap = readPricingData(price_data);

    // Process the rental data and create the rental report directly
    unordered_map<string, JobInfo> rentalReport = processRentalData(equip_data, jobForemanMap, pricingDataMap);

    // Generate the report
    generateRentalReport(date, rentalReport);

    return 0;
}
