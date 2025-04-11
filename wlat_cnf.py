####  CNF generated for w-DDT ####
#### CNF generated for Differential Characteristics in SOP form #### 

import numpy as np
import math
import os
import subprocess

def calculate_lat(sbox):
    """Compute the linear approximation table for a given S-box."""
    size = len(sbox)
    n = size.bit_length() - 1  # Determine number of bits (4 for 16-element S-box)
    lat = np.zeros((size, size), dtype=int)

    for input_mask in range(size):
        for output_mask in range(size):
            count = 0
            for input_val in range(size):
                output_val = sbox[input_val]
                # Compute dot product of input with input_mask
                input_dot = bin(input_val & input_mask).count('1') % 2
                # Compute dot product of output with output_mask
                output_dot = bin(output_val & output_mask).count('1') % 2
                # Check if they're equal
                if input_dot == output_dot:
                    count += 1
            # Store bias (count - size/2)
            lat[input_mask][output_mask] = count - size//2
    return lat

def generate_cnf(lat, n, sbox, filename):
    # Open the file in write mode
    with open(filename, 'w') as file:
        for i in range(sbox):
            for j in range(sbox):
                
                if lat[i][j] != 0:
                    # Get binary representation of i and j with n bits
                    xin = format(i, f'0{n}b')
                    xout = format(j, f'0{n}b')  # Use j for xout, not i
                    b = lat[i][j]
                    w = format(b, f'04b')
                    
                    # Generate the full CNF clause string
                    clause = xin + xout  # Concatenate xin and xout
                    
                    if lat[i][j] == 8:
                        clause += '00'

                    if int(w, 2) == 4 or int(w, 2) == -4:
                        clause += '10'
                        
                    if int(w, 2) == 2 or int(w, 2) == -2:
                        clause += '11'
       
                    # Add '1' at the end of the clause
                    clause += ' 1'
                    
                    # Write the clause to the file in one go (no spaces in between)
                    #print(len(clause))
                    #if len(clause) > 10:
                    print(clause, file=file)


def count_lines_in_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()
    return len(lines)

def insert_string_as_first_line(filename, string_to_insert):
    # Step 1: Read the existing content of the file
    with open(filename, 'r') as file:
        content = file.read()

    # Step 2: Insert the new string as the first line
    new_content = string_to_insert + '\n' + content
    
    # Step 3: Write the updated content back to the file
    with open(filename, 'w') as file:
        file.write(new_content)

def append_to_file(filename):
    # Open the file in append mode ('a') to add text at the end
    with open(filename, 'a') as file:
        file.write('\n.e')


def run_espresso_with_output():
    # Get the current working directory
    current_directory = os.getcwd()
    
    # The command to be executed (without shell redirection)
    command = ['espresso', '-epos', 'SOP.pla']
    
    try:
        # Open the output file 'wDDT.cnf' in the current directory for writing
        output_file_path = os.path.join(current_directory, 'POS.pla')
        
        #POS to SOP conversion
        with open(output_file_path, 'w') as output_file:
            # Run the command in the current working directory and redirect stdout to the output file
            result = subprocess.run(command, stdout=output_file, stderr=subprocess.PIPE, text=True, cwd=current_directory)  
        
        # Check if the command was successful
        if result.returncode == 0:
            print(f"Command executed successfully, output written to {output_file_path}.")
        else:
            print(f"Command failed with return code {result.returncode}")
            print("Error:\n", result.stderr)  # Print any errors
        
    except FileNotFoundError:
        print("Error: 'espresso' command not found. Ensure espresso is installed and available in your system's PATH.")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")


def convert_string_to_cnf(input_string):
    # Remove spaces
    input_string = input_string.replace(" ", "")

    result = []

    # Iterate over each character in the string
    for index, char in enumerate(input_string, start=1):
        if char == '-':
            continue  # Skip dashes
        elif char == '0':
            result.append(f'{index}')  # Replace '0' with negative position
        elif char == '1' and index != len(input_string):
            result.append(f'-{index}')  # Replace '1' with positive position
        elif char == '1' and index == len(input_string):
            result.append('0')  # Replace the last '1' with '0'

    # Join the list into a space-separated string
    return ' '.join(result)

def process_file(input_filename, output_filename):
    # Get the current working directory
    current_dir = os.getcwd()

    # Create the full paths to the input and output files
    input_filepath = os.path.join(current_dir, input_filename)
    output_filepath = os.path.join(current_dir, output_filename)

    # Open the input file and read all lines
    try:
        with open(input_filepath, 'r') as file:
            lines = file.readlines()  # Read all lines into a list
    except FileNotFoundError:
        print(f"Error: The file '{input_filepath}' was not found.")
        return

    # Ensure there are enough lines to start reading from the 5th line
    if len(lines) < 5:
        print("Error: The file has fewer than 5 lines.")
        return

    # Start reading from the 5th line onward (index 4)
    cnf_results = []
    for line in lines[4:]:  # This will iterate from the 5th line onward
        input_string = line.strip()  # Remove extra spaces or newlines
        if input_string:  # Process only if the line is not empty
            cnf_string = convert_string_to_cnf(input_string)
            cnf_results.append(cnf_string)
    
    # Write all CNF results to the output file
    with open(output_filepath, 'w') as file:
        for cnf_string in cnf_results:
            file.write(cnf_string + '\n')

    print(f"Processed file saved as '{output_filepath}'.")

    

#GIFT Sbox
sbox = [1, 10, 4, 12, 6, 15, 3, 9, 2, 13, 11, 7, 5, 0, 8, 14];

# Calculate the DDT for the AES S-box
lat = calculate_lat(sbox)

# Print the LAT
print("Linear Approximation Table (LAT):")
print(lat)
print()


print("Sum of Product form is generated for given LAT and written to file : wLAT.pla ")
print()
var = math.ceil(math.log2(len(sbox)))
#print(var)
generate_cnf(lat, var, len(sbox), "SOP.pla")
clauses = count_lines_in_file("SOP.pla")

string_to_insert = '.i '+ '10' + '\n' + '.o 1'  
insert_string_as_first_line("SOP.pla", string_to_insert)
append_to_file("SOP.pla")

#Run espresso to minimize wLAT.pla
run_espresso_with_output()
#process_file("wLAT.cnf", "output.cnf");

#Convert POS.pla file into standard cryptominisat format
process_file("POS.pla", "output.cnf")
