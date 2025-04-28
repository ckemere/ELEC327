import os
import shutil
import re
import argparse
import pickle
import csv

def rename_files_to_directories(assignment_path, parsed_data_path=None, normalize_filename=False):
    assignment_data = {}
    pattern = re.compile(r'(?P<student_name>.+?)_(?P<student_number>\d+)_(?P<special_number>\d+)_(?P<original_filename>.+)\.(?P<extension>\w+)(?:-(?P<version>\d+))?$')
    
    for filename in os.listdir(assignment_path):
        match = pattern.match(filename)
        if match:
            student_name = match.group('student_name')
            student_number = match.group('student_number')
            special_number = match.group('special_number')
            extension = match.group('extension')
            version = match.group('version')
            original_filename = match.group('original_filename')
            
            assignment_data[student_name] = {
                'student number': student_number,
                'special number': special_number,
                'original filename': filename,
                'real filename': original_filename,
                'extension': extension,
                'version': version
            }
            
            if parsed_data_path:
                print('Copying {}'.format(student_name))
                student_dir = os.path.join(parsed_data_path, student_name)
                os.makedirs(student_dir, exist_ok=True)
                if normalize_filename:
                    shutil.copy(os.path.join(assignment_path, filename), os.path.join(student_dir, original_filename + '.' + extension))                    
                else:
                    shutil.copy(os.path.join(assignment_path, filename), os.path.join(student_dir, filename))
    
    return assignment_data


# This assumes a Canvas-formatted gradebook!!!
def read_csv_to_dict(file_path):
    with open(file_path, mode='r', encoding='utf-8') as file:
        reader = csv.reader(file)
        
        # Read the headers (first row)
        headers = next(reader)
        
        # Skip the second header row
        next(reader)  

        # Find the index of 'SIS Login ID'
        try:
            sis_index = headers.index("ID")
        except ValueError:
            raise ValueError("Column 'ID' not found in headers")

        # Construct the dictionary
        data_dict = {}
        for row in reader:
            if len(row) < len(headers):  # Ensure row isn't shorter than headers
                continue  

            main_key = row[sis_index]  # Use SIS Login ID as the main dictionary key
            if main_key:  # Ignore empty keys
                data_dict[main_key] = {
                    headers[i]: row[i] for i in range(len(row)) if i != sis_index  # Exclude SIS Login ID from subkeys
                }
    
    return data_dict



def main():
    parser = argparse.ArgumentParser(description="Process assignment submissions.")
    parser.add_argument("assignment_dir", help="Path to the assignment directory")
    parser.add_argument("-o", "--post_process_dir", help="Optional directory for post-processing", default=None)
    parser.add_argument("-n", "--normalize", action="store_true", help="Optionally normalize filenames by stripping Canvas stuff", default=False)
    parser.add_argument("-g", "--netid_from_gradebook", help="Optional gradebook file for getting net-ids", default=None)
    args = parser.parse_args()
    
    if args.netid_from_gradebook:
        # check wether file specified in args.netid_from_gradebook exists
        if not os.path.exists(args.netid_from_gradebook):
            raise ValueError(f"Gradebook file '{args.netid_from_gradebook}' does not exist")
        else:
            print(f'Using {args.netid_from_gradebook} for netids.')
    

    data = rename_files_to_directories(args.assignment_dir, 
                                       parsed_data_path=args.post_process_dir, 
                                       normalize_filename=args.normalize)
    

    if args.netid_from_gradebook:
        gradebook = read_csv_to_dict(args.netid_from_gradebook)
        print(gradebook)
        # Loop through assignments and match a netid to each student
        for student_key, assignment_data in data.items():
            if 'student number' in assignment_data:
                student_number = assignment_data['student number']
                # Look up the student in the gradebook using student_number as key
                if student_number in gradebook:
                    # Add the SIS Login ID (netid) to the assignment data
                    assignment_data['netid'] = gradebook[student_number].get('SIS Login ID', '')
                    print(student_number, assignment_data['netid'])
                else:
                    print(f"Warning: Student number for {student_key} ({student_number}) not found in gradebook")


    if args.post_process_dir:
        pickle_file = os.path.join(args.post_process_dir, 'assignment_data.pkl')
    else:
        pickle_file = os.path.join(args.assignment_dir, 'assignment_data.pkl')
    with open(pickle_file, 'wb') as f:
        pickle.dump(data, f)
    print("Processing complete. Data saved.")

    print(data)

if __name__ == "__main__":
    main()


