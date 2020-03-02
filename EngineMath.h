#pragma once
//TODO: Redef PIF to Pi_f
#include <math.h>
#define PI 3.14159265359f 


struct Vector2 {
	float x, y;
	float& operator[](int index) { return ((&x)[index]); }

	static Vector2 Origin(){
		return Vector2{0.0f, 0.0f};
	}

	
	static Vector2 IHat(){
		return Vector2{1.0f, 0.0f};
	}
}; 

struct Vector3 {
	float x, y, z;
	float& operator[](int index) { return ((&x)[index]); }

	static Vector3 Origin() {
		return Vector3{ 0.0f, 0.0f, 0.0f };
	}

};

struct Mat2 {
	float a, b,
		  c, d;

	float& operator[](int index) { return(&a)[index]; }

	static Mat2 Identity() {
		return Mat2{ 
					1.0f, 0.0f,
					0.0f, 1.0f
				};
	}
};

inline Mat2 operator*(Mat2 a, Mat2 b) {
	Mat2 result = { 0 };

	for (int row = 0; row < 2; ++row) {
		for (int column = 0; column < 2; ++column) {
			for (int sum = 0; sum < 2; ++sum) {
				result[2 * row + column] += a[2 * row + sum] * b[2 * sum + column];
			}
		}
	}

	return result;
}

struct Mat3 {
	float	a, b, c,
			d, e, f,
			g, h, i;

	float& operator[](int index) { return(&a)[index]; }
	
	static Mat3 Identity() {
		return Mat3{
					 1.0f, 0.0f, 0.0f,
					 0.0f, 1.0f, 0.0f,
					 0.0f, 0.0f, 1.0f
		};
	}
};

Mat3 Homogenize(Mat2 a) {
	return Mat3{
				 a.a, a.b,  0.0f,
				 a.c, a.d,  0.0f,
				 0.0f, .0f, 1.0f
			   };
}




inline Mat3 operator*(Mat3 a, Mat3 b) {
	Mat3 result = { 0 };

	for (int row = 0; row < 3; ++row) {
		for (int column = 0; column < 3; ++column) {
			for (int sum = 0; sum < 3; ++sum) {
				result[3 * row + column] += a[3 * row + sum] * b[3 * sum + column];
			}
		}
	}

	return result;
}


inline float Magnitude(Vector2 vec) {
	return sqrtf(vec.x*vec.x + vec.y*vec.y);
}

inline Vector2 Normalized(Vector2 vec){

}

inline void Normalize(Vector2& vec) {
	
}




inline Vector2 square(Vector2 a) {
	return { a.x*a.x, a.y*a.y };
}

inline float square(float f) {
	return f * f;
}

inline Vector2 
operator*(float s, Vector2 a) {
	Vector2 result;

	result.x = s*a.x;
	result.y = s*a.y;

	return result;
}

inline Vector2
operator*(Vector2 a, float s) {
	Vector2 result;

	result.x = s * a.x;
	result.y = s * a.y;

	return result;
}

inline float
operator*(Vector2 a, Vector2 b) {
	return a.x*b.x + a.y*b.y;
}

inline Vector2
operator+(Vector2 a, Vector2 b) {
	Vector2 result;

	result.x = a.x + b.x;
	result.y = a.y + b.y;

	return result;
}

inline Vector2
operator-(Vector2 a, Vector2 b) {
	Vector2 result;

	result.x = a.x - b.x;
	result.y = a.y - b.y;

	return result;
}

inline Vector2 normalized(Vector2 v) {
	Vector2 ret = {v.x, v.y};
	float l = sqrt(v.x*v.x + v.y*v.y);
	return 1/l*ret;
}

inline float clamp(float f) {
	if (fabs(f) <= .001f) {
		return 0.0f;
	}
	else {
		return f;
	}
}

inline Vector2 clamp(Vector2 a) {
	return Vector2{ clamp(a.x), clamp(a.y) };
}

inline Vector2
operator-(Vector2 a) {
	Vector2 result;

	result.x = -a.x;
	result.y = -a.y;
	return result;
}

inline Vector2&
operator*=(Vector2& a, float s) {
	a = s * a;

	return a;
}

inline Vector2&
operator+=(Vector2& a, Vector2 b) {
	a = a + b;

	return a;
}

inline Vector2&
operator-=(Vector2& a, Vector2 b) {
	a = a - b;

	return a;
}


inline float Inner(Vector2 a, Vector2 b) {
	return a.x*b.x + a.y*b.y;
}

inline //Assumes a < b
bool isWithinExclusive(int value, int a, int b) {

	//Research NOR and 2's Complement method
	
	return((unsigned)(value - (a+1)) < (unsigned)(b - a));
}


inline //Assumes a < b
bool isWithinInclusive(int value, int a, int b) {

	//Research NOR and 2's Complement method

	return((unsigned)(value - a) <= (unsigned)(b - a));
}



struct Transform2D {

	//Cordinate Transform

		// Note: First the unit square 
		//		is scaled to fit width
		//	    and height then the 
		//		user scale is applied.
	float scaleX, scaleY;
	float skewX, skewY;

	// Note: Rotation relative to the center of the square.
	float orientation;

	//Relative Transforms

	Vector2 translation;
	Mat3 matrix;

};