#include "singleCountryTimeSeries.h"
#include <string>

class HashTable{
    private:
        SingleCountryTimeSeries* hashArray[512]{nullptr};
        bool isPrevFilledArray[512]{false};
        int numVal;
        int arraySize;
    public:
        HashTable();
        ~HashTable();
        int calcHash(std::string countryCode, int hashTimes);
        int firstHash(int sumW);
        int secondHash(int sumW);
        void lookUp(std::string countryCode);
        int findCountry(std::string countryCode);
        bool insertToHash(SingleCountryTimeSeries* scts, std::string countryCode);
        bool deleteFromHash(std::string countryCode);
        bool deleteIndex(int index);
        int getNumVal();
        int getArraySize();
        SingleCountryTimeSeries* operator[] (int index);
};