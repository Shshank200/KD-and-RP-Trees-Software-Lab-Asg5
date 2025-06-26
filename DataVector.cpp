#include<bits/stdc++.h>
#include "DataVector.h"
#include <cmath>
using namespace std;

DataVector::DataVector(int dimension) {
    setDimension(dimension);
}

DataVector::~DataVector() {
    // Destructor implementation if needed
}

DataVector::DataVector(const DataVector& other) {
    v = other.v;
}

DataVector::DataVector(const vector<double>& other) {
    v = other;
}

int DataVector::getDimension() const {
    return (int)v.size();
}

double DataVector::getValue(int index) const{
    return v[index];
}

void DataVector::setValue(int index,double value){
    v[index]=value;
}

void DataVector::push_back(double value) {
    v.push_back(value);
}

DataVector& DataVector::operator=(const DataVector& other) {
    if (this != &other) {
        v = other.v;
    }
    return *this;
}

void DataVector::setDimension(int dimension) {
    v.resize(dimension, 0.0);
}

DataVector DataVector::operator+(const DataVector& other) const{
    DataVector result;
    if((*this).getDimension() != other.getDimension()){
        cout << "Error: Dimension mismatch" << endl;
        return result;
    }
    result.setDimension((*this).getDimension());
    for (size_t i = 0; i < v.size(); ++i) {
        result.v[i] = v[i] + other.v[i];
    }
    return result;
}

DataVector DataVector::operator-(const DataVector& other) const {
    DataVector result;
    if((*this).getDimension() != other.getDimension()){
        cout << "Error: Dimension mismatch" << endl;
        return result;
    }
    result.setDimension((*this).getDimension());
    for (size_t i = 0; i < v.size(); ++i) {
        result.v[i] = v[i] - other.v[i];
    }
    return result;
}

double DataVector::operator*(const DataVector& other) const {
    double result = 0.0;
    if((*this).getDimension() != other.getDimension()){
        cout << "Error: Dimension mismatch" << endl;
        return result;
    }
    for (size_t i = 0; i < v.size(); ++i) {
        result += v[i] * other.v[i];
    }
    return result;
}

bool DataVector::operator==(const DataVector& other) const {
    // Check if the dimensions are the same
    if (v.size() != other.getDimension()) {
        return false;
    }

    // Compare each component
    for (int i = 0; i < other.getDimension(); ++i) {
        double epsilon = 1e-9 * max(fabs(v[i]), fabs(other.getValue(i)));
        if (fabs(v[i]-other.getValue(i)) > epsilon) {
            return false;
        }
    }

    return true;
}

bool DataVector::operator<(const DataVector& other) const {
    // Check if the dimensions are the same
    if (v.size() != other.getDimension()) {
        return v.size() < other.getDimension();
    }

    // Compare each component
    for (int i = 0; i < other.getDimension(); ++i) {
        if (v[i] != other.getValue(i)) {
            return v[i] < other.getValue(i);
        }
    }

    return false;  // Equal vectors
}


bool DataVector::operator>(const DataVector& other) const {
    // Check if the dimensions are the same
    if (v.size() != other.getDimension()) {
        return v.size() > other.getDimension();
    }

    // Compare each component
    for (int i = 0; i < other.getDimension(); ++i) {
        if (v[i] != other.getValue(i)) {
            return v[i] > other.getValue(i);
        }
    }

    return false;  // Equal vectors
}


double DataVector::norm() const {
    double sum = 0.0;
    for (double val : v) {
        sum += val * val;
    }
    return sqrt(sum);
}

double DataVector::distance(const DataVector& other) const {
    DataVector diff = *this - other;
    return diff.norm();
}
