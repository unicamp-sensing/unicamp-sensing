#!/bin/bash

secs=0
limit=1
port="/dev/ttyUSB0"


# sudo apt update -y

# printf "\n\nGetting golang....\n\n"
# sudo apt install golang

# printf "\n\nGetting arduino-cli....\n\n"
# go get -u github.com/arduino/arduino-cli

# printf "\n\nReading test environment....\n\n"
# sudo cp ~/go/bin/arduino-cli /bin/
# arduino-cli core update-index
sudo stty -F $port cs8 115200

printf "\n\nRunning all tests (PORT: $port):\n\n"
for test in ./tests/* ; do
    printf "\n=======> $test\n"

    printf "\nCreating log...\n"
    rm -f ${test#*/*/}.log
    touch ${test#*/*/}.log

    printf "\nPreparing board...\n"
    # sudo arduino-cli compile -p $port --fqbn esp8266:esp8266:nodemcuv2 $test
    # sudo arduino-cli upload -p $port --fqbn esp8266:esp8266:nodemcuv2 $test
    
    printf "\nRunning ${test}...\n"
    start=$(date +%s)
    while [ $(($(date +%s) - start)) -le $limit ]; do  
        # Update and check log
        sudo cat $port >> ${test#*/*/}.log
        if [ ! -z "$(tail -1 ${test#*/*/}.log | grep -w "Pass")" ]; then    
            echo "PASSED!!!"
            break
        elif [ ! -z "$(tail -1 ${test#*/*/}.log | grep -w "Fail")" ]; then
            echo "FAILED!!!"
            break
        fi

    done


done