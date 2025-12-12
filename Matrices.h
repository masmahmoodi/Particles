#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include <cmath>
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

namespace Matrices
{
    class Matrix
    {
    public:
        Matrix(int _rows, int _cols);

        const double& operator()(int i, int j) const
        {
            return a.at(i).at(j);
        }

        double& operator()(int i, int j)
        {
            return a.at(i).at(j);
        }

        int getRows() const { return rows; }
        int getCols() const { return cols; }

    protected:
        vector<vector<double>> a;

    private:
        int rows;
        int cols;
    };

    Matrix operator+(const Matrix& a, const Matrix& b);
    Matrix operator*(const Matrix& a, const Matrix& b);
    bool operator==(const Matrix& a, const Matrix& b);
    bool operator!=(const Matrix& a, const Matrix& b);
    ostream& operator<<(ostream& os, const Matrix& a);

    class RotationMatrix : public Matrix
    {
    public:
        RotationMatrix(double theta);
    };

    class ScalingMatrix : public Matrix
    {
    public:
        ScalingMatrix(double scale);
    };

    class TranslationMatrix : public Matrix
    {
    public:
        TranslationMatrix(double xShift, double yShift, int nCols);
    };
}

#endif
