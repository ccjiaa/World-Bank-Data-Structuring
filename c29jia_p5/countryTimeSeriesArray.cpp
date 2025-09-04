#include <string>
#include "bTree.h"
// #include "singleCountryTimeSeries.cpp"
#include <cassert>
#include <unordered_map>
#include <tuple>
#include <vector>
#include <set>
#include <queue>
#include "hashTable.h"

class CountryTimeSeriesArray {
    private:
        // SingleCountryTimeSeries* countryArray[512];
        HashTable hashtable;
        std::unordered_map< std::string, std::string > convertNameToCode;
        std::string countryCodeArray[512];
        int numCountries;
        BTree* tree;
        //Create a map of maps of vectors of tuples (of type string for series code, double for threshold, and string for relation)
        std::unordered_map< std::string, std::unordered_map< std::string, std::vector< std::tuple< std::string, double, std::string >>>> graphMap = {};
        bool isGraphPresent = false;

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
            graphMap.clear();
            convertNameToCode.clear();
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
                } else if (command == "INITIALIZE") {
                    initializeGraph();
                } else if (command == "RANGE"){
                    std::string seriesCode;
                    std::cin >> seriesCode;

                    findRange(seriesCode);
                } else if (command == "UPDATE_EDGES"){
                    std::string seriesCode;
                    double threshold;
                    std::string relation;

                    std::cin >> seriesCode;
                    std::cin >> threshold;
                    std::cin >> relation;

                    updateEdges(seriesCode, threshold, relation);
                } else if (command == "ADJACENT"){
                    std::string countryCode;
                    std::cin >> countryCode;

                    printAdjacent(countryCode);
                } else if (command == "PATH"){
                    std::string countryCode1;
                    std::string countryCode2;

                    std::cin >> countryCode1;
                    std::cin >> countryCode2;
                    
                    isPathExist(countryCode1, countryCode2);
                } else if (command == "RELATIONSHIPS"){
                    std::string countryCode1;
                    std::string countryCode2;

                    std::cin >> countryCode1;
                    std::cin >> countryCode2;

                    printRelationships(countryCode1, countryCode2);
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
                } else if (command == "PRINTGRAPH"){
                    printGraph();
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
            numCountries = 0;
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
                    //add the country name and country code to an unordered map for conversion purposes
                    convertNameToCode.emplace(curCountry, sc -> getCountryCode());
                    countryCodeArray[countryCount] = sc -> getCountryCode();
                    countryCount++;
                }
            }

            numCountries = countryCount;

            std::cout << "success" << std::endl;
            return;
        }

        /*
        Adds all countries in the LOAD into the graphMap. 
        This step is more easily done during the LOAD. Since both functions will always be called one after another, this function only needs to print success.
        */
        void initializeGraph(){
            //if a graphMap already exists, clear it
            if (isGraphPresent){
                graphMap.clear();
            }
            isGraphPresent = true;

            //go through each country with a countrySeries, add to the map
            for (int i = 0; i < numCountries; i++){
                //create a new empty unordered map
                std::unordered_map< std::string, std::vector< std::tuple< std::string, double, std::string >>> tempMap;
                //add it to the graphMap
                graphMap.emplace(countryCodeArray[i], std::move(tempMap));
            }
            std::cout << "success" << std::endl;
        }

        /*
        Adds edges to nodes in the graphMap.
        */
        void updateEdges(std::string seriesCode, double threshold, std::string relation){
            bool isUpdated = false;
            bool isTreeCreated;
            isTreeCreated = buildTree(seriesCode);
            //if no tree was created, print failure as no edges can be updated
            if (!isTreeCreated){
                std::cout << "failure" << std::endl;
                return;
            }
            std::vector< std::string > relatedVec = tree -> findOperation(threshold, relation);
            int index = 0;
            int innerIndex = 0;
            std::string curCountryCode = "";
            std::string curInnerCode = "";
            //traverse the vector
            for (int i = 0; i < relatedVec.size(); i++){
                //update current country code
                curCountryCode = convertNameToCode[relatedVec[i]];
                //for each value in the vector, place all other country names in the vector into the relationship map of the current country
                for (int j = 0; j < relatedVec.size(); j++){
                    if (convertNameToCode[relatedVec[j]] != curCountryCode){
                        //use the country name to find the country code, relatedVec[j] holds the country name
                        curInnerCode = convertNameToCode[relatedVec[j]];
                        //create the tuple
                        std::tuple< std::string, double, std::string > tempTup(seriesCode, threshold, relation);
                        //add it to the graph
                        //if the current country isn't already in the inner hashmap
                        if (graphMap[curCountryCode].find(curInnerCode) == graphMap[curCountryCode].end()){
                            //create a new vector to hold the tuple
                            std::vector< std::tuple< std::string, double, std::string >> tempVec = {std::move(tempTup)};
                            //add the vector to the hash map
                            graphMap[curCountryCode].emplace(curInnerCode, std::move(tempVec));
                            isUpdated = true;
                        } else {
                            //if the tuple isn't a duplcate
                            if (!isTupleInVec(tempTup, graphMap[curCountryCode][curInnerCode])){
                                //if already in the map, a vector must already exist, simply append the tuple to the end of the existing vector
                                graphMap[curCountryCode][curInnerCode].push_back(std::move(tempTup));
                                isUpdated = true;
                            }
                        }
                    }
                }
            }

            //vector is automatically cleared as it was called on the stack

            //print success if an edge was updated, failure if none were updated
            if (isUpdated){
                std::cout << "success" << std::endl;
                return;
            }
            std::cout << "failure" << std::endl;
            return;
        }
        /*
        Checks if the given tuple is found within a given list
        */
        bool isTupleInVec(std::tuple< std::string, double, std::string > tup, std::vector< std::tuple< std::string, double, std::string >> vec){
            //get all three values
            std::string seriesCode = std::get<0>(tup);
            double threshold = std::get<1>(tup);
            std::string relation = std::get<2>(tup);

            //create variables to hold all three values for the tuples in the vector
            std::string curCode = "";
            double curThreshold = -1;
            std::string curRelation = "";

            //traverse the vector, checking for matches in all three values
            for (int i = 0; i < vec.size(); i++){
                curCode = std::get<0>(vec[i]);
                curThreshold = std::get<1>(vec[i]);
                curRelation = std::get<2>(vec[i]);

                if ((curCode == seriesCode) && (curThreshold == threshold) && (curRelation == relation)){
                    return true;
                }
            }

            //if no duplicates found, return false
            return false;
        }

        /*
        Output all countries by name that are adjacent to the given country by code
        */
        void printAdjacent(std::string countryCode){
            //if the code is found in the graph
            if(graphMap.count(countryCode) > 0){
                int count = 0;
                bool isFirstPrinted = false;
                //go over each destination node that he source node maps to
                for (auto destination : graphMap[countryCode]){
                    if(isFirstPrinted){
                        std::cout << " ";
                    }
                    //print the country name by finding the country object in the custom hashtable
                    std::cout << hashtable[hashtable.findCountry(destination.first)] -> getCountryName();
                    count++;
                    isFirstPrinted = true;
                }
                //if no adjacent nodes, print none and return
                if (count == 0){
                    std::cout << "none";
                }
                //if something was printed, go to next line
                std::cout << std::endl;
                return;
            }
            //if the count isn't found in the graph, print failure and return
            std::cout << "failure" << std::endl;
            return;
        }

        /*
        Finds if there is a path between two given country codes
        If yes, prints and returns true, else, prints and returns false
        Assume that both country codes are within the graph
        */
        bool isPathExist(std::string cCode1, std::string cCode2){
            //create a set to store traversed nodes, store the source node cCode1
            std::set< std::string > traversedSet;
            traversedSet.emplace(cCode1);
            //create a queue for BFS, place the source node cCode1 in the queue
            std::queue< std::string > todo;
            todo.push(cCode1);

            std::string curCode;

            //until the queue is empty
            while (!todo.empty()){
                //get the next value to be popped
                curCode = todo.front();
                //if its the target code, print and return true
                if (curCode == cCode2){
                    std::cout << "true" << std::endl;
                    return true;
                }
                //otherwise pop it
                todo.pop();
                //push the country codes of all nodes adjacent to the current node that haven't yet been traversed
                for (auto country : graphMap[curCode]){
                    //if the country is not yet traversed, add it to the queue and mark as traversed
                    if(traversedSet.count(country.first) == 0){
                        todo.push(country.first);
                        traversedSet.emplace(country.first);
                    }
                }
            }
            //if we exist the while loop, that means a path wasn't found, print and return false;
            std::cout << "false" << std::endl;
            return false;
        }

        /*
        Prints all relationships between two country codes. 
        */
        void printRelationships(std::string cCode1, std::string cCode2){
            bool isFirstPrinted = false;
            //if both nodes have relations with each other
            if (graphMap[cCode1].count(cCode2) > 0){
                //for each value in the vector in the inner hashmap, which contains the tuples that represent relationships
                for (auto tup : graphMap[cCode1][cCode2]){
                    if(isFirstPrinted){
                        std::cout << " ";
                    }
                    std::cout << "(" << std::get<0>(tup) << " " << std::get<1>(tup) << " " << std::get<2>(tup) << ")"; 
                    isFirstPrinted = true;
                }
                std::cout << std::endl;
                return;
            }
            //if no edge between the node, print none
            std::cout << "none" << std::endl;
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
        Builds a binaryTree object with two arrays, one for the means and one for the country names. 
        If a tree was built, return true, else, return false
        */
        bool buildTree(std::string seriesCode){
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
                return false;
            }
            //assert that we haven't made any duplicate copies of countries
            assert(arrayIdx < 512);
            //create the tree
            tree = new BTree(strArray, dblArray, seriesCode, arrayIdx);
            return true;
            // std::cout << "success" << std::endl;
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

        //prints out the graph adjacency map
        void printGraph(){
            std::cout << "printing graph:" << std::endl;
            for (auto map : graphMap){
                std::cout << "-------------------------------------------------------------------------" << std::endl;
                std::cout << "Source: " << map.first << std::endl;
                for (auto list : map.second){
                    std::cout << "Destination: " << list.first << std::endl;
                    for (int i = 0; i < list.second.size(); i++){
                        std::cout << std::get<0>(list.second[i]) << ", " << std::get<1>(list.second[i]) << ", " << std::get<2>(list.second[i]) << std::endl;
                    }
                }
                std::cout << "-------------------------------------------------------------------------" << std::endl;
            }
        }
};