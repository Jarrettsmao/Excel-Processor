#include <iostream>
#include <string>
#include <filesystem>
#include "csv_utils.h"

namespace fs = std::filesystem;
using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 2){
        cerr << "Usage: removedups <csv_file>" << endl;
        return 1;
    }

    string input_csv = "csv_files/" + string(argv[1]);
    cout << "path: " + input_csv << endl; 

    if (!fs::exists(input_csv)) {
        cerr << "Error: File not found in the csv_files folder!" << endl;
        return 1;
    }

    removeDuplicates(input_csv);

    return 0;
}
