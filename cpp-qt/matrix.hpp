#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <initializer_list>
#include <type_traits>
#include <cstddef>

namespace MEMS {

template<::std::size_t M, ::std::size_t N, typename T = double>
class Matrix
{
private:
    T _data[M][N];

    template<::std::size_t MM, ::std::size_t NN, typename TT>
    friend class Matrix;

public:
    using index_type = ::std::size_t;

    constexpr Matrix()
        : _data()
    {
    }

    template<typename U = T>
    constexpr Matrix(const Matrix<M,N,U>& other)
    noexcept(::std::is_nothrow_assignable<T&,const U&>::value)
        : _data()
    {
        for (index_type i=0; i<M; ++i)
        {
            for (index_type j=0; j<N; ++j)
            {
                _data[i][j] = other._data[i][j];
            }
        }
    }

    template<typename U = T>
    explicit constexpr Matrix(const U (&data)[M][N])
    noexcept(::std::is_nothrow_assignable<T&,const U&>::value)
        : _data()
    {
        for (index_type i=0; i<M; ++i)
        {
            for (index_type j=0; j<N; ++j)
            {
                _data[i][j] = data[i][j];
            }
        }
    }

    template<typename U = T>
    constexpr Matrix& operator =(const Matrix<M,N,U>& other)
    noexcept(::std::is_nothrow_assignable<T&,const U&>::value)
    {
        if (this!=&other)
        {
            for (index_type i=0; i<M; ++i)
            {
                for (index_type j=0; j<N; ++j)
                {
                    _data[i][j] = other._data[i][j];
                }
            }
        }
        return *this;
    }

    constexpr T& operator ()(index_type i, index_type j)
    {
        return _data[i][j];
    }

    constexpr const T& operator ()(index_type i, index_type j) const
    {
        return _data[i][j];
    }

    constexpr T* data()
    {
        return _data[0];
    }

    constexpr const T* data() const
    {
        return _data[0];
    }

    constexpr const T* constData() const
    {
        return _data[0];
    }

    template<typename U>
    constexpr bool operator !=(const Matrix<M,N,U>& other) const
    noexcept(noexcept(T()==U()) && noexcept(T()!=U()))
    {
        if (this == &other)
            return false;
        for (index_type i=0; i<M; ++i)
        {
            for (index_type j=0; j<N; ++j)
            {
                if (_data[i][j] != other._data[i][j])
                    return true;
            }
        }
        return false;
    }

    template<typename U>
    constexpr bool operator ==(const Matrix<M,N,U>& other) const
    noexcept(noexcept(Matrix<M,N,T>()!=Matrix<M,N,U>()))
    {
        return !(*this!=other);
    }

    template<typename U>
    constexpr Matrix<M,N,decltype(T()+U())> operator +(const Matrix<M,N,U>& other) const
    noexcept(noexcept(T()+U())
             && ::std::is_nothrow_assignable<decltype(T()+U())&,const decltype(T()+U())&>::value)
    {
        Matrix<M,N,decltype(T()+U())> mat;
        for (index_type i=0; i<M; ++i)
        {
            for (index_type j=0; j<N; ++j)
            {
                mat._data[i][j] = this->_data[i][j] + other._data[i][j];
            }
        }
        return mat;
    }

    template<typename U>
    constexpr Matrix<M,N,decltype(T()-U())> operator -(const Matrix<M,N,U>& other) const
    noexcept(noexcept(T()-U())
             && ::std::is_nothrow_assignable<decltype(T()-U())&,const decltype(T()-U())&>::value)
    {
        Matrix<M,N,decltype(T()-U())> mat;
        for (index_type i=0; i<M; ++i)
        {
            for (index_type j=0; j<N; ++j)
            {
                mat._data[i][j] = this->_data[i][j] - other._data[i][j];
            }
        }
        return mat;
    }

    template<::std::size_t P,typename U>
    constexpr Matrix<M,P,decltype(T()*U())> operator *(const Matrix<N,P,U>& other) const
    noexcept(noexcept(T()*U()+T()*U())
             && ::std::is_nothrow_assignable<decltype(T()*U())&,const decltype(T()*U()+T()*U())&>::value)
    {
        Matrix<M,P,decltype(T()*U())> mat;
        for (index_type i=0; i<M; ++i)
        {
            for (index_type k=0; k<P; ++k)
            {
                for (index_type j=0; j<N; ++j)
                {
                    mat._data[i][k] = mat._data[i][k] + this->_data[i][j] * other._data[j][k];
                }
            }
        }
        return mat;
    }

    constexpr Matrix operator -() const
    noexcept(noexcept(-T())
             && ::std::is_nothrow_assignable<T&,const decltype(-T())&>::value)
    {
        Matrix mat;
        for (index_type i=0; i<M; ++i)
        {
            for (index_type j=0; j<N; ++j)
            {
                mat._data[i][j] = - this->_data[i][j];
            }
        }
        return mat;
    }

    template<typename U>
    constexpr Matrix<M,N,decltype(T()+U())> operator +(const U& scaler) const
    noexcept(noexcept(T()+U())
             && ::std::is_nothrow_assignable<decltype(T()+U())&,const decltype(T()+U())&>::value)
    {
        Matrix<M,N,decltype(T()+U())> mat;
        for (index_type i=0; i<M; ++i)
        {
            for (index_type j=0; j<N; ++j)
            {
                mat._data[i][j] = this->_data[i][j] + scaler;
            }
        }
        return mat;
    }

    template<typename U>
    constexpr Matrix<M,N,decltype(T()-U())> operator -(const U& scaler) const
    noexcept(noexcept(T()-U())
             && ::std::is_nothrow_assignable<decltype(T()-U())&,const decltype(T()-U())&>::value)
    {
        Matrix<M,N,decltype(T()-U())> mat;
        for (index_type i=0; i<M; ++i)
        {
            for (index_type j=0; j<N; ++j)
            {
                mat._data[i][j] = this->_data[i][j] - scaler;
            }
        }
        return mat;
    }

    template<typename U>
    constexpr Matrix<M,N,decltype(T()*U())> operator *(const U& scaler) const
    noexcept(noexcept(T()*U())
             && ::std::is_nothrow_assignable<decltype(T()*U())&,const decltype(T()*U())&>::value)
    {
        Matrix<M,N,decltype(T()*U())> mat;
        for (index_type i=0; i<M; ++i)
        {
            for (index_type j=0; j<N; ++j)
            {
                mat._data[i][j] = this->_data[i][j] * scaler;
            }
        }
        return mat;
    }

    template<typename U>
    constexpr Matrix<M,N,decltype(T()/U())> operator /(const U& scaler) const
    noexcept(noexcept(T()/U())
             && ::std::is_nothrow_assignable<decltype(T()/U())&,const decltype(T()/U())&>::value)
    {
        Matrix<M,N,decltype(T()/U())> mat;
        for (index_type i=0; i<M; ++i)
        {
            for (index_type j=0; j<N; ++j)
            {
                mat._data[i][j] = this->_data[i][j] / scaler;
            }
        }
        return mat;
    }

    constexpr Matrix<N,M,T> transpose() const
    noexcept(::std::is_nothrow_assignable<T&,const T&>::value)
    {
        Matrix<N,M,T> mat;
        for (index_type i=0; i<N; ++i)
        {
            for (index_type j=0; j<M; ++j)
            {
                mat(i,j) = this->_data[j][i];
            }
        }
        return mat;
    }

}; // class Matrix

template<::std::size_t M, ::std::size_t N, typename T>
constexpr Matrix<M,N,T> makeMatrix(const T (&data)[M][N])
noexcept(::std::is_nothrow_assignable<T&,const T&>::value)
{
    return Matrix<M,N,T>(data);
}

template<::std::size_t M, ::std::size_t N, typename T>
constexpr Matrix<M,N,T> constantMatrix(const T& padding)
noexcept(::std::is_nothrow_assignable<T&,const T&>::value)
{
    using index_type = typename Matrix<M,N,T>::index_type;
    Matrix<M,N,T> mat;
    for (index_type i=0; i<M; ++i)
    {
        for (index_type j=0; j<N; ++j)
        {
            mat(i,j) = padding;
        }
    }
    return mat;
}

template<::std::size_t N, typename T = double>
constexpr Matrix<N,N,T> identityMatrix(T unit = 1.)
noexcept(::std::is_nothrow_assignable<T&,T>::value)
{
    using index_type = typename Matrix<N,N,T>::index_type;
    Matrix<N,N,T> mat;
    for (index_type i=0; i<N; ++i)
    {
        mat(i,i) = unit;
    }
    return mat;
}

template<::std::size_t N, typename T>
constexpr Matrix<N,N,T> diagonalMatrix(const T (&array)[N])
noexcept(::std::is_nothrow_assignable<T&,const T&>::value)
{
    using index_type = typename Matrix<N,N,T>::index_type;
    Matrix<N,N,T> mat;
    for (index_type i=0; i<N; ++i)
    {
        mat(i,i) = array[i];
    }
    return mat;
}

template<::std::size_t M, ::std::size_t N, typename T, typename U>
constexpr Matrix<M,N,decltype(T()+U())> operator +(const T& scaler, const Matrix<M,N,U>& mat)
noexcept(noexcept(mat+scaler))
{
    return mat + scaler;
}

template<::std::size_t M, ::std::size_t N, typename T, typename U>
constexpr Matrix<M,N,decltype(T()-U())> operator -(const T& scaler, const Matrix<M,N,U>& mat)
noexcept(noexcept(-(mat-scaler)))
{
    return -(mat - scaler);
}

template<::std::size_t M, ::std::size_t N, typename T, typename U>
constexpr Matrix<M,N,decltype(T()*U())> operator *(const T& scaler, const Matrix<M,N,U>& mat)
noexcept(noexcept(mat*scaler))
{
    return mat * scaler;
}

template<::std::size_t M, ::std::size_t N, typename T>
constexpr Matrix<N,M,T> transpose(const Matrix<M,N,T> mat)
noexcept(noexcept(mat.transpose()))
{
    return mat.transpose();
}

} // namespace MEMS

#endif // MATRIX_HPP
