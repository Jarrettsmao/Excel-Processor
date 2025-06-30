import openpyxl

file_path = r"C:\Users\admin\Downloads\Historicalinvesttemp.xlsx"
file_path = file_path.replace("\\", "/")
print(file_path)

try:
    wb = openpyxl.load_workbook(file_path)
    print(f"Workbook loaded successfully: {file_path}")
    # Print the sheet names
    print("Sheet names:", wb.sheetnames)
except Exception as e:
    print(f"Error during workbook loading: {e}")
