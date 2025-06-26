
#ifndef TREE_INDEX_H
#define TREE_INDEX_H

#include<iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>
#include"DataVector.h"
using namespace std;

class TreeIndex {
protected:
    TreeIndex() {}
    virtual ~TreeIndex() {}

public:
    virtual TreeIndex& GetConcreteInstance() = 0;
    virtual void AddData(const DataVector& data) = 0;
    virtual void RemoveData(const DataVector& data) = 0;
    virtual void MakeTree(const vector<DataVector>& dataset) = 0;
    virtual vector<int> Search(const DataVector& query, int k) = 0;
};

class KDTreeIndex : public TreeIndex {
public:
    static KDTreeIndex& GetInstance();
    void AddData(const DataVector& v)override;
    void RemoveData(const DataVector& v)override;
    void MakeTree(const vector<DataVector>& dataset)override;
    vector<int> Search(const DataVector& query, int k)override;

private:
    KDTreeIndex() {}
    ~KDTreeIndex() {}
    struct KDTreeNode {
        bool isLeaf;
        struct {
            vector<int> indices;
        } leafNode;
        struct {
            int splitDimension;
            double splitValue;
            KDTreeNode* left;
            KDTreeNode* right;
        } internalNode;
    };

    KDTreeNode* root;
    vector<DataVector> dataset;
    int dimensions;

    KDTreeNode* AddDataRecursive(KDTreeNode* node, int dataIndex, int depth);
    KDTreeNode* RemoveDataRecursive(KDTreeNode* node, int dataIndex, int depth);
    KDTreeNode* ChooseRule(vector<int>& indices, int start, int end, int depth);
    void DeleteTree(KDTreeNode* node);
    void SearchRecursive(KDTreeNode* node, const DataVector& query, int k, priority_queue<pair<double, int>>& nearestNeighbors, int depth);
    //vector<int> GetIndices(const vector<DataVector>& dataset);
    TreeIndex& GetConcreteInstance() override;
};

class RPTreeIndex : public TreeIndex {
public:
    static RPTreeIndex& GetInstance();
    void AddData(const DataVector& data) override;
    void RemoveData(const DataVector& data) override;
    void MakeTree(const std::vector<DataVector>& dataset) override;
    std::vector<int> Search(const DataVector& query, int k) override;

private:
    RPTreeIndex() {}
    ~RPTreeIndex() {}

    struct RPTreeNode {
        bool isLeaf;
        struct {
            vector<int> indices;
        } leafNode;
        struct {
            DataVector splitDirection;
            double splitValue;
            double delta;  // Added for RP-Tree
            RPTreeNode* left;
            RPTreeNode* right;
        } internalNode;
    };

    RPTreeNode* root;
    vector<DataVector> dataset;
    int dimensions;

    RPTreeNode* AddDataRecursive(RPTreeNode* node, int dataIndex, int depth);
    RPTreeNode* RemoveDataRecursive(RPTreeNode* node, int dataIndex, int depth);
    RPTreeNode* ChooseRule(vector<int>& indices, int start, int end, int depth);
    int FindFarthestPoint(const vector<int>& indices, int referenceIndex);
    void DeleteTree(RPTreeNode* node);
    void SearchRecursive(RPTreeNode* node, const DataVector& query, int k, priority_queue<std::pair<double, int>>& nearestNeighbors, int depth);
    TreeIndex& GetConcreteInstance() override;
    DataVector GenerateRandomDirection();
};

#endif // TREE_INDEX_H