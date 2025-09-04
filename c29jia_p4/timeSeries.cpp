/*
Sources:
    - https://www.geeksforgeeks.org/cpp-program-for-string-to-double-conversion/
*/
#include "timeSeries.h"

//constructor
TimeSeries::TimeSeries() {
    m = 0;
    b = 0;
    // yearList = *(new ResizingArray<int>);
    // dataList = *(new ResizingArray<double>);
}

//destructor
TimeSeries::~TimeSeries() {
    seriesName = "";
    seriesCode = "";
    // delete[] yearList.getArray();
    // delete[] dataList.getArray();
}

//listens to user input and runs the corresponding functions
void TimeSeries::runCommands(){
    std::string command = "";

    while (command != "EXIT"){
        std::cin >> command;
        if (command == "LOAD"){
            std::string fileName;

            std::cin >> fileName;

            load(fileName);
        } else if (command == "PRINT"){
            print();
        } else if (command == "ADD"){
            int addYear;
            double addData;

            std::cin >> addYear;
            std::cin >> addData;

            add(addYear, addData);
        } else if (command == "UPDATE"){
            int updateYear;
            double updateData;

            std::cin >> updateYear;
            std::cin >> updateData;
            update(updateYear, updateData);
        } else if (command == "MEAN"){
            mean();
        } else if (command == "MONOTONIC"){
            is_monotonic();
        } else if (command == "FIT"){
            best_fit(m, b);
        } else {
            continue;
        }
    }

    return;
}

//loads a times series from the given file and initializes all the private variables
void TimeSeries::load(std::string newLine){
    //clear yearList and dataList in case this is called anytime other than the start of the program
    yearList.emptyArray();
    dataList.emptyArray();

    //load the file and declare the variables necessary to read out its values
    std::string tempLine;

    //get the single country time series
    std::stringstream newStr(newLine);

    //declare a counter to control which data gets inputted into the arrays
    int count = 0;

    //data of all time series starts from 1960
    int curYear = 1960;

    //initializes the seriesName and seriesCode, while entering all the years and data into yearList and dataList
    while ( getline(newStr, tempLine, ',') ) {
        //series name is always the 3rd entry in a line
        if (count == 2){
            seriesName = tempLine;
        } 
        //series code is always the 4th entry in a line
        if (count == 3){
            seriesCode = tempLine;
        }

        //first 4 dates in the csv file are not data, so start appending data from the 5th entry forward
        if (count > 3){
            //append a year-data pair
            yearList.appendVal(curYear);
            dataList.appendVal( stod(tempLine) );
            //increment the year
            curYear++;
        }

        //increment the counters
        count++;
    }
}

//prints the time series in (year,data) pairs, skipping over years with invalid data
void TimeSeries::print(){
    //if the year list is empty, then there is no valid data to be printed, output failure immediately
    if (yearList.isEmpty()){
        std::cout << "failure" << std::endl;
        return;
    }

    //create a variable to store the current index to traverse the year and data arrays
    int index = 0;
    //create a boolean to check for the case when all the years have invalid data
    bool isValidData = false;
    //iterate over the entire time of the time series
    while (index < yearList.getNumValues()){
        //assume all data is positive values, output valid data
        if (dataList[index] >= 0){
            //if we enter this if block then we must've had valid data, so isValidData is now true
            isValidData = true;
            std::cout << "(" << yearList[index] << "," << dataList[index] << ") ";
        }
        //increment index
        index++;
    }

    //if there was no valid data, output failure
    if (!isValidData){
        std::cout << "failure" << std::endl;
    } else {
        //output a new line
        std::cout << std::endl;
    }

    return;
}

//adds data into a given year if the data for that year was originally invalid
void TimeSeries::add(int year, int data){
    //if year is less than any existing year, insert year-data pair to the beginning of the time series
    //else if year is greater than any existing year, append year-data pair to the end of the time series
    if (year < yearList[0]){
        //if data is valid increment numValidValues
        if (isValid(data)){
            yearList.incrementNumValidValues();
            dataList.incrementNumValidValues();
        }
        yearList.insertVal(0, year);
        dataList.insertVal(0, data);
        std::cout << "success" << std::endl;
        return;
    } else if (year > yearList[yearList.getNumValues() - 1]){
        //if data is valid increment numValidValues
        if (isValid(data)){
            yearList.incrementNumValidValues();
            dataList.incrementNumValidValues();
        }
        yearList.appendVal(year);
        dataList.appendVal(data);
        std::cout << "success" << std::endl;
        return;
    } else {
        //turn the years in array indexes
        // int startYear = yearList[0];
        // int indexedYear = year - startYear;

        //find the year in the yearList
        for (int i = 0; i < yearList.getNumValues(); i++){
            if (yearList[i] == year){
                //if the value at the given year is invalid, add the new data
                if (dataList[i] < 0){
                    //uses the get method because the operator overloaded [] returns a value, which can't be assigned to
                    dataList.getArray()[i] = data;
                    //if data is valid increment numValidValues
                    if (isValid(data)){
                        yearList.incrementNumValidValues();
                        dataList.incrementNumValidValues();
                    }
                    std::cout << "success" << std::endl;
                } else {
                    std::cout << "failure" << std::endl;
                }
                //if you found the year in the yearList, return
                return;
            }
        }

        //if the year was not in yearList and is between the smallest and largest years, insert is ascending order
        for (int i = 0; i < yearList.getNumValues(); i++){
            //if year is smaller than the year at index i, insert it there and return
            if (year < yearList[i]){
                yearList.insertVal(i, year);
                dataList.insertVal(i, data);
                //if data is valid increment numValidValues
                if (isValid(data)){
                    yearList.incrementNumValidValues();
                    dataList.incrementNumValidValues();
                }
                std::cout << "success" << std::endl;
                return;
            }
        }
        
    }

    //if it reaches the end of the file then the value was not added to default failure
    std::cout << "failure" << std::endl;
    return;
}

//updates data for the given year if the year is already in the time series
bool TimeSeries::update(int year, int data){
    //iterate through the yearlist to check if the year has existing valid data
    for (int i = 0; i < yearList.getNumValues(); i++){
        //if yes, year is in yearlist
        if (yearList[i] == year) {
            //check if the data is valid, if yes update it with data and return success
            if (dataList[i] >= 0){
                dataList.getArray()[i] = data;
                std::cout << "success" << std::endl;
                return true;
            } else {
                std::cout << "failure" << std::endl;
                return false;
            }
        }
    }

    //if you exit the loop, it means either the year wasn't found or it didn't contain valid data
    std::cout << "failure" << std::endl;
    return false;
}

//computes the mean of all valid data in the time series
double TimeSeries::mean(){
    //check if the year list is empty, return failure if so since no valid data is present
    if (yearList.isEmpty()){
        // std::cout << "failure" << std::endl;
        return -1.0;
    }

    //create a variable to store the sum of the data values in the time series
    double sum = 0;
    //create a variable to store the number of valid data points
    int numValid = 0;
    //iterate through the data list and add the values to sum if it is valid (>= 0)
    for (int i = 0; i < dataList.getNumValues(); i++){
        if (dataList[i] >= 0){
            sum += dataList[i];
            numValid++;
        }
    }

    //if sum is less than or equal to 0, then it means that all data in the time series is invalid, so output failure
    if (sum <= 0.0){
        // std::cout << "failure" << std::endl;
        return -1.0;
    }

    //find the mean by dividing sum by the total number of values
    //dataList.getNumValues() cannot be zero because if would never pass the first conditional at the start of the function
    double mean = sum/numValid;

    //output
    // std::cout << "mean is " << mean << std::endl;

    //return the mean for later use
    return mean;
}

//checks if a time series is monotonic (strictly increasing or decreasing) or not
bool TimeSeries::is_monotonic() {
    // Special cases for when there are 0, 1, or 2 values in the data list
    if (dataList.isEmptyValid()) {
        std::cout << "failure" << std::endl;
        return false;
    }

    // If less than 3 and greater than 0 valid data then it must be monotonic
    if (dataList.getNumValidValues() < 3) {
        std::cout << "series is monotonic" << std::endl;
        return true;
    }

    // Check if it's a potential increasing monotonic or decreasing monotonic
    bool isIncreasing = false;
    double firstVal = -1.0;
    double secondVal = -1.0;
    int numValid = 0;

    // Finds the first two valid data points
    for (int i = 0; i < dataList.getNumValues(); i++) {
        if (dataList[i] >= 0) { // Check for valid value
            if (numValid == 0) {
                firstVal = dataList[i];
                numValid++;
            } else if (numValid == 1) {
                secondVal = dataList[i];
                numValid++;
                break; // We only need the first two valid values
            }
        }
    }

    // Determine if the series is increasing or decreasing
    if (firstVal < secondVal) {
        isIncreasing = true;
    } else if (firstVal > secondVal) {
        isIncreasing = false;
    } else {
        // If firstVal == secondVal, we need to check further
        int validIndex = 0;
        double thirdVal = -1.0;
        bool isAllEqual = true;
        while (validIndex < dataList.getNumValues()){
            //if the data point is valid data and is not equal to either of the two earliest valid values in the array
            if (dataList[validIndex] >= 0 && dataList[validIndex] != secondVal){
                //set thirdVal to the different valid value and set isIncreasing accordingly
                thirdVal = dataList[validIndex];
                //not all the valid values in the data list are equal anymore
                isAllEqual = false;
                //if secondVal is less than thirdVal, then increasing
                if (secondVal < thirdVal) {
                    isIncreasing = true;
                } else {
                    //otherwise secondVal must be greater than thirdVal, so decreasing
                    isIncreasing = false;
                }
            }
            validIndex++;
        }

        //if you exit the loop without finding a valid data point that is not equal to firstVal and secondVal, then all valid
        //values in the data list are equal, and is thus monotonic
        if (isAllEqual){
            std::cout << "series is monotonic" << std::endl;
            return true;
        }
    }

    // Set validVal to the second valid value
    double validVal = firstVal;

    // Iterate through the array, ignoring invalid values
    for (int i = 0; i < dataList.getNumValues(); i++) {
        if (dataList[i] >= 0) { // Check for valid value
            if (isIncreasing) {
                if (validVal > dataList[i]) {
                    std::cout << "series is not monotonic" << std::endl;
                    return false;
                }
            } else {
                if (validVal < dataList[i]) {
                    std::cout << "series is not monotonic" << std::endl;
                    return false;
                }
            }
            // Update validVal to the new valid value
            validVal = dataList[i];
        }
    }

    // If the array was not proven to be non-monotonic, then it must be monotonic
    std::cout << "series is monotonic" << std::endl;
    return true;
}

//prints the slope and y-intercept of the line of best fit of the time series
void TimeSeries::best_fit(double &m, double &b){
    //if empty list, no valid data
    if (yearList.isEmpty()){
        std::cout << "failure" << std::endl;
        return;
    }

    //xi, yi, xi^2, xiyi, and N in that order
    int sumYears = 0;
    double sumData = 0;
    int sumSquaredYears = 0;
    double sumYearDataProduct = 0;
    int numValidPairs = 0;

    //if one one valid value, increment everything once
    if (yearList.getNumValues() == 1){
        sumYears += yearList[0];
        sumData += dataList[0];
        sumSquaredYears += yearList[0]*yearList[0];
        sumYearDataProduct += yearList[0]*dataList[0];
        numValidPairs++;
    }

    //iterate through the yearList. When you encounter valid data, update the different mathematical variables accordingly
    for (int i = 0; i < yearList.getNumValues(); i++){
        if (dataList[i] >= 0){
            sumYears += yearList[i];
            sumData += dataList[i];
            sumSquaredYears += yearList[i]*yearList[i];
            sumYearDataProduct += yearList[i]*dataList[i];
            numValidPairs++;
        }
    }

    //checks if there was any valid data, outputs failure if theer was no valid data and exits the function
    if (numValidPairs <= 0){
        std::cout << "failure" << std::endl;
        return;
    }

    //finds the slope m
    double mNumerator = (numValidPairs*sumYearDataProduct) - (sumYears*sumData);
    double mDenominator = numValidPairs*sumSquaredYears - sumYears*sumYears;
    //in the case something goes very wrong and the denominator becomes zero
    //conditional should normally never be true
    // if (mDenominator == 0.0){
    //     std::cout << "failure" << std::endl;
    //     return;
    // } 
    double mSlope = mNumerator/mDenominator;

    //finds the y-intercept
    double bNumerator = sumData - mSlope*sumYears;
    double bDenominator = numValidPairs;
    //again, in the case something goes very wrong and the denominator becomes zero
    //actually already included in the numValidPairs check above, but kept in case something very wrong happens
    // if (bDenominator == 0){
    //     std::cout << "failure" << std::endl;
    //     return;
    // }
    double bIntercept = bNumerator/bDenominator;

    std::cout << "slope is " << mSlope << " intercept is " << bIntercept << std::endl;

    m = mSlope;
    b = bIntercept;

    return;
}

//returns if double data is valid (non-negative)
bool TimeSeries::isValid(double val){
    if (val >= 0){
        return true;
    }
    return false;
}

//getters
std::string TimeSeries::getSeriesName(){
    return seriesName;
}

std::string TimeSeries::getSeriesCode(){
    return seriesCode;
}

ResizingArray<int> TimeSeries::getYearList(){
    return yearList;
}

ResizingArray<double> TimeSeries::getDataList(){
    return dataList;
}

double TimeSeries::getM(){
    return m;
}

double TimeSeries::getB(){
    return b;
}

//print the private member lists to console
void TimeSeries::printYearList(){
    return yearList.print();
}

void TimeSeries::printDataList(){
    return dataList.print();
}



