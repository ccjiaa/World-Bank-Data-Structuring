#include <string>
#include "timeSeries.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "resizingArray.h"

#ifndef SINGLE_COUNTRY_TIME_SERIES_H
#define SINGLE_COUNTRY_TIME_SERIES_H

class SingleCountryTimeSeries{
    private:
        std::string countryName;       //Store counter name
        std::string countryCode;       //Store country code
        ResizingArray<TimeSeries*> arr; //create an array of pointers to time series
        
    public:
        SingleCountryTimeSeries();
        ~SingleCountryTimeSeries();
        void runCountryCommands();
        void loadCountrySeries(std::string country, std::ifstream& MyReadFile);
        void list();
        void add(std::string seriesCode, int year, int data);
        void update(std::string seriesCode, int year, int data);
        void print(std::string seriesCode);
        void deleteSeries(std::string seriesCode);
        double biggestMean();
        std::string getCountryName();
        std::string getCountryCode();
        ResizingArray<TimeSeries*> getArr();
        void setCountryName(std::string name);
        void setCountryCode(std::string code);
        double findSeriesMean(std::string seriesCode);
        bool isSeriesCodePresent(std::string seriesCode);
};

#endif