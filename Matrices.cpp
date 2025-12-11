#include "Matrices.h"

namespace Matrices
{
    Matrix::Matrix(int _rows, int _cols)
        : rows(_rows),
          cols(_cols),
          a(_rows, std::vector<double>(_cols, 0.0))
    {
    }

    Matrix operator+(const Matrix& A, const Matrix& B)
    {
        int rows = A.getRows();
        int cols = A.getCols();
        Matrix C(rows, cols);

        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                C(i, j) = A(i, j) + B(i, j);

        return C;
    }

    Matrix operator*(const Matrix& A, const Matrix& B)
    {
        int rows  = A.getRows();
        int inner = A.getCols();
        int cols  = B.getCols();

        Matrix C(rows, cols);

        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                double sum = 0.0;
                for (int k = 0; k < inner; ++k)
                    sum += A(i, k) * B(k, j);

                C(i, j) = sum;
            }
        }
        return C;
    }

    bool operator==(const Matrix& A, const Matrix& B)
    {
        if (A.getRows() != B.getRows() || A.getCols() != B.getCols())
            return false;

        for (int i = 0; i < A.getRows(); ++i)
            for (int j = 0; j < A.getCols(); ++j)
                if (A(i, j) != B(i, j))
                    return false;

        return true;
    }

    bool operator!=(const Matrix& A, const Matrix& B)
    {
        return !(A == B);
    }

    std::ostream& operator<<(std::ostream& os, const Matrix& A)
    {
        for (int i = 0; i < A.getRows(); ++i)
        {
            for (int j = 0; j < A.getCols(); ++j)
            {
                os << A(i, j);
                if (j + 1 < A.getCols())
                    os << ' ';
            }
            if (i + 1 < A.getRows())
                os << '\n';
        }
        return os;
    }

    RotationMatrix::RotationMatrix(double theta)
        : Matrix(2, 2)
    {
        (*this)(0, 0) = std::cos(theta);
        (*this)(0, 1) = -std::sin(theta);
        (*this)(1, 0) = std::sin(theta);
        (*this)(1, 1) = std::cos(theta);
    }

    ScalingMatrix::ScalingMatrix(double scale)
        : Matrix(2, 2)
    {
        (*this)(0, 0) = scale;
        (*this)(0, 1) = 0.0;
        (*this)(1, 0) = 0.0;
        (*this)(1, 1) = scale;
    }

    TranslationMatrix::TranslationMatrix(double xShift, double yShift, int nCols)
        : Matrix(2, nCols)
    {
        for (int j = 0; j < nCols; ++j)
        {
            (*this)(0, j) = xShift;
            (*this)(1, j) = yShift;
        }
    }
}
