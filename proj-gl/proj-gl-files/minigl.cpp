/**
 * minigl.cpp
 * -------------------------------
 * Implement miniGL here.
 *
 * You may include minigl.h and any of the standard C++ libraries.
 * No other includes are permitted.  Other preprocessing directives
 * are also not permitted.  These requirements are strictly
 * enforced.  Be sure to run a test grading to make sure your file
 * passes the sanity tests.
 *
 * The behavior of the routines your are implenting is documented here:
 * https://www.opengl.org/sdk/docs/man2/
 * Note that you will only be implementing a subset of this.  In particular,
 * you only need to implement enough to pass the tests in the suite.
 */

#include "minigl.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>
#include <cstdio>
#include <iostream>
#include <list>

/*
 * Cpoy from vec.h
 */

#ifndef __vec__
#define __vec__

#include <cmath>
#include <iostream>
#include <cassert>

template<class T, int n> struct vec;
template<class T, int n> T dot(const vec<T, n>& u, const vec<T, n>& v);

template<class T, int n>
struct vec {
	T x[n];

	vec() {
		make_zero();
	}

	explicit vec(const T& a) {
		assert(n == 1);
		x[0] = a;
	}

	vec(const T& a, const T& b) {
		assert(n == 2);
		x[0] = a;
		x[1] = b;
	}

	vec(const T& a, const T& b, const T& c) {
		assert(n == 3);
		x[0] = a;
		x[1] = b;
		x[2] = c;
	}

	vec(const T& a, const T& b, const T& c, const T& d) {
		assert(n == 4);
		x[0] = a;
		x[1] = b;
		x[2] = c;
		x[3] = d;
	}

	template<class U>
	explicit vec(const vec<U, n>& v) {
		for (int i = 0; i < n; i++)
			x[i] = (T) v.x[i];
	}

	void make_zero() {
		for (int i = 0; i < n; i++)
			x[i] = 0;
	}

	vec& operator +=(const vec& v) {
		for (int i = 0; i < n; i++)
			x[i] += v.x[i];
		return *this;
	}

	vec& operator -=(const vec& v) {
		for (int i = 0; i < n; i++)
			x[i] -= v.x[i];
		return *this;
	}

	vec& operator *=(const vec& v) {
		for (int i = 0; i < n; i++)
			x[i] *= v.x[i];
		return *this;
	}

	vec& operator /=(const vec& v) {
		for (int i = 0; i < n; i++)
			x[i] /= v.x[i];
		return *this;
	}

	vec& operator *=(const T& c) {
		for (int i = 0; i < n; i++)
			x[i] *= c;
		return *this;
	}

	vec& operator /=(const T& c) {
		for (int i = 0; i < n; i++)
			x[i] /= c;
		return *this;
	}

	vec operator +() const {
		return *this;
	}

	vec operator -() const {
		vec r;
		for (int i = 0; i < n; i++)
			r[i] = -x[i];
		return r;
	}

	vec operator +(const vec& v) const {
		vec r;
		for (int i = 0; i < n; i++)
			r[i] = x[i] + v.x[i];
		return r;
	}

	vec operator -(const vec& v) const {
		vec r;
		for (int i = 0; i < n; i++)
			r[i] = x[i] - v.x[i];
		return r;
	}

	vec operator *(const vec& v) const {
		vec r;
		for (int i = 0; i < n; i++)
			r[i] = x[i] * v.x[i];
		return r;
	}

	vec operator /(const vec& v) const {
		vec r;
		for (int i = 0; i < n; i++)
			r[i] = x[i] / v.x[i];
		return r;
	}

	vec operator *(const T& c) const {
		vec r;
		for (int i = 0; i < n; i++)
			r[i] = x[i] * c;
		return r;
	}

	vec operator /(const T& c) const {
		vec r;
		for (int i = 0; i < n; i++)
			r[i] = x[i] / c;
		return r;
	}

	const T& operator[](int i) const {
		return x[i];
	}

	T& operator[](int i) {
		return x[i];
	}

	T magnitude_squared() const {
		return dot(*this, *this);
	}

	T magnitude() const {
		return sqrt(magnitude_squared());
	}

	// Be careful to handle the zero vector gracefully
	vec normalized() const {
		T mag = magnitude();
		if (mag)
			return *this / mag;
		vec r;
		r[0] = 1;
		return r;
	}
	;
};

template<class T, int n>
vec<T, n> operator *(const T& c, const vec<T, n>& v) {
	return v * c;
}

template<class T, int n>
T dot(const vec<T, n> & u, const vec<T, n> & v) {
	T r = 0;
	for (int i = 0; i < n; i++)
		r += u.x[i] * v.x[i];
	return r;
}

template<class T>
vec<T, 3> cross(const vec<T, 3> & u, const vec<T, 3> & v) {
	return vec<T, 3>(u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2],
			u[0] * v[1] - u[1] * v[0]);
}

template<class T, int n>
std::ostream& operator <<(std::ostream& out, const vec<T, n> & u) {
	for (int i = 0; i < n; i++) {
		if (i)
			out << ' ';
		out << u[i];
	}
	return out;
}

template<class T, int n>
std::istream& operator >>(std::istream& in, vec<T, n> & u) {
	for (int i = 0; i < n; i++) {
		in >> u[i];
	}
	return in;
}

#endif

/*
 * End of vec.h
 */

using namespace std;

/**
 * Standard macro to report errors
 */
inline void MGL_ERROR(const char* description) {
	printf("%s\n", description);
	exit(1);
}

MGLbool thisBegan = false;
MGLpoly_mode thisPoly;
typedef vec<MGLfloat, 3> vec3;
typedef vec<MGLfloat, 4> vec4;
vec3 thisColor(1, 1, 1);
MGLfloat thisFar = 1e10;
MGLfloat thisNear = -(1e10);
MGLsize thisWidth, thisHeight;

class CHL_Ver {
public:
	CHL_Ver() :
			theVec(0, 0, 0, 1) {
	}
	;
	CHL_Ver(const CHL_Ver& b) :
			theVec(b.theVec), color(b.color) {
	}
	;
	CHL_Ver(MGLfloat x, MGLfloat y, MGLfloat z, const vec3& c) :
			theVec(x, y, z, 1), color(c) {
	}
	;

	vec3 GetVec3() {
		return vec3(theVec[0], theVec[1], theVec[2]);
	}

	vec4 theVec;
	vec3 color;
};
vector<CHL_Ver> VertexChain_Buffer;
vector<CHL_Ver> Transed_VertexChain;

/**
 * Init:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
class MGLMatrix {
public:
	MGLMatrix() {
		//Init with I
		for (int i = 0; i < 16; ++i)
			m[i] = 0;
		m[0] = 1;
		m[5] = 1;
		m[10] = 1;
		m[15] = 1;
	}

	MGLMatrix(const MGLMatrix& o) {
		for (int i = 0; i < 16; ++i)
			m[i] = o.m[i];
	}

	MGLMatrix & operator =(const MGLMatrix & rhs) {
		//if (this == &rhs) {
		//	return *this;
		//}
		for (int i = 0; i < 16; ++i)
			m[i] = rhs.m[i];
		return *this;
	}

	MGLfloat m[16];
};

MGLmatrix_mode thisMode;
MGLMatrix thisMatrix[MGL_PROJECTION + 1];
list<MGLMatrix> PROJ_MatrixStack;
list<MGLMatrix> MV_MatrixStack;

class CHL_FrameBuf {
public:

	CHL_FrameBuf(MGLsize width, MGLsize height, MGLfloat near, MGLfloat far) :
			width(width), height(height), near(near), far(far) {
		for (MGLsize i = 0; i < width; ++i) {
			for (MGLsize j = 0; j < height; ++j) {
				MGLpixel black = Make_Pixel(0, 0, 0);
				MGLfloat far_away = far + 1;
				theBuf.push_back(black);
				zBuf.push_back(far_away);
			}
		}
	}
	;

	MGLpixel GetOne(MGLsize x, MGLsize y) {
		return theBuf[pos(x, y)];
	}

	void SetOne(MGLsize x, MGLsize y, MGLfloat z, MGLpixel p) {
		if ((z < far) && (z > near))
			if (z < zBuf[pos(x, y)]) {
				theBuf[pos(x, y)] = p;
				zBuf[pos(x, y)] = z;
			}
	}

	size_t pos(MGLsize x, MGLsize y) {
		if ((x >= width) || (y >= height) || (x < 0) || (y < 0)) {
			MGL_ERROR("FATAL: OUT OF RANGE!, Gen by CHL_FrameBuf::pos");
		}
		return y * width + x;
	}

	vector<MGLpixel> theBuf;
	vector<MGLfloat> zBuf;
	MGLsize width, height;
	MGLfloat near, far;
};

void FillTri(CHL_FrameBuf &buffer, CHL_Ver t1, CHL_Ver t2, CHL_Ver t3) {
	vec3 v1 = t1.GetVec3();
	vec3 v2 = t2.GetVec3();
	vec3 v3 = t3.GetVec3();

	MGLfloat bottom = min(v1[1], min(v2[1], v3[1])), left = min(v1[0],
			min(v2[0], v3[0])), top = max(v1[1], max(v2[1], v3[1])), right =
			max(v1[0], max(v2[0], v3[0]));

	vec3 b_a(v2 - v1);
	vec3 c_a(v3 - v1);
	vec3 c_b(v3 - v2);
	vec3 a_c(v1 - v3);
	vec3 n(cross(b_a, c_a));
	MGLfloat n_square = n.magnitude_squared();
	n /= n_square;

	// 0~1 => 0~255
	t1.color *= 255;
	t2.color *= 255;
	t3.color *= 255;

	if (left < 0)
		left = 0.1;		//floor
	if (right >= thisWidth)
		right = thisWidth - 1.5;		//ceil
	if (bottom < 0)
		bottom = 0.1;
	if (top >= thisHeight)
		top = thisHeight - 1.5;

	for (int i = floor(left); i < ceil(right); ++i) {
		for (int j = floor(bottom); j < ceil(top); ++j) {
			vec3 p(i + 0.5, j + 0.5, 0);
			vec3 na(cross(c_b, p - v2));
			vec3 nb(cross(a_c, p - v3));

			MGLfloat alpha = dot(n, na);
			MGLfloat beta = dot(n, nb);
			MGLfloat gamma = 1 - alpha - beta;

			if ((alpha >= 0) && (beta >= 0) && (gamma >= 0)) {
				//TODO color
				vec3 p = alpha * t1.color + beta * t2.color + gamma * t3.color;
				MGLfloat z = alpha * t1.theVec[2] + beta * t2.theVec[2]
						+ gamma * t3.theVec[2];
				buffer.SetOne(i, j, z,
						Make_Pixel(round(p[0]), round(p[1]), round(p[2])));
			}
		}
	}
}

CHL_Ver Sub_ViewPort(CHL_Ver input, MGLsize width, MGLsize height) {
//STEP 4 IN Vertex_Transformation
	input.theVec[0] = (input.theVec[0] * 0.5 + 0.5) * width;
	input.theVec[1] = (input.theVec[1] * 0.5 + 0.5) * height;
	//-1~1 => 0~1
	//input.theVec[2] = (1.0 + input.theVec[2]) * 0.5;
	return input;
}

void RasterizeTri(CHL_FrameBuf &buffer) {
	if (Transed_VertexChain.size() % 3 != 0)
		MGL_ERROR("FATAL: Wrong Vertex counts, Gen by RasterizeTri");

	cout<<"NumOfVer"<<Transed_VertexChain.size()<<endl;
	int k = 0;
	for (size_t i = 0; i < Transed_VertexChain.size(); i += 3) {
		k++;
		FillTri(buffer,
				Sub_ViewPort(Transed_VertexChain[i], buffer.width,
						buffer.height),
				Sub_ViewPort(Transed_VertexChain[i + 1], buffer.width,
						buffer.height),
				Sub_ViewPort(Transed_VertexChain[i + 2], buffer.width,
						buffer.height));
	}
	cout<<"NumOfTri"<<k<<endl;
}

vec4 Mat_Ver(MGLMatrix matrix, vec4 vertex) {
	vec4 ans;
	int n = 4, m = 4;
	for (int i = 0; i < n; i++) {
		ans[i] = 0;
		for (int k = 0; k < m; ++k) {
			ans[i] += matrix.m[k * n + i] * vertex[k];
		}
	}
	return ans;
}

/**
 * Read pixel data starting with the pixel at coordinates
 * (0, 0), up to (width,  height), into the array
 * pointed to by data.  The boundaries are lower-inclusive,
 * that is, a call with width = height = 1 would just read
 * the pixel at (0, 0).
 *
 * Rasterization and z-buffering should be performed when
 * this function is called, so that the data array is filled
 * with the actual pixel values that should be displayed on
 * the two-dimensional screen.
 */
void mglReadPixels(MGLsize width, MGLsize height, MGLpixel *data) {
	if (thisBegan)
		MGL_ERROR("GL_INVALID_OPERATION, Gen by mglReadPixels");

	thisWidth = width;
	thisHeight = height;
	CHL_FrameBuf thisBuf(width, height, thisNear, thisFar);
	RasterizeTri(thisBuf);
	for (MGLsize i = 0; i < width; i++) {
		for (MGLsize j = 0; j < height; ++j) {
			//DANGEROUS~
			data[thisBuf.pos(i, j)] = thisBuf.GetOne(i, j);
		}
	}

}

/**
 * Start specifying the vertices for a group of primitives,
 * whose type is specified by the given mode.
 */
void mglBegin(MGLpoly_mode mode) {
	if (thisBegan)
		MGL_ERROR("GL_INVALID_OPERATION, Gen by mglBegin");

	thisBegan = true;
	thisPoly = mode;
}

/**
 * Stop specifying the vertices for a group of primitives.
 */
void mglEnd() {
	if (!thisBegan)
		MGL_ERROR("GL_INVALID_OPERATION, Gen by mglEnd");

	VertexChain_Buffer.clear();
	thisBegan = false;
}

/**
 * Specify a two-dimensional vertex; the x- and y-coordinates
 * are explicitly specified, while the z-coordinate is assumed
 * to be zero.  Must appear between calls to mglBegin() and
 * mglEnd().
 */
void mglVertex2(MGLfloat x, MGLfloat y) {
	mglVertex3(x, y, 0);
}

/**
 * Specify a three-dimensional vertex.  Must appear between
 * calls to mglBegin() and mglEnd().
 */
void mglVertex3(MGLfloat x, MGLfloat y, MGLfloat z) {
	if (!thisBegan)
		MGL_ERROR("Wrong, Gen by mglVertex3");

	CHL_Ver newOne(x, y, z, thisColor);
	//https://www.khronos.org/opengl/wiki/Vertex_Transformation
	//Transform
	newOne.theVec = Mat_Ver(thisMatrix[MGL_MODELVIEW], newOne.theVec);
	//Clip
	newOne.theVec = Mat_Ver(thisMatrix[MGL_PROJECTION], newOne.theVec);
	//Normalize
	newOne.theVec = newOne.theVec / newOne.theVec[3];

	VertexChain_Buffer.push_back(newOne);

	if (thisPoly == MGL_TRIANGLES) {
		if (VertexChain_Buffer.size() == 3) {
			Transed_VertexChain.push_back(VertexChain_Buffer[0]);
			Transed_VertexChain.push_back(VertexChain_Buffer[1]);
			Transed_VertexChain.push_back(VertexChain_Buffer[2]);
			VertexChain_Buffer.clear();
		}
	} else {
		if (VertexChain_Buffer.size() == 4) {
			//First, CCW
			Transed_VertexChain.push_back(VertexChain_Buffer[0]);
			Transed_VertexChain.push_back(VertexChain_Buffer[1]);
			Transed_VertexChain.push_back(VertexChain_Buffer[2]);
			//Second, CCW
			Transed_VertexChain.push_back(VertexChain_Buffer[0]);
			Transed_VertexChain.push_back(VertexChain_Buffer[2]);
			Transed_VertexChain.push_back(VertexChain_Buffer[3]);
			VertexChain_Buffer.clear();
		}
	}

}

/**
 * Set the current matrix mode (modelview or projection).
 */
void mglMatrixMode(MGLmatrix_mode mode) {
	thisMode = mode;
}

/**
 * Push a copy of the current matrix onto the stack for the
 * current matrix mode.
 */
void mglPushMatrix() {
	if (thisBegan) {
		MGL_ERROR("GL_INVALID_OPERATION ,Gen by mglPushMatrix");
	}

	if (thisMode == MGL_MODELVIEW) {
		MV_MatrixStack.push_back(thisMatrix[MGL_MODELVIEW]);
	} else if (thisMode == MGL_PROJECTION) {
		PROJ_MatrixStack.push_back(thisMatrix[MGL_PROJECTION]);
	} else {
		MGL_ERROR("CRITICAL, gen by mglPushMatrix");
	}
}

/**
 * Pop the top matrix from the stack for the current matrix
 * mode.
 */
void mglPopMatrix() {
	if (thisBegan) {
		MGL_ERROR("GL_INVALID_OPERATION ,Gen by mglPopMatrix");
	}

	if (thisMode == MGL_MODELVIEW) {
		if (MV_MatrixStack.size() < 1) {
			MGL_ERROR("GL_STACK_UNDERFLOW ,Gen by mglPopMatrix");
		} else {
			thisMatrix[MGL_MODELVIEW] = *MV_MatrixStack.begin();
			MV_MatrixStack.pop_front();
		}
	} else if (thisMode == MGL_PROJECTION) {
		if (PROJ_MatrixStack.size() < 1) {
			MGL_ERROR("GL_STACK_UNDERFLOW ,Gen by mglPopMatrix");
		} else {
			thisMatrix[MGL_PROJECTION] = *PROJ_MatrixStack.begin();
			PROJ_MatrixStack.pop_front();
		}
	} else {
		MGL_ERROR("CRITICAL, gen by mglPopMatrix");
	}

}

/**
 * Replace the current matrix with the identity.
 */
void mglLoadIdentity() {
	for (int i = 0; i < 16; i++)
		thisMatrix[thisMode].m[i] = 0;
	thisMatrix[thisMode].m[0] = 1;
	thisMatrix[thisMode].m[5] = 1;
	thisMatrix[thisMode].m[10] = 1;
	thisMatrix[thisMode].m[15] = 1;
}

/**
 * Replace the current matrix with an arbitrary 4x4 matrix,
 * specified in column-major order.  That is, the matrix
 * is stored as:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
void mglLoadMatrix(const MGLfloat *matrix) {
	if (thisBegan) {
		MGL_ERROR("GL_INVALID_OPERATION, Gen by mglLoadMatrix");
	}
	for (int i = 0; i < 16; ++i) {
		thisMatrix[thisMode].m[i] = matrix[i];
	}
}

/**
 * Multiply the current matrix by an arbitrary 4x4 matrix,
 * specified in column-major order.  That is, the matrix
 * is stored as:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
void mglMultMatrix(const MGLfloat *matrix) {
	int n = 4, m = 4, p = 4;
	MGLMatrix old(thisMatrix[thisMode]);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < p; j++) {
			thisMatrix[thisMode].m[j * n + i] = 0;
			for (int k = 0; k < m; ++k) {
				thisMatrix[thisMode].m[j * n + i] += old.m[k * n + i]
						* matrix[j * p + k];
			}
		}
	}
}

/**
 * Multiply the current matrix by the translation matrix
 * for the translation vector given by (x, y, z).
 */
void mglTranslate(MGLfloat x, MGLfloat y, MGLfloat z) {
	if (thisBegan) {
		MGL_ERROR("GL_INVALID_OPERATION, Gen by mglTranslate");
	}

	MGLMatrix TranslateMatrix;
	TranslateMatrix.m[12] = x;
	TranslateMatrix.m[13] = y;
	TranslateMatrix.m[14] = z;
	mglMultMatrix(TranslateMatrix.m);

}

/**
 * Multiply the current matrix by the rotation matrix
 * for a rotation of (angle) degrees about the vector
 * from the origin to the point (x, y, z).
 */
void mglRotate(MGLfloat angle, MGLfloat x, MGLfloat y, MGLfloat z) {
	if (thisBegan) {
		MGL_ERROR("GL_INVALID_OPERATION, Gen by mglTranslate");
	}

	const MGLfloat pi = 3.1415926535897932384626433832795;
	vec3 t(vec3(x, y, z).normalized());
	x = t[0];
	y = t[1];
	z = t[2];
	MGLfloat c = cos(angle / 180.0 * pi);
	MGLfloat s = sin(angle / 180.0 * pi);

	MGLMatrix RotateMatrix;
	RotateMatrix.m[0] = x * x * (1 - c) + c;
	RotateMatrix.m[1] = y * x * (1 - c) + z * s;
	RotateMatrix.m[2] = x * z * (1 - c) - y * s;
	RotateMatrix.m[4] = x * y * (1 - c) - z * s;
	RotateMatrix.m[5] = y * y * (1 - c) + c;
	RotateMatrix.m[6] = y * z * (1 - c) + x * s;
	RotateMatrix.m[8] = x * z * (1 - c) + y * s;
	RotateMatrix.m[9] = y * z * (1 - c) - x * s;
	RotateMatrix.m[10] = z * z * (1 - c) + c;
	mglMultMatrix(RotateMatrix.m);

}

/**
 * Multiply the current matrix by the scale matrix
 * for the given scale factors.
 */
void mglScale(MGLfloat x, MGLfloat y, MGLfloat z) {
	if (thisBegan) {
		MGL_ERROR("GL_INVALID_OPERATION, Gen by mglScale");
	}

	MGLMatrix ScaleMatrix;
	ScaleMatrix.m[0] = x;
	ScaleMatrix.m[5] = y;
	ScaleMatrix.m[10] = z;
	mglMultMatrix(ScaleMatrix.m);

}

/**
 * Multiply the current matrix by the perspective matrix
 * with the given clipping plane coordinates.
 */
void mglFrustum(MGLfloat left, MGLfloat right, MGLfloat bottom, MGLfloat top,
		MGLfloat near, MGLfloat far) {

	//https://msdn.microsoft.com/en-us/library/windows/desktop/dd373537(v=vs.85).aspx
	MGLMatrix FrustumMatrix;
	FrustumMatrix.m[0] = (2.0 * near) / (right - left);
	FrustumMatrix.m[5] = (2.0 * near) / (top - bottom);
	FrustumMatrix.m[8] = (right + left) / (right - left);
	FrustumMatrix.m[9] = (top + bottom) / (top - bottom);
	FrustumMatrix.m[10] = (far + near) / (near - far);
	FrustumMatrix.m[11] = -1;
	FrustumMatrix.m[14] = (2.0 * far * near) / (near - far);
	FrustumMatrix.m[15] = 0;

	mglMultMatrix(FrustumMatrix.m);
}

/**
 * Multiply the current matrix by the orthographic matrix
 * with the given clipping plane coordinates.
 */
void mglOrtho(MGLfloat left, MGLfloat right, MGLfloat bottom, MGLfloat top,
		MGLfloat near, MGLfloat far) {
	if ((left == right) || (bottom == top) || (near == far)) {
		MGL_ERROR("GL_INVALID_VALUE, Gen by mglOrtho");
	}
	if (thisBegan) {
		MGL_ERROR("GL_INVALID_OPERATION");
	}

	thisNear = near;
	thisFar = far;

	MGLMatrix OrthoMatrix;
	OrthoMatrix.m[0] = 2.0 / (right - left);
	OrthoMatrix.m[5] = 2.0 / (top - bottom);
	OrthoMatrix.m[10] = (-2.0) / (far - near);
	OrthoMatrix.m[12] = (right + left) / (left - right);
	OrthoMatrix.m[13] = (top + bottom) / (bottom - top);
	OrthoMatrix.m[14] = (far + near) / (near - far);
	mglMultMatrix(OrthoMatrix.m);
}

/**
 * Set the current color for drawn shapes.
 */
void mglColor(MGLfloat red, MGLfloat green, MGLfloat blue) {
	thisColor[0] = red;
	thisColor[1] = green;
	thisColor[2] = blue;
}
