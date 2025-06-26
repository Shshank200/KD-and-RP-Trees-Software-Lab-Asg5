#include<iostream>
#include"Tree.h"
#include"DataVector.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>  
#include<algorithm>
#include<chrono>
#include<set>
#define endl '\n'
using namespace std;
int main(){
    vector<DataVector> dataset;
    cout << "How many vectors\n";
    int n;
    cin >> n;
    cout << "diension\n";
    int d;
    cin >> d;
    for(int i=0;i<n;i++){
        DataVector v;
        for(int j=0;j<d;j++){
            double x;
            cin >> x;
            v.push_back(x);
        }
        dataset.push_back(v);
    }
    cout << "How many nearest neighbors do you want to find?\n";
    int k;
    cin >> k;
    cout << "Dimension is " << dataset[0].getDimension() << endl;
    RPTreeIndex& rptree = RPTreeIndex::GetInstance();
    rptree.MakeTree(dataset);
    cout << "Enter query\n";
    DataVector v;
    for(int j=0;j<d;j++){
        double x;
        cin >> x;
        v.push_back(x);
    }
    vector<int> result = rptree.Search(v,k);
    for(int i=0;i<result.size();i++){
        cout << result[i] << ' ';
    }
    cout << endl;
}