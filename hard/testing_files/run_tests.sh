#!/bin/bash
set -e
setup=false # Default considers the environment is ready
time=60 # Default time limit for each test
port=$1 # Default serial port connected to the board
# echo ${port::1}

# Checks which flags are set
while getopts st: option
do
case "${option}"
in
s) setup=true;;
t) time=${OPTARG};;
esac
done

# Must receive as the first parameter the path which contains the file to
# where the serial writes from the board are sent (EX: /dev/ttyUSB0)
if [ -z $port ] || [ $(echo $port | head -c 1) = "-" ]; then
    echo ""
    echo "MISSING ARGUMENT: The serial port connected to the board was not defined"
    echo "Try the following:"
    echo " - Run the 'arduino-cli board list' command"
    echo " - Find the row/entry with the desired board"
    echo " - Get the atribute/colunm 'Port' of such row/entry"
    echo " - Pass the atribute as the first argument of the script"
    echo ""
    exit 1
fi

# Theres a couple of things that must be done to prepare the
# testing environment:
# - Install golang and then the arduino-cli
# - Adding the board and libraries to the cli
# This needs to be done only once.
if [ "$setup" = true ]; then
    echo "=====> [ PREPARING ENVIRONMENT ] <====="
    
    printf "\n\nUpdating packages....\n\n"
    sudo apt update -y

    printf "\n\nGetting golang....\n\n"
    sudo apt install golang

    printf "\n\nGetting arduino-cli....\n\n"
    go get -u github.com/arduino/arduino-cli

    printf "\n\nReading test environment....\n\n"
    sudo cp ~/go/bin/arduino-cli /bin/
    arduino-cli core update-index

    echo "=====> [ READY ] <====="
fi


sudo stty -F $port cs8 115200

printf "\n\nRunning all tests (PORT: $port):\n\n"
for test in ./tests/* ; do
    printf "\n=======> $test\n"

    printf "\nCreating log...\n"
    rm -f logs/${test#*/*/}.log
    touch logs/${test#*/*/}.log

    printf "\nCompiling Test...\n"
    sudo arduino-cli compile -p $port --fqbn esp8266:esp8266:nodemcuv2 $test > /dev/null
    printf "\nLoading Board...\n"
    sudo arduino-cli upload -p $port --fqbn esp8266:esp8266:nodemcuv2 $test > /dev/null

    printf "\nRunning ${test}...\n"
    start=$(date +%s)
    while [ $(($(date +%s) - start)) -le $time ]; do  

        # Update log
        sudo cat $port >> logs/${test#*/*/}.log
        
        # Check for keyword
        if [ ! -z "$(tail -1 logs/${test#*/*/}.log | grep -w "Pass")" ]; then    
            echo "PASSED!!!"
            break
        elif [ ! -z "$(tail -1 logs/${test#*/*/}.log | grep -w "Fail")" ]; then
            echo "FAILED!!!"
            break
        fi

    done

    if [ $(($(date +%s) - start)) -le $time ]; then
        echo "TIMEOUT: The time limit of ${time}s was exceeded."
        echo " - Try incresing the time limit with the flag '-t'"
        echo " - Check if the test is serial printing the keywords (Pass/Fail)"
    fi

done