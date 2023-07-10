#!/bin/bash

# Function to find Makefiles recursively
find_makefiles() {
    local parent_dir="$1"

    # Find Makefiles recursively in the parent directory
    find "$parent_dir" -type f -name "Makefile" -execdir sh -c 'echo "Running make clean && make all in $PWD"; make clean && make all' \;
}

# Prompt for the parent directory
read -p "Enter the parent directory path: " parent_directory

# Check if the directory exists
if [ ! -d "$parent_directory" ]; then
    echo "Directory not found."
    exit 1
fi

# Find and run make commands if Makefiles are found
find_makefiles "$parent_directory"

