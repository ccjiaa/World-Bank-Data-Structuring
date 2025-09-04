#include <string>
#include "bTree.h"
// #include "singleCountryTimeSeries.cpp"
#include <cassert>
#include "hashTable.h"

class CountryTimeSeriesArray {
    private:
        // SingleCountryTimeSeries* countryArray[512];
        HashTable hashtable;
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
            delete tree;
            tree = nullptr;
            // for (int i = 0; i < 512; i++){
            //     delete hashtable[i];
            //     hashtable.setValueAtIndex(1, nullptr);
            // }
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
                } else if (command == "REMOVE"){
                    std::string countryCode;
                    std::cin >> countryCode;
                    
                    removeFromTable(countryCode);
                } else if (command == "LIMITS"){
                    std::string condition;
                    std::cin >> condition;

                    findLimits(condition);
                } else if (command == "LOOKUP"){
                    std::string countryCode;
                    std::cin >> countryCode;
                    
                    lookUpInTable(countryCode);
                } else if (command == "INSERT"){
                    std::string countryCode;
                    std::string filename;
                    std::cin >> countryCode;
                    std::cin >> filename;
                    
                    insertIntoTable(countryCode, filename);
                } else if (command == "PRINT"){
                    printTree();
                } else if (command == "PRINTHASH"){
                    printHashTable();
                } else {
                    //do nothing
                }
            }

            return;
        }

        /*
        This function loads every line of the given file into TimeSeries objects, ad then places those time series into SingleCountrySeries 
        objects, which are then placed into the array in the CountryTimeSeriesArray class.
        */
        void loadAllCountries(std::string filename){
            //create two ifstreams, one to find new countries, one to call the singleCountryTimeSeries LOAD()
            std::ifstream MyReadFile;
            MyReadFile.open(filename);
            std::ifstream MyInnerReadFile;
            MyInnerReadFile.open(filename);
            std::string line;
            std::string tempLine;
            bool noCountryCode = true;
            std::string curCountry = "";
            SingleCountryTimeSeries* sc;
            int countryCount = 0;

            //read through the file line by line until the end
            while (getline(MyReadFile, line)) { 
                std::stringstream str(line); //convert to a stringstream to use in getline
                getline(str, tempLine, ','); //get first value in the line (country name)
                //if the current line has a different country from the previous
                if (tempLine != curCountry){
                    //start reading from the beginning of the file again
                    MyInnerReadFile.clear();
                    MyInnerReadFile.seekg(0);
                    //get new country name
                    curCountry = tempLine;
                    //create a singleCountryTimeSeries object
                    sc = new SingleCountryTimeSeries();
                    sc -> loadCountrySeries(curCountry, MyInnerReadFile);
                    //adds the singleCountryTimeSeries to the hash table
                    hashtable.insertToHash(sc, sc -> getCountryCode());
                    countryCount++;
                }
            }

            numCountries = countryCount;

            std::cout << "success" << std::endl;
            return;
        }
        /*
        Prints out the name of a given country and its country name, then lists all the series codes for said country
        Calls SingleCountryTimeSeries class's List() function.
        */
        void listCountryTimeSeries(std::string countryName){
            //iterate through the entire hashtable
            for (int i = 0; i < 512; i++){
                //if value not a nullptr
                if (hashtable[i] != nullptr){
                    //check if the country name matches, if yes then list out all that country's series codes
                    if (hashtable[i] -> getCountryName() == countryName){
                        hashtable[i] -> list();
                        break;
                    }
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
            for (int i = 0; i < 512; i++){
                //if the value at this hashindex is not nullptr
                if (hashtable[i] != nullptr){
                    //find the mean for the specific series code
                    tempMean = hashtable[i] -> findSeriesMean(seriesCode);
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

            //find all countries who contain the timeseries with the target seriesCode, and copy the country name and mean into the two arrays
            for (int i = 0; i < 512; i++){
                if (hashtable[i] != nullptr){
                    if (hashtable[i] -> isSeriesCodePresent(seriesCode)){
                        dblArray[arrayIdx] = hashtable[i] -> findSeriesMean(seriesCode);
                        strArray[arrayIdx] = hashtable[i] -> getCountryName();
                        arrayIdx++;
                    }
                }
            }

            //if nothing was added to the tree, do not build the tree
            if (arrayIdx == 0){
                return;
            }
            //assert that we haven't made any duplicate copies of countries
            assert(arrayIdx < 512);
            //create the tree
            tree = new BTree(strArray, dblArray, seriesCode, arrayIdx);

            std::cout << "success" << std::endl;
        }

        /*
        Find all countries whose means satisfy the operation
        */
        void findMean(double mean, std::string operation){
            //if tree hasn't been built yet, print failure and return
            if (tree == nullptr){
                std::cout << "failure" << std::endl;
                return;
            }
            //otherwise call tree method findOperation
            tree -> findOperation(mean, operation);
            return;
        }

        /*
        Deletes a country from all nodes, and deletes any leaf nodes that only contained this country
        */
        void deleteCountry(std::string countryName){
            //if tree hasn't been built yet, print failure and return
            if (tree == nullptr){
                // std::cout << "failure" << std::endl;
                return;
            }
            //otherwise call tree method deleteCountryFromTree
            tree -> deleteCountryFromTree(countryName);
            return;
        }

        /*
        Finds the names of the countries with either the smallest or largest means in the tree, depending on the condition
        */
        void findLimits(std::string condition){
            //if tree hasn't been built yet, print failure and return
            if (tree == nullptr){
                std::cout << "failure" << std::endl;
                return;
            } 
            //otherwise call tree method findLims
            tree -> findLims(condition);
            return;
        }

        /*
        Finds the country with the given countryCode in the hashtable, and prints the index and number of hash steps it took
        Prints failure if the country is not found
        */
        void lookUpInTable(std::string countryCode){
            //calls hashtable method lookUp, which also prints the correct ouputs
            hashtable.lookUp(countryCode);
        }

        /*
        Deletes the country with the given country code from the hashtable, if in tree, delete from tree as well
        */
        void removeFromTable(std::string countryCode){
            //find the index of the country with the given countryCode
            int deleteIndex = hashtable.findCountry(countryCode);
            //if you didn't find the country, print failure and return
            if (deleteIndex < 0){
                std::cout << "failure" << std::endl;
                return;
            }
            //otherwise, call BTree delete by using the obtained index to get the countryName if a tree exists
            deleteCountry(hashtable[deleteIndex] -> getCountryName());
            //delete the value from the hashtable as well, since deleteIndex must be a valid index
            hashtable.deleteIndex(deleteIndex);
            std::cout << "success" << std::endl;
            return;
        }

        /*
        Inserts a country with the given countrycode into the hashtable if it is not already in the hashtable
        */
        void insertIntoTable(std::string countryCode, std::string filename){
            //if the countryCode is already in the hashtable, assume that country codes are unique and that the country is already in the table
            if (hashtable.findCountry(countryCode) >= 0){
                std::cout << "failure" << std::endl;
                return;
            }

            //otherwise, open the file
            std::ifstream MyReadFile;
            MyReadFile.open(filename);
            //declare variables to hold the lines from the file
            std::string line;
            std::string tempLine;
            std::string curCountryName = "";
            bool reachedCode = false;

            //find the target country's countryName from its countryCode
            while (getline(MyReadFile, line)) { 
                std::stringstream str(line); //convert to a stringstream to use in getline
                //get first value in the line (country name)
                getline(str, tempLine, ','); 
                //saved the countryName
                curCountryName = tempLine;
                //get the second value in the line (country code)
                getline(str, tempLine, ','); 
                //if the current line has the same countryCode as the target countryCode, found country name, break out
                if (tempLine == countryCode){
                    break;
                }
            }
            //reset MyReadFile back to the start of the file
            MyReadFile.clear();
            MyReadFile.seekg(0);
            //instantiate a SingleCountryTimeSeries object and call its load function to fill it with timeSeries objects
            SingleCountryTimeSeries* scts = new SingleCountryTimeSeries;
            scts -> loadCountrySeries(curCountryName, MyReadFile);

            //now that the SingleCountryTimeSeries has been created, add it to the hashtable using hashtable insert
            bool isAdded = hashtable.insertToHash(scts, countryCode);

            //print success if scts is added, failure if not
            if (isAdded){
                std::cout << "success" << std::endl;
            } else {
                std::cout << "failure" << std::endl;
            }

            return;
        }

        void printTree(){
            tree -> printTreeStart();
        }

        void printHashTable(){
            std::cout << "printing" << std::endl;
            for (int i = 0; i < 512; i++){
                if (hashtable[i] == nullptr){
                    //do nothing
                } else {
                    std::cout << hashtable[i] -> getCountryName() << ", index = " << i << std::endl;
                }
            }
        }
};