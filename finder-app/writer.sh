# If missing writefile
if [ $# -lt 1 ]
then
    echo "Error: Missing writefile argument!"
    exit 1
fi

# If missing writestr
if [ $# -lt 2 ]
then
    echo "Error: Missing writestr argument!"
    exit 1
fi

# If path is a directory
if [ -d $1 ]
then
    echo "Error: $1 is a directory path!"
    exit 1
fi

# If the directory portion of the path does not exist create it
directory_path=$(dirname $1)
if [ ! -d $directory_path ]
then
    mkdir -p "$directory_path"
fi

# If file already exist delete it
if [ -e $1 ]
then
    rm $1
fi

# Create and write to file
echo $2 > $1

# Verify the directory and file now exist
if [ ! -e $1 -o ! -d $directory_path ]
then
    echo "Error: Could not create file!"
    exit 1
fi