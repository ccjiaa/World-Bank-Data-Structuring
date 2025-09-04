#include "bTreeNode.h"

class BTree{
    private:
        std::string strArr[512];
        double dblArr[512];
        BTreeNode* root;
        std::string seriesCode;
        int numVal;
        int numNode;
    
    public:
        BTree(std::string arrS[512], double arrD[512], std::string sc, int nv);
        ~BTree();
        void buildTree(BTreeNode* parent);
        std::vector< std::string > findOperation(double tarMean, std::string operation);
        void deleteCountryFromTree(std::string countryName);
        void findLims(std::string condition);
        void destroyRecursive(BTreeNode* node);
        BTreeNode* getRoot();
        void printTreeStart();
        void printTree(BTreeNode* node);
};