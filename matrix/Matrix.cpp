#include "Matrix.h"

namespace mario
{
	bool isEqual(const double &_num1, const double &_num2)
	{
		if (_num1 - _num2<=EPSINON 
			&& _num1 - _num2>=-EPSINON)
		{
			return true;
		}

		return false;
	}


	Matrix::Matrix() : m_row(0), m_col(0), m_data(NULL){}


	Matrix::Matrix(const int &_row, const int &_col, const double &_e) : m_row(_row), m_col(_col)
	{
		if (_row <= 0 || _col <= 0)
		{
			throw "Error in Matrix(int&, int&, const double&): the row and col of matrix must > 0.";
			return;
		}

		m_data = new double[m_row*m_col];
		for (int i = 0; i < m_row*m_col; ++i)
		{
			m_data[i] = _e;
		}
	}


	Matrix::Matrix(const matrix &_m)
	{
		m_data = new double[_m.m_row*_m.m_col];
		m_row = _m.m_row;
		m_col = _m.m_col;
		memcpy((void*)m_data, (void*)_m.m_data, m_row*m_col*sizeof(double));
	}


	Matrix::~Matrix()
	{
		release();
	}

	
	void Matrix::release()
	{
		delete[]m_data;
		m_data = NULL;
		m_row = 0;
		m_col = 0;
	}
	

	int Matrix::rows() const
	{
		return m_row;
	}


	int Matrix::cols() const
	{
		return m_col;
	}


	bool Matrix::isSquareMatrix() const
	{
		return m_row == m_col;
	}


	double* Matrix::getRowPoint(const int &_r) const
	{
		if (_r < 0 || _r >= m_row)
		{
			//cout << _r << "\t" << m_row << endl;
			throw "Error in getRowPoint(const int&) const: the index of row is wrong.";
			return NULL;
		}

		return m_data + _r*m_col;
	}


	double Matrix::get(const int &_row, const int &_col) const
	{
		if (_row<0 || _col<0
			|| _row>m_row - 1 || _col>m_col - 1)
		{
			throw "Error in Matrix::get(const int&, const int&) const: the index is wrong.";
			return 0;
		}
		
		double *pR = getRowPoint(_row);
		return pR[_col];
	}


	bool Matrix::set(const int &_row, const int &_col, const double &_e)
	{
		if (_row<0 || _col<0
			|| _row>m_row || _col>m_col)
		{
			throw "Error in Matrix::set(const int&, const int&, const double&): the index is wrong.";
			return false;
		}

		double *pR = getRowPoint(_row);
		pR[_col] = _e;

		return true;
	}


	bool Matrix::swapRow(const int &_i, const int &_j)
	{
		if (_i < 0 || _j < 0
			|| _i >= m_row 
			|| _j >= m_row)
		{
			throw "Error in swapRow(const int&, const int&): i<0 || j<0 || i>=row || j>=row.";
			return false;
		}

		double temp = 0;
		double *pI = m_data + _i*m_col;
		double *pJ = m_data + _j*m_col;
		for (int k = 0; k < m_col; ++k)
		{
			/*
			temp = pI[k];
			pI[k] = pJ[k];
			pJ[k] = temp;
			*/
			pI[k] += pJ[k];
			pJ[k] = pI[k] - pJ[k];
			pI[k] = pI[k] - pJ[k];
		}

		return true;
	}


	double Matrix::getDet() const
	{
		if (!isSquareMatrix())
		{
			throw "Error in getDet(): the matrix is not square.";
		}

		Matrix copyMatrix(*this);
		//??????????????????????????????
		int iter = 0;

		double det = 1;
		for (int i = 0; i < m_row; ++i)
		{
			if (isEqual(copyMatrix(i, i), 0))
			{
				for (int j = i; j < m_row; ++j)
				{
					if (!isEqual(copyMatrix(j, i), 0))
					{
						if (!copyMatrix.swapRow(i, j))
						{
							return 0;
						}
						++iter;
					}
				}
			}

			for (int k = i + 1; k < m_row; ++k)
			{
				double scale = -1 * copyMatrix(k, i) / copyMatrix(i, i);
				double data = 0;
				for (int u = 0; u < m_row; ++u)
				{
					data = copyMatrix(k, u) + copyMatrix(i, u)*scale;
					copyMatrix.set(k, u, data);
				}
			}
		}

		for (int i = 0; i < m_row; ++i)
		{
			det *= copyMatrix(i, i);
		}

		if (1 == iter % 2)
		{
			det *= -1;
		}

		return det;
	}


	Matrix Matrix::inv_LU() const
	{
		if (!isSquareMatrix())
		{
			throw "Error in inv_LU(): The matrix is not square.";
			return Matrix();
		}

		double det = getDet();
		if (det >= -EPSINON && det <= EPSINON)
		{
			throw "Error in inv_LU(): the determinant is equal to 0.";
			return Matrix();
		}

		int n = m_row;
		//??????l???l_inverse???u???u_inverse??????
		Matrix l(n, n);
		Matrix l_inverse(n, n);
		Matrix u(n, n);
		Matrix u_inverse(n, n);

		//?????????????????????
		for (int i = 0; i < n; ++i)
		{
			l.set(i, i, 1);
			l_inverse.set(i, i, 1);
		}

		for (int i = 0; i < n; ++i)
		{
			for (int j = i; j < n; ++j)
			{
				double s = 0;
				for (int k = 0; k < i; ++k)
				{
					s += l(i, k) * u(k, j);	//????????????u???
				}
				u.set(i, j, get(i, j) - s);
			}

			for (int j = i + 1; j < n; ++j)
			{
				double s = 0;
				for (int k = 0; k < i; ++k)
				{
					s += l(j, k) * u(k, i);
				}
				l.set(j, i, (get(j, i) - s) / u(i, i));	//????????????l???
			}
		}

		for (int i = 1; i < n; ++i)
		{
			for (int j = 0; j < i; ++j)
			{
				double s = 0;
				for (int k = 0; k < i; ++k)
				{
					s += l(i, k) * l_inverse(k, j);
				}
				l_inverse.set(i, j, -s);
			}
		}

		for (int i = 0; i < n; ++i)
		{
			u_inverse.set(i, i, 1 / u(i, i));
		}
		for (int i = 1; i < n; ++i)
		{
			for (int j = i - 1; j >= 0; --j)
			{
				double s = 0;
				for (int k = j + 1; k <= i; ++k)
				{
					s += u(j, k) * u_inverse(k, i);
				}
				u_inverse.set(j, i, -s / u(j, j));
			}
		}

		Matrix result(u_inverse * l_inverse);

		for (int i = 0; i < result.m_row; ++i)
		{
			for (int j = 0; j < result.m_col; ++j)
			{
				if (_isnan(result.get(i, j)))
				{
					throw "Error in inv_LU(): the result is 1.#INF or -1.#IND or 1.#INF000 or -1.#INF000.";
				}
			}
		}

		return result;
	}


	Matrix Matrix::T() const
	{
		Matrix result(m_col, m_row);

		for (int r = 0; r < m_row; ++r)
		{
			double *pSrc = getRowPoint(r);
			for (int c = 0; c < m_col; ++c)
			{
				double *pDst = result.getRowPoint(c);
				pDst[r] = pSrc[c];
			}
		}

		return result;
	}


	Matrix Matrix::mul(const Matrix &_m) const
	{
		if (m_row != _m.m_row || m_col != _m.m_col)
		{
			throw "Error in mul(): the size of matrix is not equal.";
		}
		else if (NULL == m_data)
		{
			throw "Error in mul(): the data point to NULL.";
		}

		Matrix result(m_row, m_col);

		double *pR = result.m_data;
		double *pM1 = m_data;
		double *pM2 = _m.m_data;
		for (int i = 0; i < m_row*m_col; ++i)
		{
			pR[i] = pM1[i] * pM2[i];
		}

		return result;
	}


	double Matrix::dot(const Matrix &_m) const
	{
		if (m_row != _m.m_row || m_col != _m.m_col)
		{
			throw "Error in Matrix::dot(m): m_row != _m.m_row || m_col != _m.m_col.";
		}

		matrix result = mul(_m).sumByCol().sumByRow();
		double r = *result.m_data;

		return r;
	}


	double Matrix::operator()(const int &_row, const int &_col) const
	{
		return get(_row, _col);
	}


	Matrix& Matrix::operator=(const Matrix &_m)
	{
		if (_m.m_data == m_data)
		{
			return *this;
		}
		if (NULL == m_data)
		{
			m_row = _m.m_row;
			m_col = _m.m_col;
			m_data = new double[m_row*m_col];
			memcpy((void*)m_data, (void*)_m.m_data, m_row*m_col*sizeof(double));
			return *this;
		}
		if (m_row != _m.m_row || m_col != _m.m_col)
		{
			throw "Error in operator=(): size of matrix is not equal.";
		}
		
		double *pDst = m_data;
		double *pSrc = _m.m_data;
		memcpy((void*)pDst, (void*)pSrc, m_col*m_col*sizeof(double));
		
		return *this;
	}


	Matrix Matrix::row(const int &_i) const
	{
		if (_i<0 || _i>m_row - 1)
		{
			throw "Error in row(): the index is out of boundary.";
		}

		Matrix result(1, m_col);

		double *pSrc = getRowPoint(_i);
		double *pDst = result.m_data;
		memcpy((void*)pDst, (void*)pSrc, m_col*sizeof(double));
		
		return result;
	}


	Matrix Matrix::col(const int &_i) const
	{
		if (_i<0 || _i>m_col - 1)
		{
			throw "Error in row(): the index is out of boundary.";
		}

		Matrix result(m_row, 1);

		double *pSrc = m_data + _i;
		double *pDst = result.m_data;
		for (int i = 0; i < m_row; ++i)
		{
			pDst[i] = pSrc[i*m_col];
		}

		return result;
	}


	Matrix Matrix::block(const int &_row, const int &_col, const int &_height, const int &_width) const
	{
		if (_row < 0 || _col < 0 || _height <= 0 || _width <= 0)
		{
			throw "Error in block(): _row < 0 || _col < 0 || _height <= 0 || _width <= 0.";
		}
		if (_row + _height>m_row || _col + _width>m_col)
		{
			throw "Error in block(): _row + _height>m_row || _col + _width>m_col.";
		}

		Matrix result(_height, _width);

		for (int r = _row, resultRow = 0; r < _row + _height; ++r, ++resultRow)
		{
			double *pR = result.getRowPoint(resultRow);
			double *pM = getRowPoint(r);
			for (int c = _col; c < _col + _width; ++c)
			{
				pR[c] = pM[c];
			}
		}

		return result;
	}


	Matrix Matrix::sumByRow() const
	{
		if (NULL == m_data)
		{
			throw "Error in sumByRow(): the data is NULL.";
		}

		Matrix result(m_row, 1, 0);

		double *pDst = result.m_data;
		for (int i = 0; i < m_row; ++i)
		{
			double *pSrc = getRowPoint(i);
			for (int j = 0; j < m_col; ++j)
			{
				pDst[i] += pSrc[j];
			}
		}
		
		return result;
	}


	Matrix Matrix::sumByCol() const
	{
		if (NULL == m_data)
		{
			throw "Error in sumByRow(): the data is NULL.";
		}

		Matrix result(1, m_col, 0);

		double *pDst = result.m_data;
		for (int i = 0; i < m_col; ++i)
		{
			double *pSrc = m_data + i;
			for (int j = 0; j < m_row; ++j)
			{
				pDst[i] += pSrc[j*m_col];
			}
		}

		return result;
	}


	Matrix Matrix::meanByRow() const
	{
		return sumByRow() / m_col;
	}


	Matrix Matrix::meanByCol() const
	{
		return sumByCol() / m_row;
	}


	Matrix Matrix::log2() const
	{
		if (NULL == m_data)
		{
			throw "Error in sqrt(): data is NULL.";
		}

		Matrix result(m_row, m_col);

		for (int r = 0; r < m_row; ++r)
		{
			double *pR = result.getRowPoint(r);
			double *pM = getRowPoint(r);
			for (int c = 0; c < m_col; ++c)
			{
				if (pM[c] <= 0)
				{
					throw "Error in Matrix::log(): pM[c] <= 0.";
				}
				pR[c] = std::log(pM[c]);
			}
		}

		return result;
	}


	Matrix Matrix::sqrt() const
	{
		if (NULL == m_data)
		{
			throw "Error in sqrt(): data is NULL.";
		}

		Matrix result(m_row, m_col);

		for (int r = 0; r < m_row; ++r)
		{
			double *pDst = result.getRowPoint(r);
			double *pSrc = getRowPoint(r);
			for (int c = 0; c < m_col; ++c)
			{
				if (pSrc[c] < 0)
				{
					throw "Error in sqrt(): data < 0.";
				}

				pDst[c] = std::sqrt(pSrc[c]);
			}
		}

		return  result;
	}


	Matrix Matrix::exp() const
	{
		if (NULL == m_data)
		{
			throw "Error in sqrt(): data is NULL.";
		}

		Matrix result(m_row, m_col);

		for (int r = 0; r < m_row; ++r)
		{
			double *pDst = result.getRowPoint(r);
			double *pSrc = getRowPoint(r);
			for (int c = 0; c < m_col; ++c)
			{
				pDst[c] = std::exp(pSrc[c]);
			}
		}

		return  result;
	}


	Matrix Matrix::sin() const
	{
		if (NULL == m_data)
		{
			throw "Error in sqrt(): data is NULL.";
		}

		Matrix result(m_row, m_col);

		for (int r = 0; r < m_row; ++r)
		{
			double *pDst = result.getRowPoint(r);
			double *pSrc = getRowPoint(r);
			for (int c = 0; c < m_col; ++c)
			{
				pDst[c] = std::sin(pSrc[c]);
			}
		}

		return  result;
	}


	Matrix Matrix::cos() const
	{
		if (NULL == m_data)
		{
			throw "Error in sqrt(): data is NULL.";
		}

		Matrix result(m_row, m_col);

		for (int r = 0; r < m_row; ++r)
		{
			double *pDst = result.getRowPoint(r);
			double *pSrc = getRowPoint(r);
			for (int c = 0; c < m_col; ++c)
			{
				pDst[c] = std::cos(pSrc[c]);
			}
		}

		return  result;
	}


	Matrix Matrix::tan() const
	{
		if (NULL == m_data)
		{
			throw "Error in sqrt(): data is NULL.";
		}

		Matrix result(m_row, m_col);

		for (int r = 0; r < m_row; ++r)
		{
			double *pDst = result.getRowPoint(r);
			double *pSrc = getRowPoint(r);
			for (int c = 0; c < m_col; ++c)
			{
				pDst[c] = std::tan(pSrc[c]);
			}
		}

		return  result;
	}


	Matrix Matrix::sigmoid() const
	{
		Matrix _m = *this*-1;
		Matrix exp_x = _m.exp();

		return 1 / (1 + exp_x);
	}


	Matrix Matrix::relu() const
	{
		Matrix result(m_row, m_col);

		double *pDst = result.m_data;
		double *pSrc = m_data;
		for (int i = 0; i < m_row*m_col; ++i)
		{
			pDst[i] = pSrc[i]>0 ? pSrc[i] : 0;
		}

		return result;
	}


	Matrix Matrix::tanh() const
	{
		matrix x_ = *this*-1;

		Matrix exp_x_ = x_.exp();
		Matrix exp_x = exp();
		
		return (exp_x - exp_x_) / (exp_x + exp_x_);
	}


	Matrix diag(const int &_n, const double &_e)
	{
		Matrix result(_n, _n, 0);

		for (int i = 0; i < _n; ++i)
		{
			result.set(i, i, _e);
		}

		return result;
	}


	Matrix operator+(const Matrix &_m1, const Matrix &_m2)
	{
		if (_m1.m_col != _m2.m_col)
		{
			throw "Error in operator(m1, m2)+: size of two matrix is not equal.";
		}

		Matrix result(_m1.m_row, _m2.m_col);
		double *pDst, *pSrc1;
		double *pSrc2 = _m2.m_data;

		if (1 == _m2.m_row)
		{
			for (int r = 0; r < _m1.m_row; ++r)
			{
				pDst = result.getRowPoint(r);
				pSrc1 = _m1.getRowPoint(r);
				for (int c = 0; c < _m1.m_col; ++c)
				{
					pDst[c] = pSrc1[c] + pSrc2[c];
				}
			}
		}
		else if (_m1.m_row == _m2.m_row)
		{
			pDst = result.m_data;
			pSrc1 = _m1.m_data;
			for (int i = 0; i < result.m_row*result.m_col; ++i)
			{
				pDst[i] = pSrc1[i] + pSrc2[i];
			}
		}

		return result;
	}


	Matrix& operator+=(Matrix &_m1, const Matrix &_m2)
	{
		if (_m1.m_row != _m2.m_row || _m1.m_col != _m2.m_col)
		{
			throw "Error in operator(m1, m2)+: size of two matrix is not equal.";
		}

		double *pDst = _m1.m_data;
		double *pSrc = _m2.m_data;
		for (int i = 0; i < _m1.m_row*_m1.m_col; ++i)
		{
			pDst[i] += pSrc[i];
		}

		return _m1;
	}


	Matrix operator+(const Matrix &_m, const double &_e)
	{
		Matrix result(_m.m_row, _m.m_col);

		double *pDst = result.m_data;
		double *pSrc = _m.m_data;
		for (int i = 0; i < result.m_row*result.m_col; ++i)
		{
			pDst[i] = pSrc[i] + _e;
		}

		return result;
	}


	Matrix operator+(const double &_e, const Matrix &_m)
	{
		return _m + _e;
	}


	Matrix& operator+=(Matrix &_m, const double &_e)
	{
		double *pDst = _m.m_data;
		for (int i = 0; i < _m.m_row*_m.m_col; ++i)
		{
			pDst[i] += _e;
		}

		return _m;
	}


	Matrix operator-(const Matrix &_m1, const Matrix &_m2)
	{
		return _m1 + (-1 * _m2);
	}


	Matrix& operator-=(Matrix &_m1, const Matrix &_m2)
	{
		return _m1 += -1 * _m2;
	}


	Matrix operator-(const Matrix &_m, const double &_e)
	{
		return _m + -_e;
	}


	Matrix operator-(const double &_e, const Matrix &_m)
	{
		return -1 * _m + _e;
	}


	Matrix& operator-=(Matrix &_m, const double &_e)
	{
		return _m += (-_e);
	}


	Matrix operator*(const Matrix &_m1, const Matrix &_m2)
	{
		if (_m1.m_col != _m2.m_row)
		{
			throw "Error in operator*(m1, m2): _m1.m_col != _m2.m_row.";
		}

		Matrix result(_m1.m_row, _m2.m_col, 0);

		for (int r = 0; r < _m1.m_row; ++r)
		{
			double *pR = result.getRowPoint(r);
			double *pM1 = _m1.getRowPoint(r);
			for (int c = 0; c < _m2.m_col; ++c)
			{
				double *pM2 = _m2.m_data + c;
				for (int k = 0; k < _m1.m_col; ++k)
				{
					pR[c] += pM1[k] * pM2[k*_m2.m_col];
				}
			}
		}

		return result;
	}


	Matrix operator*(const Matrix &_m, const double &_e)
	{
		if (NULL == _m.m_data)
		{
			throw "Error in operator*(m, e): NULL == _m.m_data.";
		}

		Matrix result(_m.m_row, _m.m_col);

		double *pR = result.m_data;
		double *pM = _m.m_data;
		for (int i = 0; i < _m.m_row*_m.m_col; ++i)
		{
			pR[i] = pM[i] * _e;
		}

		return result;
	}


	Matrix operator*(const double &_e, const Matrix &_m)
	{
		return _m*_e;
	}


	Matrix& operator*=(Matrix &_m, const double &_e)
	{
		if (NULL == _m.m_data)
		{
			throw "Error in operator*=(m, e): NULL == _m.m_data.";
		}

		double *pM = _m.m_data;
		for (int i = 0; i < _m.m_row*_m.m_col; ++i)
		{
			pM[i] = pM[i] * _e;
		}

		return _m;
	}


	Matrix operator/(const Matrix &_m, const double &_e)
	{
		if (isEqual(_e, 0))
		{
			throw "Error in operator/(m, e): e==0.";
		}

		return _m*(1 / _e);
	}


	Matrix operator/(const Matrix &_m1, const Matrix &_m2)
	{
		if (_m1.m_row != _m2.m_row || _m1.m_col != _m2.m_col)
		{
			throw "Error in operator/(m1, m2): _m1.m_row != _m2.m_row || _m1.m_col != _m2.m_col.";
		}

		Matrix result(_m1.m_row, _m1.m_col);

		double *pR = result.m_data;
		double *pM1 = _m1.m_data;
		double *pM2 = _m2.m_data;
		for (int i = 0; i < _m1.m_row*_m1.m_col; ++i)
		{
			if (isEqual(pM2[i], 0))
			{
				throw "Error in operator/(m1, m2): element==0.";
			}

			pR[i] = pM1[i] / pM2[i];
		}

		return result;
	}


	Matrix& operator/=(Matrix &_m, const double &_e)
	{
		if (isEqual(_e, 0))
		{
			throw "Error in operator/=(m, e): e==0.";
		}

		double *pM = _m.m_data;
		for (int i = 0; i < _m.m_row*_m.m_col; ++i)
		{
			pM[i] /= _e;
		}

		return _m;
	}

	
	Matrix operator/(const double &_e, const Matrix &_m)
	{
		Matrix result(_m.m_row, _m.m_col);

		double *pR = result.m_data;
		double *pM = _m.m_data;
		for (int i = 0; i < _m.m_row*_m.m_col; ++i)
		{
			if (isEqual(pM[i], 0))
			{
				throw "Error in operator/(e, m): m.e==0.";
			}

			pR[i] = _e / pM[i];
		}

		return result;
	}


	Matrix matrixRand(const int &_row, const int &_col, const double &_start, const double &_end)
	{
		if (_row <= 0 || _col <= 0)
		{
			throw "Error in matrixRand(): _row <= 0 || _col <= 0.";
		}
		if (_start >= _end)
		{
			throw "Error in matrixRand(): _start >= _end.";
		}
		
		Matrix result(_row, _col);

		double *pR = result.m_data;
		for (int i = 0; i < _row*_col; ++i)
		{
		//	srand((unsigned)time(NULL));
			*(pR + i) = rand() / double(RAND_MAX) * (_end - _start) + _start;
		}

		return result;
	}


	Matrix copyRow(const Matrix &_m, const int &_rowNum)
	{
		if (_rowNum <= 0)
		{
			throw "Error in copyRow(m, row): _rowNum <= 0.";
		}
		if (1 != _m.rows())
		{
			throw "Error in copyRow(m, row): 1 != _m.rows().";
		}

		Matrix result(_rowNum, _m.cols());
		double *pM = _m.getRowPoint(0);

		for (int r = 0; r < _rowNum; ++r)
		{
			double *pR = result.getRowPoint(r);

			memcpy((void*)pR, (void*)pM, _m.cols()*sizeof(double));
		}

		return result;
	}


	ostream& operator<<(ostream &_out, const Matrix &_m)
	{
		double *p = _m.m_data;
		_out << "[\n";

		for (int i = 0; i < _m.m_row; ++i)
		{
			_out << "  ";
			for (int j = 0; j < _m.m_col; ++j)
			{
				_out << *(p + i*_m.m_col + j) << ",\t ";
			}
			_out << "\n";
		}
		_out << "]\n";
		
		return _out;
	}

}	//	!mario
