# Testing Grounds

## Purpose
The `testing_grounds/` folder consists of automated hardware related tests. It's intended for checking the board's software and sensors.  Some of the current tests involves sensors functionality & integration, handcrafted libraries correcteness and memory allocation. The environment was built using the 'arduino-cli' software and aims to be as modular and automated as possible, ensuring the ease of adding/removing and executing tests without the need of an IDE.

## Structure
This section revolves around the finality of each folder and file within the **Testing Grounds**.

### The **run_tests** script
This is the testing enviroment's core. It will automate the compilation and loading of the testing sketches contained in `\tests`. The script can also be used for setting up the environment and installing libraries listed in `libraries.txt`. The script must have execution permition to be used.

- How to execute:
    - Give execution permition to the script with `sudo chmod +x run_tests`
    - Prepare the testing environment with `sudo ./path-to-folder\run_tests -s`
    - With the environmente set, Execute the script with `./path-to-folder\run_tests`

Format: `run_tests {-s | serial-port} [options]`
- Flag `-s`:
    - Prepares the evironment for the script (downloads required software)
- serial-port:
    - Is the path of the serial port I/O file (EX: `/dev/ttyUSB0`)
- Flags:
    - `-s` : Will run the enviroment setup routine. Only needs to be used once
    - `-t` : Defines the maximum amount of seconds each test has to complete
    - `-l` : Install all libraies contained in the `libraries.txt` file
    - `-f` : Receives a list of tests names to be run (if not set, run all tests)
    - `-e` : Terminates script if any test errors or fails.

### The **tests** folder
This folder will contain the `.ino` testing sketches which will be compiled and loaded onto the board. The sketches do most of the work testing the board's components. To comunicate with the testing script, every test has it's own log. To aprove or fail a test, simply print one of the keywords `Pass` or `Fail` onto the serial port during the sketch's execution.
Note that, uppon printing one of the keywords, the script will stop logging the test run the remainning tests.
- To add/remove tests, simply add/remove a sketch from `/tests`.
- Ensure that the test will print `Pass` or `Fail` on the serial port at some point.
- If no keywords are printed in the serial, the script will timeout and consider it a `Fail`.

### The **libraries.txt** file
To compile some tests, libraries may be needed. *If the library is available* for the arduino-cli, the script will take care of installing any libraries which are placed within this file.
- Adding **suported libraries** to the environment:
    - Check availability with `arduino-cli lib search library-name`.
    - If found, add the line `library-name@version` to *libraries.txt*.
    - Run `run_tests` with the `-l` flag set to install libraries.
- For **unsupported libraries**:
    - Search the library's repositrory and dowloando the `.zip` file of the desired version.
    - Unzip the file inside the `~/Arduino/libraries` folder.

### The **logs** folder
In order to comunicate with the script and debugging purposes, every test has it's serial port logged in a `.txt` file int `/logs` which has the same name of the `.ino` sketch which created it. These can be accessed for post analisys of the tests results. Keep in mind that every time a test is executed, it's old log is deleted. The logs can also be used as the test executes for debugging purposes.