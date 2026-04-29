#pragma once

#include <immintrin.h>
#include <iostream>
#include <cmath>
#include "vect4f.hpp"

class alignas(16) mat4f
{
	public:
		vect4f       cols[4];

		/* Constructors */
		inline mat4f()
		{
			cols[0] = vect4f(1.0f, 0.0f, 0.0f, 0.0f);
			cols[1] = vect4f(0.0f, 1.0f, 0.0f, 0.0f);
			cols[2] = vect4f(0.0f, 0.0f, 1.0f, 0.0f);
			cols[3] = vect4f(0.0f, 0.0f, 0.0f, 1.0f);
		}

		inline mat4f(mat4f const &) = default;

		inline mat4f(vect4f const& a, vect4f const& b, vect4f const& c, vect4f const& d)
		{
			cols[0] = a;
			cols[1] = b;
			cols[2] = c;
			cols[3] = d;
		}

		/* Destructor */
		inline ~mat4f() = default;

		/* Operators */
		inline mat4f & operator=(mat4f const &) = default;

		inline mat4f operator+(mat4f const & rhs) const
		{
			return mat4f(
				cols[0] + rhs.cols[0],
				cols[1] + rhs.cols[1],
				cols[2] + rhs.cols[2],
				cols[3] + rhs.cols[3]
			);
		}

		inline mat4f operator-(mat4f const & rhs) const
		{
			return mat4f(
				cols[0] - rhs.cols[0],
				cols[1] - rhs.cols[1],
				cols[2] - rhs.cols[2],
				cols[3] - rhs.cols[3]
			);
		}

		inline vect4f operator*(vect4f const & v) const
		{
			__m128 e0 = _mm_set1_ps(v.getX());
			__m128 e1 = _mm_set1_ps(v.getY());
			__m128 e2 = _mm_set1_ps(v.getZ());
			__m128 e3 = _mm_set1_ps(v.getW());

			__m128 r = _mm_add_ps(
				_mm_add_ps(_mm_mul_ps(cols[0].getMM(), e0), _mm_mul_ps(cols[1].getMM(), e1)),
				_mm_add_ps(_mm_mul_ps(cols[2].getMM(), e2), _mm_mul_ps(cols[3].getMM(), e3))
			);

			return vect4f(r);
		}

		inline mat4f operator*(mat4f const & rhs) const
		{
			return mat4f(
				(*this) * rhs.cols[0],
				(*this) * rhs.cols[1],
				(*this) * rhs.cols[2],
				(*this) * rhs.cols[3]
			);
		}

		/* Accessors */
		inline vect4f & operator[](int i) { return cols[i]; }
		inline const vect4f & operator[](int i) const { return cols[i]; }

		/* Static Functions */
		static inline mat4f identity()
		{
			return mat4f();
		}

		static inline mat4f translate(vect4f const& t)
		{
			return mat4f(
				vect4f(1.0f, 0.0f, 0.0f, 0.0f),
				vect4f(0.0f, 1.0f, 0.0f, 0.0f),
				vect4f(0.0f, 0.0f, 1.0f, 0.0f),
				vect4f(t.getX(), t.getY(), t.getZ(), 1.0f)
			);
		}

		static inline mat4f rotate(float angle, vect4f const& axis)
		{
			vect4f u = axis.normalize();

			float c = std::cos(angle);
			float s = std::sin(angle);
			float t = 1.0f - c;

			float x = u.getX();
			float y = u.getY();
			float z = u.getZ();

			return mat4f(
				vect4f(t * x * x + c, t * x * y + s * z, t * x * z - s * y, 0.0f),
				vect4f(t * x * y - s * z, t * y * y + c, t * y * z + s * x, 0.0f),
				vect4f(t * x * z + s * y, t * y * z - s * x, t * z * z + c, 0.0f),
				vect4f(0.0f, 0.0f, 0.0f, 1.0f)
			);
		}

		static inline mat4f scale(vect4f const& s)
		{
			return mat4f(
				vect4f(s.getX(), 0.0f, 0.0f, 0.0f),
				vect4f(0.0f, s.getY(), 0.0f, 0.0f),
				vect4f(0.0f, 0.0f, s.getZ(), 0.0f),
				vect4f(0.0f, 0.0f, 0.0f, 1.0f)
			);
		}

		static inline mat4f perspective(float fov, float aspect, float near, float far)
		{
			float f = 1.0f / std::tan(fov * 0.5f);
			float nf = 1.0f / (near - far);

			return mat4f(
				vect4f(f / aspect, 0.0f, 0.0f, 0.0f),
				vect4f(0.0f, f, 0.0f, 0.0f),
				vect4f(0.0f, 0.0f, (far + near) * nf, -1.0f),
				vect4f(0.0f, 0.0f, (2.0f * far * near) * nf, 0.0f)
			);
		}

		static inline mat4f lookAt(vect4f const & eye, vect4f const & center, vect4f const & up)
		{
			vect4f f = (center - eye).normalize();
			vect4f s = f.cross(up).normalize();
			vect4f u = s.cross(f);

			return mat4f(
				vect4f(s.getX(), u.getX(), -f.getX(), 0.0f),
				vect4f(s.getY(), u.getY(), -f.getY(), 0.0f),
				vect4f(s.getZ(), u.getZ(), -f.getZ(), 0.0f),
				vect4f(-s.dot(eye), -u.dot(eye), f.dot(eye), 1.0f)
			);
		}

		inline mat4f inverse() const
		{
			__m128 A = _mm_shuffle_ps(cols[0].mm, cols[1].mm, _MM_SHUFFLE(1, 0, 1, 0));//[X0, Y0, X1, Y1]
			__m128 B = _mm_shuffle_ps(cols[2].mm, cols[3].mm, _MM_SHUFFLE(1, 0, 1, 0));//[X2, Y2, X3, Y3]
			__m128 C = _mm_shuffle_ps(cols[0].mm, cols[1].mm, _MM_SHUFFLE(3, 2, 3, 2));//[Z0, W0, Z1, W1]
			__m128 D = _mm_shuffle_ps(cols[2].mm, cols[3].mm, _MM_SHUFFLE(3, 2, 3, 2));//[Z2, W2, Z3, W3]

			//determinants blocs matrix 2x2
			__m128 detSub = _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(cols[0].mm, cols[2].mm, _MM_SHUFFLE(2, 0, 2, 0)), _mm_shuffle_ps(cols[1].mm, cols[3].mm, _MM_SHUFFLE(3, 1, 3, 1))),
				_mm_mul_ps(_mm_shuffle_ps(cols[0].mm, cols[2].mm, _MM_SHUFFLE(3, 1, 3, 1)), _mm_shuffle_ps(cols[1].mm, cols[3].mm, _MM_SHUFFLE(2, 0, 2, 0)))
			);

			//extract sub matrix determinants
			__m128 detA_vec = _mm_shuffle_ps(detSub, detSub, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 detB_vec = _mm_shuffle_ps(detSub, detSub, _MM_SHUFFLE(2, 2, 2, 2));
			__m128 detC_vec = _mm_shuffle_ps(detSub, detSub, _MM_SHUFFLE(1, 1, 1, 1));
			__m128 detD_vec = _mm_shuffle_ps(detSub, detSub, _MM_SHUFFLE(3, 3, 3, 3));

			//D#C et A#B
			__m128 D_C = _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(D, D, _MM_SHUFFLE(0, 3, 0, 3)), C),
				_mm_mul_ps(_mm_shuffle_ps(D, D, _MM_SHUFFLE(1, 2, 1, 2)), _mm_shuffle_ps(C, C, _MM_SHUFFLE(2, 3, 0, 1)))
			);
			__m128 A_B = _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(A, A, _MM_SHUFFLE(0, 3, 0, 3)), B),
				_mm_mul_ps(_mm_shuffle_ps(A, A, _MM_SHUFFLE(1, 2, 1, 2)), _mm_shuffle_ps(B, B, _MM_SHUFFLE(2, 3, 0, 1)))
			);

			// B.(D#C)
			__m128 B_DC = _mm_add_ps(
				_mm_mul_ps(_mm_shuffle_ps(B, B, _MM_SHUFFLE(1, 0, 1, 0)), _mm_shuffle_ps(D_C, D_C, _MM_SHUFFLE(2, 2, 0, 0))),
				_mm_mul_ps(_mm_shuffle_ps(B, B, _MM_SHUFFLE(3, 2, 3, 2)), _mm_shuffle_ps(D_C, D_C, _MM_SHUFFLE(3, 3, 1, 1)))
			);
			__m128 X_ = _mm_sub_ps(_mm_mul_ps(detD_vec, A), B_DC);
			//C.(A#B)
			__m128 C_AB = _mm_add_ps(
				_mm_mul_ps(_mm_shuffle_ps(C, C, _MM_SHUFFLE(1, 0, 1, 0)), _mm_shuffle_ps(A_B, A_B, _MM_SHUFFLE(2, 2, 0, 0))),
				_mm_mul_ps(_mm_shuffle_ps(C, C, _MM_SHUFFLE(3, 2, 3, 2)), _mm_shuffle_ps(A_B, A_B, _MM_SHUFFLE(3, 3, 1, 1)))
			);
			__m128 W_ = _mm_sub_ps(_mm_mul_ps(detA_vec, D), C_AB);

			//D.(A_B)#
			__m128 D_AB = _mm_sub_ps(
				_mm_mul_ps(D, _mm_shuffle_ps(A_B, A_B, _MM_SHUFFLE(0, 0, 3, 3))),
				_mm_mul_ps(_mm_shuffle_ps(D, D, _MM_SHUFFLE(1, 0, 3, 2)), _mm_shuffle_ps(A_B, A_B, _MM_SHUFFLE(2, 2, 1, 1)))
			);
			__m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB_vec, C), D_AB);
			//A.(D_C)#
			__m128 A_DC = _mm_sub_ps(
				_mm_mul_ps(A, _mm_shuffle_ps(D_C, D_C, _MM_SHUFFLE(0, 0, 3, 3))),
				_mm_mul_ps(_mm_shuffle_ps(A, A, _MM_SHUFFLE(1, 0, 3, 2)), _mm_shuffle_ps(D_C, D_C, _MM_SHUFFLE(2, 2, 1, 1)))
			);
			__m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC_vec, B), A_DC);

			//Trace
			__m128 tr = _mm_mul_ps(A_B, _mm_shuffle_ps(D_C, D_C, _MM_SHUFFLE(3, 1, 2, 0)));
			tr = _mm_hadd_ps(tr, tr);
			tr = _mm_hadd_ps(tr, tr);//_mm_hadd_ps(__m128 a, __m128 b) -> a0+a1 | a2+a3 | b0+b1 | b2+b3
			
			////detM = detA*detD + detB*detC - tr -- determinant global
			__m128 detM = _mm_add_ps(_mm_mul_ps(detA_vec, detD_vec), _mm_mul_ps(detB_vec, detC_vec));
			detM = _mm_sub_ps(detM, tr);

			//rDetM = 1.0 / detM
			const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
			__m128 rDetM = _mm_div_ps(adjSignMask, detM);

			X_ = _mm_mul_ps(X_, rDetM);
			Y_ = _mm_mul_ps(Y_, rDetM);
			Z_ = _mm_mul_ps(Z_, rDetM);
			W_ = _mm_mul_ps(W_, rDetM);

			return mat4f(
				_mm_shuffle_ps(X_, Z_, _MM_SHUFFLE(1, 3, 1, 3)),
				_mm_shuffle_ps(X_, Z_, _MM_SHUFFLE(0, 2, 0, 2)),
				_mm_shuffle_ps(Y_, W_, _MM_SHUFFLE(1, 3, 1, 3)),
				_mm_shuffle_ps(Y_, W_, _MM_SHUFFLE(0, 2, 0, 2))
			);
		}

};

/* Stream insertion operator */
inline std::ostream& operator<<(std::ostream& os, mat4f const & m)
{
	os << "\n";
	for (int row = 0; row < 4; ++row)
	{
		os << "| "
		<< m.cols[0][row] << " "
		<< m.cols[1][row] << " "
		<< m.cols[2][row] << " "
		<< m.cols[3][row] << " |\n";
	}
	return os;
}

inline mat4f operator*(float s, mat4f const & m)
{
	return mat4f(
		m.cols[0] * s,
		m.cols[1] * s,
		m.cols[2] * s,
		m.cols[3] * s
	);
}