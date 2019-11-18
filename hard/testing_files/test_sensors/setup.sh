sudo apt update -y

printf "\n\nGetting golang....\n\n"
sudo apt install golang

printf "\n\nGetting arduino-cli....\n\n"
go get -u github.com/arduino/arduino-cli

printf "\n\nReading test environment....\n\n"
sudo cp ~/go/bin/arduino-cli /bin/
arduino-cli core update-index