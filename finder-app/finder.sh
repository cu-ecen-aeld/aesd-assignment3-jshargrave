# If missing filesdir
if [ $# -lt 1 ]
then
    echo "Error: Missing filesdir argument!"
    exit 1
fi

# If missing searchstr
if [ $# -lt 2 ]
then
    echo "Error: Missing searchstr argument!"
    exit 1
fi

# If filesdir is not directory
if [ ! -d $1 ]
then
    echo "Error: '$1' is not a valid directory!"
    exit 1
fi

# Get the number of files in directory and number of matches
number_of_files=$(find -P $1 -type f | wc -l)
matching_lines=$(grep -r -o $2 $1 | wc -l)

# Print results
echo "The number of files are $number_of_files and the number of matching lines are $matching_lines"