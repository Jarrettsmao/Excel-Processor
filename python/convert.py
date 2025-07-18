import openpyxl
import csv
import os
import sys

def convert_xlsx_to_csv(xlsx_filename):
    folder, filename = os.path.split(xlsx_filename)
    basename,_ = os.path.splitext(filename)

    # Get the current working directory (which is the 'python_folder' in this case)
    current_directory = os.getcwd()
    # Go up one level to the root of the project
    # project_root = os.path.abspath(os.path.join(current_directory))

    excelFolder = os.path.join(current_directory, 'excel_files')

    # join the name and path and check if the files exists there
    xlsx_filepath = os.path.join(excelFolder, xlsx_filename)
    if not os.path.isfile(xlsx_filepath):
        print(f"File {xlsx_filepath} not found!")
        return
    else:
        print(f"File {xlsx_filepath} present!")

    #creates a folder in the python directory
    # outputFolder = os.path.join((os.path.abspath(__file__)), 'csv_files') 
    outputFolder = os.path.join(current_directory, 'csv_files')

    if os.path.exists(outputFolder):
        print(f"Output folder '{outputFolder}' already exists.")
    else:
        print(f"Creating output folder '{outputFolder}'.")

    os.makedirs(outputFolder, exist_ok=True)
    
    # creates new csv file 
    csv_filename = os.path.join(outputFolder, basename + ".csv")

    wb = openpyxl.load_workbook(xlsx_filepath)

    # select active sheet
    sheet = wb.active

    with open(csv_filename, 'w', newline="") as csvfile:
        writer = csv.writer(csvfile)

        #iterate through rows in the Excel sheet
        for row in sheet.iter_rows(values_only=True):
            writer.writerow(row)

    print(f"Conversion complete: {csv_filename}")


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