#include<iostream>
#include"TreeIndex.h"
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
   cout << "starting\n";
    const string file_path = "fmnist-test.csv"; // Adjust the file path accordingly

    // Open the CSV file
    ifstream file(file_path);

    if (!file.is_open()) {
        cerr << "Error opening the file." << endl;
        return 1;
    }

    vector<DataVector> dataset;

    // Read each line from the file
    string line;
    while (getline(file, line)) {
        DataVector data_row;
        istringstream iss(line);
        double value;

        // Read each value as a double
        while (iss >> value) {
            // Assume values are separated by commas
            data_row.push_back(value);

            // Check for a comma and consume it
            if (iss.peek() == ',') {
                iss.ignore();
            }
        }

        // Add the row to the dataset
        dataset.push_back(data_row);
    }
    cout << "done\n";
    // Close the file
    file.close();
    cout << "Which method do you want to use KD(0) or RP(1) Trees\n";
    int r;
    cin >> r;
    cout << "How many nearest neighbors do you want to find?\n";
    int k;
    cin >> k;
    vector<int> result;
    if(r){
        RPTreeIndex& rptree = RPTreeIndex::GetInstance();
        rptree.MakeTree(dataset);
        cout << "Tree made\n";
        auto start_time = chrono::high_resolution_clock::now();
        result = rptree.Search(dataset[0],k);
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        cout << "Time taken: " << duration.count() << " milliseconds" << endl;
    }
    else{
        KDTreeIndex& kdtree = KDTreeIndex::GetInstance();
        kdtree.MakeTree(dataset);
        cout << "Tree made\n";
        auto start_time = chrono::high_resolution_clock::now();
        result = kdtree.Search(dataset[0],k);
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        cout << "Time taken: " << duration.count() << " milliseconds" << endl;
    }
    for(int i=0;i<result.size();i++){
        cout << result[i] << ' ';
    }
    cout << endl;
}