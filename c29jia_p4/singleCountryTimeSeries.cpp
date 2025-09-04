#include "singleCountryTimeSeries.h"

SingleCountryTimeSeries::SingleCountryTimeSeries(){
    //instantiate as empty strings to check if a previous country was loaded before
    countryName = "";
    countryCode = "";
    // arr = *(new ResizingArray<TimeSeries*>);
}
SingleCountryTimeSeries::~SingleCountryTimeSeries(){
    //delete all values in the array which should call all the time series' destructors
    for (int i = 0; i < arr.getNumValidValues(); i++){
        TimeSeries* temp = arr[i];
        delete arr[i];
        temp = nullptr;
    }
}
//listens to user input and runs the corresponding functions
/*
void SingleCountryTimeSeries::runCountryCommands(){
    std::string command = "";

    while (command != "EXIT"){
        std::cin >> command;
        if (command == "LOAD"){
            std::string country;
            std::cin >> country;

            loadCountrySeries(country);
        } else if (command == "LIST") {
            list();
        } else if (command == "ADD"){
            std::string seriesCode;
            int addYear;
            double addData;

            std::cin >> seriesCode;
            std::cin >> addYear;
            std::cin >> addData;

            add(seriesCode, addYear, addData);
        } else if (command == "UPDATE"){
            std::string seriesCode;
            int updateYear;
            double updateData;

            std::cin >> seriesCode;
            std::cin >> updateYear;
            std::cin >> updateData;
            update(seriesCode, updateYear, updateData);
        } else if (command == "PRINT"){
            std::string seriesCode;
            std::cin >> seriesCode;

            print(seriesCode);
        } else if (command == "DELETE"){
            std::string seriesCode;

            std::cin >> seriesCode;
            deleteSeries(seriesCode);
        } else if (command == "BIGGEST"){
            biggestMean();
        } else {
            continue;
        }
    }

    return;
}
*/

//loads all time series for a given country name
void SingleCountryTimeSeries::loadCountrySeries(std::string country, std::ifstream& MyReadFile){
    // std::cout << "inside the inner load :D" << std::endl;
    //if there is already a country name, empty the array of time series and reset name and code to empty string
    if (countryName != ""){
        //empties a array of pointers by deallocating each array value individually
        arr.emptyPointerArray();
        
        countryName = "";
        countryCode = "";
    }

    //std::string filename = "lab2_multidata.csv";    //store the filename in a string for easier manipulation later on
    // std::ifstream MyReadFile(filename);
    std::string line;
    std::string tempLine;
    bool noCountryCode = true;

    //read through the file line by line until the end
    while (getline(MyReadFile, line)) { 
        std::stringstream str(line); //convert to a stringstream to use in getline
        getline(str, tempLine, ','); //get first value in the line (country name)
        if (tempLine == country){    //if it's the target country
            TimeSeries* series = new TimeSeries();                 //instantiate a new time series
            (*series).load(line);             //load all data in the line to the new time series
            arr.appendVal(series);             //add it to the resizing array of time series
            arr.incrementNumValidValues();     //increase the number of valid values, since all objects as valid

            //checks if the country code has a value or not. If not, fill it
            if (noCountryCode){
                getline(str, tempLine, ','); //get second value in the line
                countryCode = tempLine;      //set country code
                countryName = country;       //set country name
                noCountryCode = false;
            }
        }
    }


    // MyReadFile.close();

    //print success for a successful load
    // std::cout << "success" << std::endl;
}

//prints the country name and code, then prints all the series names in the array of time series
void SingleCountryTimeSeries::list(){
    std::cout << countryName;           //print country name
    std::cout << " " << countryCode;    //print country code
    for (int i = 0; i < arr.getNumValues(); i++){   //iterate through the array and print a space then the series name
        std::cout << " " << arr[i] -> getSeriesName();
    }

    std::cout << std::endl;
}

//adds a data point to a specific timeseries with the corresponding series code
void SingleCountryTimeSeries::add(std::string seriesCode, int year, int data){
    //boolean to check if the year was found
    bool codeFound = false;
    for (int i = 0; i < arr.getNumValues(); i++){
        if (arr[i] -> getSeriesCode() == seriesCode){  //if the series code is valid
            //code is found
            codeFound = true;
            //call timeSeries' update function, outputs success or failure
            arr[i] -> add(year, data);
            break;
        }
    }

    //if code not found, time series does not exist, output failure
    if (!codeFound){
        std::cout << "failure" << std::endl;
    }
    return;
}

//updates a data point of a specific timeseries with the corresponding series code
void SingleCountryTimeSeries::update(std::string seriesCode, int year, int data){
    //boolean to check if the year was found
    bool codeFound = false;
    for (int i = 0; i < arr.getNumValues(); i++){
        if (arr[i] -> getSeriesCode() == seriesCode){  //if the series code is valid
            //code is found
            codeFound = true;
            //call timeSeries' update function, outputs success or failure
            arr[i] -> update(year, data);
            break;
        }
    }
    //if code not found, time series does not exist, output failure
    if (!codeFound){
        std::cout << "failure" << std::endl;
    }
    return;
}

//prints the timeseries with the specific series code
void SingleCountryTimeSeries::print(std::string seriesCode){
    //boolean to check if the year was found
    bool codeFound = false;
    for (int i = 0; i < arr.getNumValues(); i++){
        if (arr[i] -> getSeriesCode() == seriesCode){  //if the series code is valid
            //code is found
            codeFound = true;
            //call timeSeries' print function
            arr[i] -> print();  
            break;
        }
    }
    //if code not found, time series does not exist, output failure
    if (!codeFound){
        std::cout << "failure" << std::endl;
    }
    return;
}

//deletes a value (timeseries) from the array of timeseries. It is assumed that the deleted time series will call it's own destructor
void SingleCountryTimeSeries::deleteSeries(std::string seriesCode){
    for (int i = 0; i < arr.getNumValues(); i++){
        if (arr[i] -> getSeriesCode() == seriesCode){  //if the series code is valid
            arr.deleteVal(i);   //deletes the time series at index i
            arr.decrementNumValidValues();  //decrement the number of valid values
            std::cout << "success" << std::endl;

            //return immediately to avoid the failure print after the loop
            return;              
        }
    }

    //if you're out of the loop, then the series code is invalid
    std::cout << "failure" << std::endl;
}

//returns the seriesCode of the timeSeries with the biggest mean
double SingleCountryTimeSeries::biggestMean(){
    //Create variables to store the series code of the current max mean, the current max mean, and the current mean respectably
    std::string biggestSeriesCode;
    double biggestMean = -1.0;
    double curMean = -1.0;
    //iterate through the array of time series and calculate the mean for each time series
    for (int i = 0; i < arr.getNumValues(); i++){
        curMean = arr[i] -> mean();
        //if the mean of a later time series is greater than the current greatest mean, set greatest mean to the current mean
        if (curMean > biggestMean){
            biggestMean = curMean;
            biggestSeriesCode = arr[i] -> getSeriesCode();
        }
    }

    //if the biggest mean is still less than 0, then there was no valid data so output failure and return
    if (biggestMean < 0){
        std::cout << "failure" << std::endl;
        return -1.0;
    }

    //print the biggestSeriesCode since it's guaranteed to exist
    std::cout << biggestSeriesCode << std::endl;
    //in case you need the biggest mean for something else in the future
    return biggestMean;
}

//getters
std::string SingleCountryTimeSeries::getCountryName(){
    return countryName;
}
std::string SingleCountryTimeSeries::getCountryCode(){
    return countryCode;
}
ResizingArray<TimeSeries*> SingleCountryTimeSeries::getArr(){
    return arr;
}

//setters
void SingleCountryTimeSeries::setCountryName(std::string name){
    countryName = name;
}
void SingleCountryTimeSeries::setCountryCode(std::string code){
    countryCode = code;
}

//find timeSeries with specific seriesCode
double SingleCountryTimeSeries::findSeriesMean(std::string seriesCode){
    for (int i = 0; i < arr.getNumValidValues(); i++){
        //when you find the series, return its mean (mean will be -1.0 if no valid data)
        if (arr[i] -> getSeriesCode() == seriesCode){
            return arr[i] -> mean();
        }
    }
    return -1.0;    //if series code not found, return -1.0
}

//checks if the timeSeries with the given seriesCode is in the countrySeries
bool SingleCountryTimeSeries::isSeriesCodePresent(std::string seriesCode){
    for (int i = 0; i < arr.getNumValidValues(); i++){
        //put the timeseries pointer into a variable for easier manipulation
        TimeSeries* temp = arr[i];
        //when you find the series, return its mean (mean will be -1.0 if no valid data)
        if (temp -> getSeriesCode() == seriesCode){
            return true;
        }
    }
    return false;
}