#include"TreeIndex.h"

using namespace std;

// KDTreeIndex Implementation

KDTreeIndex& KDTreeIndex::GetInstance() {
    static KDTreeIndex instance;
    return instance;
}

TreeIndex& KDTreeIndex::GetConcreteInstance() {
    return GetInstance();
}

void KDTreeIndex::AddData(const DataVector& v) {
    dataset.push_back(v);
    int dataIndex = dataset.size()-1;
    root = AddDataRecursive(root, dataIndex, 0);
}

void KDTreeIndex::RemoveData(const DataVector& v) {
    int dataIndex;
    for(int i=0.;i<dataset.size();i++){
        if(dataset[i]==v){
            dataIndex = i;
            break;
        }
    }
    root = RemoveDataRecursive(root, dataIndex, 0);
}

KDTreeIndex::KDTreeNode* KDTreeIndex::AddDataRecursive(KDTreeNode* node, int dataIndex, int depth) {
    if (node == nullptr) {
        KDTreeNode* newNode = new KDTreeNode();
        newNode->isLeaf = true;
        newNode->leafNode.indices.push_back(dataIndex);
        return newNode;
    }

    if (node->isLeaf) {
        node->leafNode.indices.push_back(dataIndex);

        // Check if the leaf node exceeds the threshold
        if (node->leafNode.indices.size() > 100) {
            // Split the leaf node and create internal nodes
            int medianIndex = (node->leafNode.indices.size()) / 2;
            node->isLeaf = false;

            // Choose the splitting dimension based on the current depth
            int currentDimension = node->internalNode.splitDimension;

            // Sort indices along the chosen dimension
            sort(node->leafNode.indices.begin(), node->leafNode.indices.end(),
                 [currentDimension,this](int a, int b) {
                     return this->dataset[a].getValue(currentDimension) < this->dataset[b].getValue(currentDimension);
                 });

            // Create left and right child nodes
            node->internalNode.splitDimension = currentDimension;
            node->internalNode.splitValue = dataset[node->leafNode.indices[medianIndex]].getValue(currentDimension);
            node->internalNode.left = ChooseRule(node->leafNode.indices, 0, medianIndex - 1, depth + 1);
            node->internalNode.right = ChooseRule(node->leafNode.indices, medianIndex,
                                                   node->leafNode.indices.size() - 1, depth + 1);

            // Clear indices from the leaf node
            node->leafNode.indices.clear();
        }

        return node;
    } else {
        // Non-leaf node: Traverse down the tree based on split dimension and split value
        int currentDimension = node->internalNode.splitDimension;
        if (dataset[dataIndex].getValue(currentDimension) < node->internalNode.splitValue) {
            node->internalNode.left = AddDataRecursive(node->internalNode.left, dataIndex, depth + 1);
        } else {
            node->internalNode.right = AddDataRecursive(node->internalNode.right, dataIndex, depth + 1);
        }

        return node;
    }
}

KDTreeIndex::KDTreeNode* KDTreeIndex::RemoveDataRecursive(KDTreeNode* node, int dataIndex, int depth) {
    if (node == nullptr) {
        return nullptr;
    }

    if (node->isLeaf) {
        // If the node is a leaf, find and remove the data index
        auto it = find(node->leafNode.indices.begin(), node->leafNode.indices.end(), dataIndex);
        if (it != node->leafNode.indices.end()) {
            node->leafNode.indices.erase(it);
        }

        // Check if the leaf node becomes empty after removal
        if (node->leafNode.indices.empty()) {
            delete node;
            return nullptr; // Signal parent to update its pointers
        }

        return node;
    } else {
        // Non-leaf node: Traverse down the tree based on split dimension and split value
        int currentDimension = node->internalNode.splitDimension;
        if (dataset[dataIndex].getValue(currentDimension) < node->internalNode.splitValue) {
            node->internalNode.left = RemoveDataRecursive(node->internalNode.left, dataIndex, depth + 1);
        } else {
            node->internalNode.right = RemoveDataRecursive(node->internalNode.right, dataIndex, depth + 1);
        }

        // Check if the left or right child node becomes empty after removal
        if (node->internalNode.left == nullptr) {
            return node->internalNode.right;
        } else if (node->internalNode.right == nullptr) {
            return node->internalNode.left;
        }

        return node;
    }
}

void KDTreeIndex::DeleteTree(KDTreeNode* node) {
    if (node == nullptr) {
        return;
    }

    if (node->isLeaf) {
        // Delete leaf node
        delete node;
    } else {
        // Recursively delete left and right child nodes
        DeleteTree(node->internalNode.left);
        DeleteTree(node->internalNode.right);

        // Delete internal node
        delete node;
    }
}

void KDTreeIndex::MakeTree(const vector<DataVector>& dataset) {
    this->dataset = dataset;
    dimensions = dataset[0].getDimension();
    vector<int> indices;
    for(int i=0;i<dataset.size();i++){
        indices.push_back(i);
    }
    root = ChooseRule(indices, 0, dataset.size() - 1, 0);
}

KDTreeIndex::KDTreeNode* KDTreeIndex::ChooseRule(vector<int>& indices, int start, int end, int depth) {
    if (start > end) {
        return nullptr;
    }

    // Check if it's time to create a leaf node
    if (end - start + 1 <= 100) {
        KDTreeNode* leafNode = new KDTreeNode();
        leafNode->isLeaf = true;
        leafNode->leafNode.indices.insert(leafNode->leafNode.indices.end(), indices.begin() + start, indices.begin() + end + 1);
        return leafNode;
    }

    int currentDimension;
    double currentMaxSpread = 0;

    // Calculate the variance for each dimension
    vector<double> dimensionSpread(dimensions, 0.0);
    for (int i = 0; i < dimensions; ++i) {
        double mean = 0.0;
        for (int j = start; j <= end; ++j) {
            double value = dataset[indices[j]].getValue(i);
            mean += value;
        }
        mean /= (end - start + 1);

        for (int j = start; j <= end; ++j) {
            double value = dataset[indices[j]].getValue(i);
            dimensionSpread[i] += (value - mean) * (value - mean);
        }
        dimensionSpread[i] /= (end - start + 1);
        if (dimensionSpread[i] >= currentMaxSpread) {
            currentMaxSpread = dimensionSpread[i];
            currentDimension = i;
        }
    }

    // Sort the indices based on the current dimension
    sort(indices.begin() + start, indices.begin() + end + 1,
         [currentDimension, this](int a, int b) {
             return this->dataset[a].getValue(currentDimension) < this->dataset[b].getValue(currentDimension);
         });

    int medianIndex = start + (end - start) / 2;

    KDTreeNode* newNode = new KDTreeNode();
    newNode->isLeaf = false;
    newNode->internalNode.splitDimension = currentDimension;
    newNode->internalNode.splitValue = dataset[indices[medianIndex]].getValue(currentDimension);
    newNode->internalNode.left = ChooseRule(indices, start, medianIndex - 1, depth + 1);
    newNode->internalNode.right = ChooseRule(indices, medianIndex, end, depth + 1);

    return newNode;
}

vector<int> KDTreeIndex::Search(const DataVector& query, int k) {
    priority_queue<pair<double, int>> nearestNeighbors;
    SearchRecursive(root, query, k, nearestNeighbors, 0);

    // Extract the indices from the priority queue
    vector<int> result;
    while (!nearestNeighbors.empty()) {
        result.push_back(nearestNeighbors.top().second);
        nearestNeighbors.pop();
    }

    // Reverse the result to have the nearest neighbors at the front
    reverse(result.begin(), result.end());

    return result;
}

void KDTreeIndex::SearchRecursive(KDTreeNode* node, const DataVector& query, int k,
                                 priority_queue<pair<double, int>>& nearestNeighbors,int depth) {
    if (node == nullptr) {
        return;
    }

    if (node->isLeaf) {
        // Calculate distances and update nearest neighbors
        for (int index : node->leafNode.indices) {
            double distance = dataset[index].distance(query);
            nearestNeighbors.push({distance, index});

            // Maintain the size of the priority queue to k
            if (nearestNeighbors.size() > k) {
                nearestNeighbors.pop();
            }
        }
        
    } else {
        // Non-leaf node: Traverse down the tree based on split dimension and split value
        int currentDimension = node->internalNode.splitDimension;

        // Determine which child to traverse first
        KDTreeNode* firstChild;
        KDTreeNode* secondChild;
        if (query.getValue(currentDimension) < node->internalNode.splitValue) {
            firstChild = node->internalNode.left;
            secondChild = node->internalNode.right;
        } else {
            firstChild = node->internalNode.right;
            secondChild = node->internalNode.left;
        }

        // Recursively search in the direction of the query
        SearchRecursive(firstChild, query, k, nearestNeighbors, depth + 1);

        // Check if we need to search the other side based on the distance to the splitting plane
        double splitDistance = fabs(query.getValue(currentDimension) - node->internalNode.splitValue);
        if (nearestNeighbors.size() < k || splitDistance < nearestNeighbors.top().first) {
            SearchRecursive(secondChild, query, k, nearestNeighbors, depth + 1);
        }
    }
}

// RPTreeIndex Implementation

RPTreeIndex& RPTreeIndex::GetInstance() {
    static RPTreeIndex instance;
    return instance;
}

TreeIndex& RPTreeIndex::GetConcreteInstance() {
    return GetInstance();
}

void RPTreeIndex::AddData(const DataVector& v) {
    dataset.push_back(v);
    int dataIndex = dataset.size() - 1;
    root = AddDataRecursive(root, dataIndex, 0);
}

void RPTreeIndex::RemoveData(const DataVector& v) {
    int dataIndex;
    for (int i = 0; i < dataset.size(); i++) {
        if (dataset[i] == v) {
            dataIndex = i;
            break;
        }
    }
    root = RemoveDataRecursive(root, dataIndex, 0);
}

RPTreeIndex::RPTreeNode* RPTreeIndex::AddDataRecursive(RPTreeNode* node, int dataIndex, int depth) {
    if (node == nullptr) {
        RPTreeNode* newNode = new RPTreeNode();
        newNode->isLeaf = true;
        newNode->leafNode.indices.push_back(dataIndex);
        return newNode;
    }

    if (node->isLeaf) {
        node->leafNode.indices.push_back(dataIndex);

        // Check if the leaf node exceeds the threshold
        if (node->leafNode.indices.size() > 100) {
            // Split the leaf node using the ChooseRule function
            node->isLeaf = false;
            node = ChooseRule(node->leafNode.indices, 0, node->leafNode.indices.size() - 1, depth);
            // Clear indices from the leaf node
            node->leafNode.indices.clear();
        }

        return node;
    } else {
        // Non-leaf node: Traverse down the tree based on split direction and split value
        DataVector currentDirection = node->internalNode.splitDirection;
        double splitValue = node->internalNode.splitValue;
        if (dataset[dataIndex] * currentDirection < splitValue) {
            node->internalNode.left = AddDataRecursive(node->internalNode.left, dataIndex, depth + 1);
        } else {
            node->internalNode.right = AddDataRecursive(node->internalNode.right, dataIndex, depth + 1);
        }

        return node;
    }
}

RPTreeIndex::RPTreeNode* RPTreeIndex::RemoveDataRecursive(RPTreeNode* node, int dataIndex, int depth) {
    if (node == nullptr) {
        return nullptr;
    }

    if (node->isLeaf) {
        // If the node is a leaf, find and remove the data index
        auto it = find(node->leafNode.indices.begin(), node->leafNode.indices.end(), dataIndex);
        if (it != node->leafNode.indices.end()) {
            node->leafNode.indices.erase(it);
        }

        // Check if the leaf node becomes empty after removal
        if (node->leafNode.indices.empty()) {
            delete node;
            return nullptr; // Signal parent to update its pointers
        }

        return node;
    } else {
        // Non-leaf node: Traverse down the tree based on split direction and split value
        DataVector currentDirection = node->internalNode.splitDirection;
        double splitValue = node->internalNode.splitValue;
        if (dataset[dataIndex]*currentDirection < splitValue) {
            node->internalNode.left = RemoveDataRecursive(node->internalNode.left, dataIndex, depth + 1);
        } else {
            node->internalNode.right = RemoveDataRecursive(node->internalNode.right, dataIndex, depth + 1);
        }

        // Check if the left or right child node becomes empty after removal
        if (node->internalNode.left == nullptr) {
            return node->internalNode.right;
        } else if (node->internalNode.right == nullptr) {
            return node->internalNode.left;
        }

        return node;
    }
}

void RPTreeIndex::DeleteTree(RPTreeNode* node) {
    if (node == nullptr) {
        return;
    }

    if (node->isLeaf) {
        // Delete leaf node
        delete node;
    } else {
        // Recursively delete left and right child nodes
        DeleteTree(node->internalNode.left);
        DeleteTree(node->internalNode.right);

        // Delete internal node
        delete node;
    }
}

void RPTreeIndex::MakeTree(const vector<DataVector>& dataset) {
    this->dataset = dataset;
    dimensions = dataset[0].getDimension();
    vector<int> indices;
    for(int i=0;i<dataset.size();i++){
        indices.push_back(i);
    }
    root = ChooseRule(indices, 0, dataset.size() - 1, 0);
}

RPTreeIndex::RPTreeNode* RPTreeIndex::ChooseRule(vector<int>& indices, int start, int end, int depth) {
    if (start > end) {
        return nullptr;
    }

    // Check if it's time to create a leaf node
    if (end - start + 1 <= 100) {
        RPTreeNode* leafNode = new RPTreeNode();
        leafNode->isLeaf = true;
        leafNode->leafNode.indices.insert(leafNode->leafNode.indices.end(),  indices.begin() + start, indices.begin() + end + 1);
        return leafNode;
    }

    // Choose the splitting direction randomly
    DataVector randomDirection = GenerateRandomDirection();

    // Calculate the projections along the chosen direction for each index
    vector<pair<double, int>> projections; // Pair of (projection, index)
    for (int i = start; i <= end; ++i) {
        double projection = dataset[indices[i]] * randomDirection;
        projections.push_back({projection, indices[i]});
    }

    // Sort the indices based on their projections
    sort(projections.begin(), projections.end());

    // Calculate the median value as the split value
    int medianIndex = start + (end - start) / 2;

    // Choose x at random and find the farthest point y from it
    int xIndex = projections[rand() % projections.size()].second;
    int yIndex = FindFarthestPoint(indices, xIndex);
    // Calculate the maximum distance from x to y
    double maxDistance = dataset[xIndex].distance(dataset[yIndex]);
    // Calculate delta within the specified range [−1, 1] · 6‖x − y‖/√D
    double delta = (((double)rand() / RAND_MAX) * 6 * maxDistance) / sqrt(dataset[0].getDimension());

    RPTreeNode* newNode = new RPTreeNode();
    newNode->isLeaf = false;
    newNode->internalNode.splitDirection = randomDirection;
    // Add random delta to the median
    newNode->internalNode.splitValue = projections[medianIndex].first + delta;

    // Split indices into left and right based on the split value
    vector<int> leftIndices, rightIndices;
    for (int i = start; i <= end; ++i) {
        if (dataset[indices[i]] * randomDirection < newNode->internalNode.splitValue) {
            leftIndices.push_back(indices[i]);
        } else {
            rightIndices.push_back(indices[i]);
        }
    }

    // Recursively build left and right subtrees
    newNode->internalNode.left = ChooseRule(leftIndices, 0, leftIndices.size() - 1, depth + 1);
    newNode->internalNode.right = ChooseRule(rightIndices, 0, rightIndices.size() - 1, depth + 1);

    return newNode;
}

int RPTreeIndex::FindFarthestPoint(const vector<int>& indices, int referenceIndex) {
    int farthestIndex = referenceIndex;
    double maxDistance = 0.0;

    // Iterate through all indices to find the farthest point from the reference point
    for (int i : indices) {
        double distance = dataset[referenceIndex].distance(dataset[i]);
        if (distance > maxDistance) {
            maxDistance = distance;
            farthestIndex = i;
        }
    }

    return farthestIndex;
}


vector<int> RPTreeIndex::Search(const DataVector& query, int k) {
    priority_queue<pair<double, int>> nearestNeighbors;
    SearchRecursive(root, query, k, nearestNeighbors, 0);

    // Extract the indices from the priority queue
    vector<int> result;
    while (!nearestNeighbors.empty()) {
        result.push_back(nearestNeighbors.top().second);
        nearestNeighbors.pop();
    }

    // Reverse the result to have the nearest neighbors at the front
    reverse(result.begin(), result.end());

    return result;
}

void RPTreeIndex::SearchRecursive(RPTreeNode* node, const DataVector& query, int k,
                                  priority_queue<pair<double, int>>& nearestNeighbors, int depth) {
    if (node == nullptr) {
        return;
    }

    if (node->isLeaf) {
        // Calculate distances and update nearest neighbors
        for (int index : node->leafNode.indices) {
            double distance = dataset[index].distance(query);
            nearestNeighbors.push({distance, index});
            // Maintain the size of the priority queue to k
            if (nearestNeighbors.size() > k) {
                nearestNeighbors.pop();
            }
        }

    } else {
        // Non-leaf node: Traverse down the tree based on split direction and split value
        DataVector currentDirection = node->internalNode.splitDirection;
        double splitValue = node->internalNode.splitValue;

        // Determine which child to traverse first
        RPTreeNode* firstChild;
        RPTreeNode* secondChild;
        if ((query*currentDirection) < splitValue) {
            firstChild = node->internalNode.left;
            secondChild = node->internalNode.right;
        } else {
            firstChild = node->internalNode.right;
            secondChild = node->internalNode.left;
        }

        // Recursively search in the direction of the query
        SearchRecursive(firstChild, query, k, nearestNeighbors, depth + 1);

        // Check if we need to search the other side based on the distance to the splitting plane
        double length = query.norm();
        double dot = query*currentDirection;
        double splitDistance = (length*length)-(dot*dot);
        splitDistance = sqrt(splitDistance);
        if (nearestNeighbors.size() < k || splitDistance < nearestNeighbors.top().first) {
            SearchRecursive(secondChild, query, k, nearestNeighbors, depth + 1);
        }
    }
}

DataVector RPTreeIndex::GenerateRandomDirection() {
    // Generate a random unit vector
    DataVector randomDirection;

    for (int i = 0; i < dimensions; ++i) {
        randomDirection.push_back(((double)rand() / RAND_MAX) * 2.0 - 1.0);
    }

    // Normalize the vector to make it a unit vector
    double norm = randomDirection.norm();
    for (int i = 0; i < dimensions; ++i) {
        randomDirection.setValue(i,randomDirection.getValue(i)/norm);
    }

    return randomDirection;
}
