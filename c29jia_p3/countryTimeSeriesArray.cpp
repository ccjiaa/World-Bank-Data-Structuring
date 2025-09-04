#include <string>
#include "bTree.h"
// #include "singleCountryTimeSeries.cpp"
#include <cassert>

class CountryTimeSeriesArray {
    private:
        SingleCountryTimeSeries* countryArray[512];
        int numCountries;
        BTree* tree;
        //do we need an array of trees, one for each series code??

    public:
    //constructor, calls load
        CountryTimeSeriesArray(){
            numCountries = 0;
            tree = nullptr;
        }

        ~CountryTimeSeriesArray(){
            //call delete and hence the destructor on each country series object
            for (int i = 0; i < numCountries; i++){
                delete countryArray[i];
                countryArray[i] = nullptr;
            }
            delete tree;
            tree = nullptr;
        }
        //listens to user input and runs the corresponding functions
        void runCountryCommands(){
            std::string command = "";

            while (command != "EXIT"){
                std::cin >> command;
                if (command == "LOAD"){
                    std::string filename;
                    std::cin >> filename;

                    loadAllCountries(filename);
                } else if (command == "LIST") {
                    std::string countryName;
                    std::cin >> countryName;

                    listCountryTimeSeries(countryName);
                } else if (command == "RANGE"){
                    std::string seriesCode;
                    std::cin >> seriesCode;

                    findRange(seriesCode);
                } else if (command == "BUILD"){
                    std::string seriesCode;
                    std::cin >> seriesCode;
                    buildTree(seriesCode);
                } else if (command == "FIND"){
                    double mean;
                    std::string operation;
                    std::cin >> mean;
                    std::cin >> operation;

                    findMean(mean, operation);
                } else if (command == "DELETE"){
                    std::string countryName;
                    std::cin >> countryName;
                    
                    deleteCountry(countryName);
                } else if (command == "LIMITS"){
                    std::string condition;
                    std::cin >> condition;

                    findLimits(condition);
                } else if (command == "PRINT"){
                    printTree();
                } else {
                    continue;
                }
            }

            return;
        }

        /*
        This function loads every line of the given file into TimeSeries objects, ad then places those time series into SingleCountrySeries 
        objects, which are then placed into the array in the CountryTimeSeriesArray class.
        */
        void loadAllCountries(std::string filename){
            // std::string filename = filename;    //store the filename in a string for easier manipulation later on
            std::ifstream MyReadFile;
            MyReadFile.open(filename);
            std::ifstream MyInnerReadFile;
            MyInnerReadFile.open(filename);
            std::string line;
            std::string tempLine;
            bool noCountryCode = true;
            std::string curCountry = "";
            SingleCountryTimeSeries* sc;
            int arrIdx = 0;

            //read through the file line by line until the end
            while (getline(MyReadFile, line)) { 
                std::stringstream str(line); //convert to a stringstream to use in getline
                getline(str, tempLine, ','); //get first value in the line (country name)
                if (tempLine != curCountry){    //if it's the target country
                    MyInnerReadFile.clear();
                    MyInnerReadFile.seekg(0);
                    curCountry = tempLine;
                    sc = new SingleCountryTimeSeries();
                    sc -> loadCountrySeries(curCountry, MyInnerReadFile);
                    countryArray[arrIdx] = sc;
                    arrIdx++;
                }
            }

            numCountries = arrIdx;

            std::cout << "success" << std::endl;
            return;
        }
        /*
        Prints out the name of a given country and its country code, then lists all the series codes for said country
        Calls SingleCountryTimeSeries class's List() function.
        */
        void listCountryTimeSeries(std::string countryName){
            for (int i = 0; i < numCountries; i++){
                if (countryArray[i] -> getCountryName() == countryName){
                    countryArray[i] -> list();
                    break;
                }
            }
        }

        /*
        Finds the maximum mean and the minimum mean for a specific seriesCode. Returns maxMean - minMean (the range)
        */
        double findRange(std::string seriesCode){
            double minMean = -1.0;
            double maxMean = -1.0;
            double tempMean;

            //traverse the countryArray
            for (int i = 0; i < numCountries; i++){
                //find the mean for the specific series code
                tempMean = countryArray[i] -> findSeriesMean(seriesCode);
                //if the mean is valid
                if (tempMean >= 0){
                    //set minMean and maxMean to the first valid mean
                    if (minMean < 0 && maxMean < 0){
                        minMean = tempMean;
                        maxMean = tempMean;
                    } else {
                        //use two if statements in the case when tempmean is both larger than max and smaller than min (shouldn't ever happen)
                        if (tempMean < minMean){
                            minMean = tempMean;
                        }
                        if (tempMean > maxMean){
                            maxMean = tempMean;
                        }
                    }
                }
            }

            //we assume that there will always be at least one country with a valid mean for any series code
            std::cout << minMean << " " << maxMean << std::endl;
            return maxMean - minMean;
        }

        /*
        Builds a binaryTree object with two arrays, one for the means and one for the country names
        */
        void buildTree(std::string seriesCode){
            // check if there's an existing tree
            if (tree != nullptr){
                delete tree;
            }
            
            //make two arrays, one of doubles and one of strings
            std::string strArray[512]{""};
            double dblArray[512]{-1.0};
            int arrayIdx = 0;
            double curMean;
            std::string curCountryName;
            for (int i = 0; i < numCountries; i++){
                if (countryArray[i] -> isSeriesCodePresent(seriesCode)){
                    dblArray[arrayIdx] = countryArray[i] -> findSeriesMean(seriesCode);
                    strArray[arrayIdx] = countryArray[i] -> getCountryName();
                    arrayIdx++;
                }
            }

            if (arrayIdx == 0){
                return;
            }
            assert(arrayIdx < 512);

            tree = new BTree(strArray, dblArray, seriesCode, arrayIdx);

            std::cout << "success" << std::endl;
        }

        void findMean(double mean, std::string operation){
            if (tree == nullptr){
                std::cout << "failure" << std::endl;
                return;
            }
            tree -> findOperation(mean, operation);
            return;
        }

        /*
        Deletes a country from all nodes, and deletes any leaf nodes that only contained this country
        Deleting leaf nodes part is currently dysfunctional
        */
        void deleteCountry(std::string countryName){
            if (tree == nullptr){
                std::cout << "failure" << std::endl;
                return;
            }
            tree -> deleteCountryFromTree(countryName);
            return;
        }

        void findLimits(std::string condition){
            if (tree == nullptr){
                std::cout << "failure" << std::endl;
                return;
            } 
            tree -> findLims(condition);
            return;
            //if condition = lowest
                //keep traveling down left children until you can't anymore, print the country names of all countries in that node (including invalid)
            //if condition = highest
                //keep traveling down the right children until you can't anymore, print the country names for all countries in the leaf node
        }

        double calcMiddleMean(std::string seriesCode){
            double minMean = -1.0;
            double maxMean = -1.0;
            double tempMean;

            //traverse the countryArray
            for (SingleCountryTimeSeries* scts : countryArray){
                //find the mean for the specific series code
                tempMean = scts -> findSeriesMean(seriesCode);
                //if the mean is valid
                if (tempMean >= 0){
                    //set minMean and maxMean to the first valid mean
                    if (minMean < 0 && maxMean < 0){
                        minMean = tempMean;
                        maxMean = tempMean;
                    } else {
                        //use two if statements in the case when tempmean is both larger than max and smaller than min (shouldn't ever happen)
                        if (tempMean < minMean){
                            minMean = tempMean;
                        }
                        if (tempMean > maxMean){
                            maxMean = tempMean;
                        }
                    }
                }
            }

            return (minMean + maxMean)/2;
        }

        void printTree(){
            tree -> printTreeStart();
        }
};