import openpyxl
import csv
import os
import sys

def convert_xlsx_to_csv(xlsx_filename):
    folder, filename = os.path.split(xlsx_filename)
    basename,_ = os.path.splitext(filename)

    print("folder:" + folder)
    print("filename:" + filename)
    print("basename:" + basename)

    # # Get the current working directory (which is the 'python_folder' in your case)
    # current_directory = os.getcwd()
    # # Go up one level to the root of the project
    # project_root = os.path.dirname(current_directory)
    # excelFolder = os.path.join(project_root, 'excel_files')

    # # join the path and chekc if the files exists there
    # xlsx_filepath = os.path.join(excelFolder, xlsx_filename)
    # if not os.path.isfile(xlsx_filepath):
    #     print(f"File {xlsx_filepath} not found!")
    # return

    # outputFolder = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'csv_files')

    # csv_filename = os.path.join(outputFolder, basename + ".csv")

    # os.makedirs(outputFolder, exist_ok=True)

    # wb = openpyxl.load_workbook(xlsx_filepath)

    # # select active sheet
    # sheet = wb.active

    # with open(csv_filename, 'w', newline="") as csvfile:
    #     writer = csv.writer(csvfile)

    #     #iterate through rows in the Excel sheet
    #     for row in sheet.iter_rows(values_only=True):
    #         writer.writerow(row)

    # print(f"Conversion complete: {csv_filename}")


# Entry point: Script is called from the command line with an argument
if __name__ == "__main__":
    # Check if a file name was provided as a command-line argument
    if len(sys.argv) != 2:
        print("Usage: python convert.py <filename>")
        sys.exit(1)

    # Get the file name passed as a command-line argument
    file_name = sys.argv[1]

    # Call the conversion function with the specified file name
    convert_xlsx_to_csv(file_name)