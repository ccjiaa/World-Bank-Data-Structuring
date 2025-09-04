#include <iostream>
#include "singleCountryTimeSeries.h"

class BTreeNode{
    private:
        std::string nodeStrArr[512];
        double nodeDblArr[512];
        int numVal;
        double midMean;
        double lowMean;
        double highMean;
        BTreeNode* left;
        BTreeNode* right;

    public:
        BTreeNode(std::string arrS[512], double arrD[512], int nv);
        ~BTreeNode();
        void printTargetMean(double tarMean, std::string operation);
        void deleteCountry(std::string countryName);
        std::string* getStrArr();
        double* getDblArr();
        int getNumVal();
        double getMidMean();
        double getLowMean();
        double getHighMean();
        BTreeNode* getLeft();
        BTreeNode* getRight();
        void setLeft(BTreeNode* node);
        void setRight(BTreeNode* node);
        void printNode();
        void printStrArr();
        bool isAllEqual();
};