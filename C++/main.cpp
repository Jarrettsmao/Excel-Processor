#include <iostream>
#include <filesystem>
#include "rental_report.h"

namespace fs = std::filesystem;
using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: rentalreport <csv_file>" << endl;
        return 1;
    }

    string input_csv = "csv_files/" + string(argv[1]);
    cout << "path: " + input_csv << endl; 

    if (!fs::exists(input_csv)) {
        cerr << "Error: File not found in the csv_files folder!" << endl;
        return 1;
    }

    generateRentalReport(input_csv);

    return 0;
}
