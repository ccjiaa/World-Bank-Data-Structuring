#include "bTreeNode.h"
#include <cassert>
#include <vector>

BTreeNode::BTreeNode(std::string arrS[512], double arrD[512], int nv){
    //initialize the lowmean and highmean
    lowMean = 0;
    highMean = 0;
    //copy the values from the given arrays in the member arrays
    for (int i = 0; i < 512; i++){
        nodeStrArr[i] = "";
        nodeDblArr[i] = -1.0;
    }
    for (int i = 0; i < nv; i++){
        nodeStrArr[i] = arrS[i];
        nodeDblArr[i] = arrD[i];
    }
    //initialize the numVal
    numVal = nv;
    //instantiate variables to keep track of the minimum and maxiumum mean
    double curLow = -1.0;
    double curHigh = -1.0;
    double curMean;
    //find the minimum and maximum means
    for (int i = 0; i < numVal; i++){
        //store the current tuple mean
        curMean = nodeDblArr[i];
        //if the mean is less than 0, it must be -1, i.e. invalid mean, which we treat as mean = 0
        if (curMean < 0){
            curMean = 0;
        }
        //set low and high to the first mean
        if (curLow < 0 && curHigh < 0){
            curLow = curMean;
            curHigh = curMean;
        }
        //If curMean is higher than high or lower than low, replace curLow/curHigh
        if (curMean < curLow){
            curLow = curMean;
        }
        if (curMean > curHigh){
            curHigh = curMean;
        }
    }
    //set lowMean and highMean to the correct values
    lowMean = curLow;
    highMean = curHigh;
    //calculate midMean
    midMean = (highMean + lowMean)/2;
    //initialize the left and right child pointers
    left = nullptr;
    right = nullptr;
}

BTreeNode::~BTreeNode(){
    // delete[] nodeStrArr;
    // delete[] nodeDblArr;
    left = nullptr;
    right = nullptr;
}

void BTreeNode::printTargetMean(double tarMean, std::string operation){
    //asserts that numVal won't be greater than array size to avoid seg faults
    assert(numVal < 512);
    if (numVal > 0){
        if (operation == "less"){
            for (int i = 0; i < numVal; i++){
                if (nodeDblArr[i] < tarMean){
                    std::cout << nodeStrArr[i] << " ";
                }
            }
        } else if (operation == "greater"){
            for (int i = 0; i < numVal; i++){
                if (nodeDblArr[i] > tarMean){
                    std::cout << nodeStrArr[i] << " ";
                }
            }
        } else {
            double temp = 0;
            for (int i = 0; i < numVal; i++){
                temp = nodeDblArr[i];
                //if the mean is an invalid mean -1, change the mean to 0
                if (temp < 0){
                    temp = 0;
                }
                //if the mean in the node and the target mean are equal, print the country name
                if (std::abs(temp - tarMean) < 0.001){
                    std::cout << nodeStrArr[i] << " ";
                }
            }
        }
    }
    std::cout << std::endl;
}

std::vector< std::string > BTreeNode::returnTargetMean(double tarMean, std::string operation){
    //create a new vector
    std::vector< std::string > strVec;
    //asserts that numVal won't be greater than array size to avoid seg faults
    assert(numVal < 512);
    int index = 0;
    if (numVal > 0){
        if (operation == "less"){
            for (int i = 0; i < numVal; i++){
                if (nodeDblArr[i] < tarMean){
                    strVec.push_back(nodeStrArr[i]);
                    index++;
                }
            }
        } else if (operation == "greater"){
            for (int i = 0; i < numVal; i++){
                if (nodeDblArr[i] > tarMean){
                    strVec.push_back(nodeStrArr[i]);
                    index++;
                }
            }
        } else {
            double temp = 0;
            for (int i = 0; i < numVal; i++){
                temp = nodeDblArr[i];
                //if the mean is an invalid mean -1, change the mean to 0
                if (temp < 0){
                    temp = 0;
                }
                //if the mean in the node and the target mean are equal, print the country name
                if (std::abs(temp - tarMean) < 0.001){
                    strVec.push_back(nodeStrArr[i]);
                    index++;
                }
            }
        }
    }

    return strVec;
}

void BTreeNode::deleteCountry(std::string countryName){
    std::string tempStr = "";
    double tempDbl = -1.0;
    for (int i = 0; i < numVal; i++){
        if (nodeStrArr[i] == countryName){
            tempStr = nodeStrArr[i];
            nodeStrArr[i] = nodeStrArr[numVal - 1];
            nodeStrArr[numVal - 1] = tempStr;

            tempDbl = nodeDblArr[i];
            nodeDblArr[i] = nodeDblArr[numVal - 1];
            nodeDblArr[numVal - 1] = tempDbl;
            
            numVal--;
            return;
        }
    }
    return;
}

std::string* BTreeNode::getStrArr(){
    return nodeStrArr;
}
double* BTreeNode::getDblArr(){
    return nodeDblArr;
}
int BTreeNode::getNumVal(){
    return numVal;
}
double BTreeNode::getMidMean(){
    return midMean;
}
double BTreeNode::getLowMean(){
    return lowMean;
}
double BTreeNode::getHighMean(){
    return highMean;
}
BTreeNode* BTreeNode::getLeft(){
    return left;
}
BTreeNode* BTreeNode::getRight(){
    return right;
}

void BTreeNode::setLeft(BTreeNode* node){
    left = node;
}
void BTreeNode::setRight(BTreeNode* node){
    right = node;
}

void BTreeNode::printNode(){
    std::cout << "array: " << std::endl;
    std::cout << numVal << std::endl;
    std::string tempString;
    double tempMean;
    int count = 0;
    std::cout << "countries:";
    for (int i = 0; i < numVal; i++){
        std::cout << " " << nodeStrArr[i];
    }
    std::cout << std::endl;
    std::cout << "mean: " << midMean << std::endl;
}

void BTreeNode::printStrArr(){
    std::cout << nodeStrArr[0];
    for (int i = 1; i < numVal; i++){
        std::cout << " " << nodeStrArr[i];
    }
    std::cout << std::endl;
    return;
}

bool BTreeNode::isAllEqual(){
    if (highMean - lowMean <= 0.001 || -1*(lowMean - highMean) <= 0.001){
        return true;
    }
    return false;
}