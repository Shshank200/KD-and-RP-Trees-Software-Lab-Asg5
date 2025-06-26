// DataVector.h

#ifndef DATAVECTOR_H
#define DATAVECTOR_H

#include <vector>
#include <cmath>
using namespace std;
class DataVector {
private:
    vector<double> v;

public:
    // Constructor: Creates a DataVector with the specified dimension (default is 0).
    DataVector(int dimension=0);

    // Destructor: Cleans up resources when a DataVector object goes out of scope.
    ~DataVector();

    // Copy Constructor: Creates a new DataVector as a copy of another DataVector.
    DataVector(const DataVector& other);

    DataVector(const vector<double>& other);
    // Copy Assignment Operator: Assigns the values of one DataVector to another.
    DataVector& operator=(const DataVector& other);

    // Gets size of the vector
    int getDimension () const;

    double getValue(int index) const;

    void setValue(int index,double value);

    // Push Back: Adds a new element to the end of the DataVector.
    void push_back(double value);

    // Set Dimension: Sets the dimension of the DataVector, creating a new vector if necessary.
    void setDimension(int dimension=0);

    // Vector Addition: Performs element-wise addition of two DataVectors.
    DataVector operator+(const DataVector& other) const;

    // Vector Subtraction: Performs element-wise subtraction of one DataVector from another.
    DataVector operator-(const DataVector& other) const;

    // Dot Product: Computes the dot product of two DataVectors.
    double operator*(const DataVector& other) const;

    // Vector comparision: Checks equality at every index.
    bool operator==(const DataVector& other) const;

    // Vector less than operator: Compares element-wise
    bool operator<(const DataVector& other) const;

    // Vector greater than operator: Compares element-wise
    bool operator>(const DataVector& other) const;

    // Norm: Calculates the length of the vector.
    double norm() const;

    // Distance: Calculates the distance between two vectors.
    double distance(const DataVector& other) const;

    //void swap(DataVector& other) noexcept;
};

#endif // DATAVECTOR_H
