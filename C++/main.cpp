#include <iostream>
#include <string>
#include "csv_utils.h"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 2){
        cerr << "Usage: removedups <csv_file>" << endl;
        return 1;
    }

    string input_csv = "csv_files/" + string(argv[1]);

    if (!fs::exists(input_csv)) {
        cerr << "Error: File not found in the csv_files folder!" << endl;
        return 1;
    }

    removeDuplicates(input_csv);

    return 0;
}
