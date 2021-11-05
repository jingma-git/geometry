#pragma once

namespace egl
{
    // Perform concatenation of a two matrices along a single dimension
    // If dim == 1, then D = [A;B;C]. If dim == 2 then C = [A B C]
    //
    // Template:
    //   Scalar  scalar data type for sparse matrices like double or int
    //   Mat  matrix type for all matrices (e.g. MatrixXd, SparseMatrix)
    //   MatC  matrix type for output matrix (e.g. MatrixXd) needs to support
    //     resize
    // Inputs:
    //   A  first input matrix
    //   B  second input matrix
    //   C  third input matrix
    //   dim  dimension along which to concatenate, 1 or 2
    // Outputs:
    //   D  output matrix
    //
    template <typename Scalar>
    void cat(
        const int dim,
        const Eigen::SparseMatrix<Scalar> &A,
        const Eigen::SparseMatrix<Scalar> &B,
        const Eigen::SparseMatrix<Scalar> &C,
        Eigen::SparseMatrix<Scalar> &D)
    {
        using namespace Eigen;
        if (dim == 1)
        {
            int rows = A.rows() + B.rows() + C.rows();
            int cols = A.cols();
            D.resize(rows, cols);

            Eigen::VectorXi per_col = Eigen::VectorXi::Zero(cols);
            for (int k = 0; k < A.outerSize(); ++k)
            {
                for (typename SparseMatrix<Scalar>::InnerIterator it(A, k); it; ++it)
                {
                    per_col(k)++;
                }
                for (typename SparseMatrix<Scalar>::InnerIterator it(B, k); it; ++it)
                {
                    per_col(k)++;
                }
                for (typename SparseMatrix<Scalar>::InnerIterator it(C, k); it; ++it)
                {
                    per_col(k)++;
                }
            }

            for (int k = 0; k < A.outerSize(); ++k)
            {
                for (typename SparseMatrix<Scalar>::InnerIterator it(A, k); it; ++it)
                {
                    D.insert(it.row(), k) = it.value();
                }
                for (typename SparseMatrix<Scalar>::InnerIterator it(B, k); it; ++it)
                {
                    D.insert(A.rows() + it.row(), k) = it.value();
                }
                for (typename SparseMatrix<Scalar>::InnerIterator it(C, k); it; ++it)
                {
                    D.insert(A.rows() + B.rows() + it.row(), k) = it.value();
                }
            }
        }
        else if (dim == 2)
        {
            int rows = A.rows();
            int cols = A.cols() + B.cols() + C.cols();
            D.resize(rows, cols);

            Eigen::VectorXi per_col = Eigen::VectorXi::Zero(cols);
            for (int k = 0; k < A.outerSize(); ++k)
            {
                for (typename SparseMatrix<Scalar>::InnerIterator it(A, k); it; ++it)
                {
                    per_col(k)++;
                }
            }
            for (int k = 0; k < B.outerSize(); ++k)
            {
                for (typename SparseMatrix<Scalar>::InnerIterator it(B, k); it; ++it)
                {
                    per_col(A.cols() + k)++;
                }
            }
            for (int k = 0; k < C.outerSize(); ++k)
            {
                for (typename SparseMatrix<Scalar>::InnerIterator it(C, k); it; ++it)
                {
                    per_col(A.cols() + B.cols() + k)++;
                }
            }

            for (int k = 0; k < A.outerSize(); ++k)
            {
                for (typename SparseMatrix<Scalar>::InnerIterator it(A, k); it; ++it)
                {
                    D.insert(it.row(), k) = it.value();
                }
                for (typename SparseMatrix<Scalar>::InnerIterator it(B, k); it; ++it)
                {
                    D.insert(it.row(), A.cols() + k) = it.value();
                }
                for (typename SparseMatrix<Scalar>::InnerIterator it(C, k); it; ++it)
                {
                    D.insert(it.row(), A.cols() + B.cols() + k) = it.value();
                }
            }
        }
    }
}