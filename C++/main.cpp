#include <iostream>
#include <filesystem>
#include "rental_report.h"

namespace fs = std::filesystem;
using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: rentalreport <equipmentdata_file> <foremancontacts_file> <YYYY-MM-DD>" << endl;
        return 1;
    }

    string equip_data = "csv_files/" + string(argv[1]);
    string foreman_contacts = "csv_files/" + string(argv[2]);
    string date = argv[3];
    cout << "path: " + equip_data << endl;
    cout << "path: " + foreman_contacts << endl; 

    if (!fs::exists(equip_data) && !fs::exists(foreman_contacts)) {
        cerr << "Error: One of the files not found in the csv_files folder!" << endl;
        return 1;
    }

    generateRentalReport(equip_data, date, readJobForemanDetails(foreman_contacts));

    return 0;
}
