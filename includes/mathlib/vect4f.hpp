#pragma once

#include <immintrin.h>
#include <iostream>
#include <cmath>

class mat4f;

class alignas(16) vect4f
{
	friend class mat4f;
	public:
	union {
		__m128	mm;
		struct	{ float x, y, z, w;};
		float	data[4];
	};
		/* Constructors */
			inline				vect4f() : mm(_mm_setzero_ps()) {}
			inline				vect4f(vect4f const& rhs) = default;
			inline				vect4f(float x, float y, float z, float w = 0.0f) : mm(_mm_set_ps(w, z, y, x)) {}
			inline				vect4f(__m128 m) : mm(m) {}
		/* Destructor */
			inline				~vect4f() = default;
		/* Getters */
			inline __m128		getMM() const { return mm; }
			inline float		getX() const { return x; }
			inline float		getY() const { return y; }
			inline float		getZ() const { return z; }
			inline float		getW() const { return w; }

		/* Operators */
			inline vect4f &		operator=(vect4f const& rhs) = default;
			inline vect4f		operator+(vect4f const& rhs) const { return vect4f(_mm_add_ps(this->mm, rhs.mm)); }
			inline vect4f		operator-(vect4f const& rhs) const { return vect4f(_mm_sub_ps(this->mm, rhs.mm)); }
			inline vect4f		operator*(vect4f const& rhs) const { return vect4f(_mm_mul_ps(this->mm, rhs.mm)); }
			inline vect4f		operator*(float x) const { return vect4f(_mm_mul_ps(this->mm , _mm_set1_ps(x))); }
			inline vect4f		operator/(vect4f const& rhs) const { return vect4f(_mm_div_ps(this->mm, rhs.mm)); }
			inline vect4f		operator/(float x) const { return vect4f(_mm_div_ps(this->mm, _mm_set1_ps(x))); }
			inline void*		operator new(size_t size) { return _mm_malloc(size, 16); }
			inline void			operator delete(void *prt) { return _mm_free(prt); }
			inline void*		operator new[](size_t size) { return _mm_malloc(size, 16); }
			inline void			operator delete[](void *ptr) { _mm_free(ptr); }
			inline vect4f &		operator +=(vect4f const& rhs) { this->mm = _mm_add_ps(this->mm, rhs.mm); return *this; }
			inline vect4f &		operator *=(vect4f const& rhs) { this->mm = _mm_mul_ps(this->mm, rhs.mm); return *this; }
			inline vect4f &		operator -=(vect4f const& rhs) { this->mm = _mm_sub_ps(this->mm, rhs.mm); return *this; }
			inline vect4f &		operator *=(float x) { this->mm = _mm_mul_ps(this->mm, _mm_set1_ps(x)); return *this; }
			inline vect4f &		operator /=(float x) { this->mm = _mm_div_ps(this->mm, _mm_set1_ps(x)); return *this; }
			inline float&		operator[](int i) { return data[i]; }
			inline const float&	operator[](int i) const { return data[i]; }

		/* Functions */
			float				dot(vect4f const &) const;
			float				length() const;
			float				lengthSq() const;
			vect4f				normalize() const;
			vect4f				cross(vect4f const &) const;
};

/* Stream insertion operator */
inline std::ostream&	operator<<(std::ostream& os, vect4f const & v)
{
	os << "(" << v.getX() << ", " << v.getY() << ", " << v.getZ() << ", " << v.getW() << ")";
	return os;
}

inline vect4f	operator*(float s, vect4f const & v)
{
	return v * s;
}


/*----------------------------------------------------------------------------------------*/
/* Functions */

/*
	dot(a, b) = a.x *  b.x + a.y * b.y + a.z * b.z + a.w * b.w (1)
	(1) <=> dot(a, b) = X + Y + Z + W
	
	_mm_mul_ps(a, b) = a.x * b.x | a.y * b.y | a.z * b.z | a.w * b.w (2)
	(2) <=> _mm_mul_ps(a, b) = X | Y | Z | W
	(2) <=> _mm_mul_ps(a, b) = RES
	(2) <=> RES = X | Y | Z | W
	(2) <=> __mm_movehdup_ps(RES) = Y | Y | W | W
	(2) <=> SHUF = Y | Y | W | W

	(3) <=> SUM = __mm_add_ps(RES, SHUF)
	(3) <=> SUM = X + Y | Y + Y | Z + W | W + W

	_mm_movehl_ps(SHUF, SUM) = Y | Y | X + Y | Y + Y (4)
	(4) SHUF2 = Z + W | W + W | W | W

	(5) _mm_add_ss(SUM, SHUF2) = X + Y + Z + W | Y + Y | Z + W | W + W

	(6) _mm_cvtss_f32((5)) = X + Y + Z + W;
*/
inline float	vect4f::dot(vect4f const & rhs) const
{
	__m128 mul = _mm_mul_ps(mm, rhs.mm);
	__m128 shuf = _mm_movehdup_ps(mul);//_mm_movehdup_ps(v) => v.y | v.y | v.w | v.w
	__m128 sums = _mm_add_ps(mul, shuf);
	
	shuf = _mm_movehl_ps(shuf, sums);//_mm_moveldup_ps(a, b) =>  b.z | b.w | a.z | a.w 
	sums = _mm_add_ss(sums, shuf);//_mm_add_ss(a, b) = a.x + b.x | a.y | a.z | a.w

	return _mm_cvtss_f32(sums);
}

inline float	vect4f::length() const
{
	return std::sqrt(this->dot(this->mm));
}

inline float	vect4f::lengthSq() const
{
	return this->dot(this->mm);
}

/* Newton-Raphson */
inline vect4f	vect4f::normalize() const
{
	__m128 va = _mm_set1_ps(dot(this->mm));
	__m128 v0 = _mm_rsqrt_ps(va); 

	static const __m128 v1 = _mm_set1_ps(0.5f);
	static const __m128 v2 = _mm_set1_ps(1.5f);

	__m128 vnr = _mm_mul_ps(v0, _mm_sub_ps(v2, _mm_mul_ps(v1, _mm_mul_ps(va, _mm_mul_ps(v0, v0)))));

	return vect4f(_mm_mul_ps(this->mm, vnr));
}

inline vect4f	vect4f::cross(vect4f const & rhs) const
{
	return vect4f(_mm_sub_ps(_mm_mul_ps(
										_mm_shuffle_ps(this->mm, this->mm, _MM_SHUFFLE(3, 0, 2, 1)),
										_mm_shuffle_ps (rhs.mm, rhs.mm, _MM_SHUFFLE(3, 1, 0, 2))
									),
							_mm_mul_ps(
										_mm_shuffle_ps(this->mm, this->mm, _MM_SHUFFLE(3, 1, 0, 2)),
										_mm_shuffle_ps(rhs.mm, rhs.mm, _MM_SHUFFLE(3, 0, 2, 1))
									)));
}