# 🥢ML Chopsticks🥢
Solution to gather user behavior information when using chopsticks and compile a set of usage statistics based on a ML generated model.

## Inspiration
Following a gimmick that popped up at the end of last [CWF](https://scottkwang.github.io/CodeWithFriends-Spring2020/) edition the project involves chopsticks. The pitch was to create a "5G enabled" chopstick but I then realized that it was a bit impractical (at least for now). Putting the 5G part of the idea aside, this project will enable the user, thanks to ML based models, to gather information about his/her usage behaviour and all sorts of statistics of use.

## General features

## Tech used
For the Tactigon One Board I've used the Arduino IDE to develop a basic BLE peripheral code. The main function gathers IMU data with a 50Hz timing and then puts it into a buffer that will be used to update the BLE characteristic. A simple "if-else" statement is used to check the Bluetooth connection so that if it drops or didn't happened at all the data won't be "pushed out", saving battery.

The RPi, once it's booted up, execute a python code the will search for the Tactigon and connect to it. It then subscribes to the BLE characteristic in notification and waits for new data pushed by the Tactigon One Board. It then feeds, in real-time, every data collected to a custom ML algorithm (scikit-learn library based) that will the following operations:

* Filtering: the raw data read on the characteristic is filtered using some data filtering code that will get rid of possible unwanted/corrupted data that is gathered by the Tactigon One Borad.
* Segmentation: the data comes through at a timing of 50Hz and that is a bit problematic... It has advantages, like more fine detection of movements but introduces a layer of complication for the recognition that later will be executed. This step ensure that a valid interval of data is feed into the recognition algorithm.
* Recognition and flagging: the data segment provided will pass through a custom ML code that will calculate the confidence value for a specific gesture from the pre-trained model that is specified into the code. Once a gesture is recognized it will be flagged by the algorithm, which will later record it in a log file for later use.

## Built with
* [scikit-learn](https://scikit-learn.org/stable/) - installed via pip
* [Python 3.7.7](https://www.python.org/downloads/release/python-377/) - Win64
* Arduino IDE
### Note

## Specifications
The project was developed and tested on the following hardware:
* [Raspberry Pi 3 Model B+](https://www.raspberrypi.org/products/raspberry-pi-3-model-b-plus/?resellerType=home)
* [Tactigon One Board](https://www.nextind.eu/it/product/the-tactigon-one-it/)
## Known issues

## To do

## Reference
The documentation about the Tactigon board can be found [here](https://github.com/TactigonTeam/Docs).

For Tactigon libraries exposed in the Arduino IDE you can look [here](https://www.thetactigon.com/arduino/doxygen/index.html).
