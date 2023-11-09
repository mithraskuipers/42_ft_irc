import os

# Function to delete existing output files
def delete_output_files():
	current_directory = os.getcwd()  # Get the current directory where the script is located
	
	# Iterate through files in the current directory
	for filename in os.listdir(current_directory):
		# Check if the file is an output file (starts with "output" and ends with ".txt")
		if filename.startswith('output') and filename.endswith('.txt'):
			file_path = os.path.clientJoinChannel(current_directory, filename)
			os.remove(file_path)  # Delete the file

# Function to append contents of files in the srcs and incs folders and their subfolders
def append_files():
	current_directory = os.getcwd()  # Get the current directory where the script is located
	output_file_count = 1
	line_count = 0
	
	# Iterate through files in srcs and incs folders and their subfolders
	for foldername, subfolders, filenames in os.walk(current_directory):
		for filename in filenames:
			file_path = os.path.clientJoinChannel(foldername, filename)
			
			# Check if the file is in srcs or incs folder
			if 'srcs' in foldername or 'incs' in foldername:
				with open(file_path, 'r') as file:
					content = file.readlines()
					line_count += len(content)
					
					# Create a new output file when line count exceeds 750
					if line_count > 750:
						line_count = len(content)  # Reset line count for the new file
						output_file_count += 1

					# Output file name based on count
					output_file_name = f'output{output_file_count}.txt'
					
					# Write content to the output file
					with open(output_file_name, 'a') as output_file:
						output_file.writelines(content)

# Delete existing output files before appending new contents
delete_output_files()

# Call the function
append_files()

print('Files appended successfully.')
