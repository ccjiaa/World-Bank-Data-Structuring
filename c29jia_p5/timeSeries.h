#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "resizingArray.h"

#ifndef TIME_SERIES_H
#define TIME_SERIES_H

class TimeSeries {
    private:
        std::string seriesName;
        std::string seriesCode;
        ResizingArray<int> yearList;
        ResizingArray<double> dataList;
        double m;
        double b;
        
    public:
        //constructor
        TimeSeries();
        ~TimeSeries();
        void runCommands();
        void load(std::string newLine);
        void print();
        void add(int year, int data);
        bool update(int year, int data);
        double mean();
        bool is_monotonic();
        void best_fit(double &m, double &b);
        bool isValid(double val);
        std::string getSeriesName();
        std::string getSeriesCode();
        ResizingArray<int> getYearList();
        ResizingArray<double> getDataList();
        double getM();
        double getB();
        void printYearList();
        void printDataList();
};

#endif