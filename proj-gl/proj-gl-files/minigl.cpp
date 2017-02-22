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

using namespace std;

/**
 * Standard macro to report errors
 */
inline void MGL_ERROR(const char* description)
{
	printf("%s\n", description);
	exit(1);
}

MGLbool thisBegan=false;
MGLpoly_mode thisPoly;
class CHL_Ver
{
public:
	CHL_Ver() :
			x(0), y(0), z(0), w(1)
	{
	}
	;
	CHL_Ver(const CHL_Ver& b) :
			x(b.x), y(b.y), z(b.z), w(b.w)
	{
	}
	;
	CHL_Ver(MGLfloat x, MGLfloat y, MGLfloat z) :
			x(x), y(y), z(z), w(1)
	{
	}
	;
	MGLfloat x, y, z, w;
};
vector<CHL_Ver> VertexChain_Buffer;
vector<CHL_Ver> Transed_VertexChain;

CHL_Ver Cross(CHL_Ver u, CHL_Ver v)
{
	return CHL_Ver(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z,
			u.x * v.y - u.y * v.x);
}

CHL_Ver Subtract(CHL_Ver u, CHL_Ver v)
{
	return CHL_Ver(u.x - v.x, u.y - v.y, u.z - v.z);
}

MGLfloat Dot(CHL_Ver u, CHL_Ver v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

class CHL_FrameBuf
{
public:

	CHL_FrameBuf(MGLsize width, MGLsize height) :
			width(width), height(height)
	{
		for (MGLsize i = 0; i < width; ++i)
		{
			for (MGLsize j = 0; j < height; ++j)
			{
				MGLpixel black = Make_Pixel(0, 0, 0);
				MGLfloat far_away = -(1e8);
				theBuf.push_back(black);
				zBuf.push_back(far_away);
			}
		}
	}
	;

	MGLpixel GetOne(MGLsize x, MGLsize y)
	{
		return theBuf[pos(x, y)];
	}

	void SetOne(MGLsize x, MGLsize y, MGLfloat z, MGLpixel p)
	{
		cout<<x<<","<<y<<","<<z<<endl;
		if (z > zBuf[pos(x, y)])
		{
			cout<<"HIT"<<endl;
			theBuf[pos(x, y)] = p;
			zBuf[pos(x, y)] = z;
		}
	}

	size_t pos(MGLsize x, MGLsize y)
	{
		if ((x >= width) || (y >= height))
		{
			MGL_ERROR("FATAL: OUT OF RANGE!, Gen by CHL_FrameBuf::pos");
		}
		return x * height + y;
	}

	vector<MGLpixel> theBuf;
	vector<MGLfloat> zBuf;
	MGLsize width, height;
};

void FillTri(CHL_FrameBuf &buffer, CHL_Ver t1, CHL_Ver t2, CHL_Ver t3)
{
	cout<<"FillTri"<<endl;
	MGLfloat bottom = t1.y, left = t1.x, top = t1.y, right = t1.x;
	cout<<t2.x<<","<<t2.y<<","<<t3.x<<","<<t3.y<<endl;
	if (t2.x < left)
		left = t2.x;
	if (t3.x < left)
		left = t3.x;
	if (t2.x > right)
		right = t2.x;
	if (t3.x > right)
		right = t3.x;
	if (t2.y < bottom)
		bottom = t2.y;
	if (t3.y < bottom)
		bottom = t3.y;
	if (t2.y > top)
		top = t2.y;
	if (t3.y > top)
		top = t3.y;

	CHL_Ver b_a(Subtract(t2, t1));
	CHL_Ver c_a(Subtract(t3, t1));
	CHL_Ver c_b(Subtract(t3, t2));
	CHL_Ver a_c(Subtract(t1, t3));
	CHL_Ver n(Cross(b_a, c_a));
	MGLfloat n_square = Dot(n, n);
	n.x = n.x / n_square;
	n.y = n.y / n_square;
	n.z = n.z / n_square;
	cout<<left<<","<<right<<","<<bottom<<","<<top<<endl;
	for (int i = floor(left); i < ceil(right); ++i)
	{
		for (int j = floor(bottom); j < ceil(top); ++j)
		{
			CHL_Ver p(i + 0.5, j + 0.5, 0);
			CHL_Ver na(Cross(c_b, Subtract(p, t2)));
			CHL_Ver nb(Cross(a_c, Subtract(p, t3)));

			MGLfloat alpha = Dot(n, na);
			MGLfloat beta = Dot(n, nb);
			MGLfloat gamma = 1 - alpha - beta;

			cout<<alpha<<","<<beta<<","<<gamma<<endl;
			if ((alpha >= 0) && (beta >= 0) && (gamma >= 0))
			{
				//TODO color
				buffer.SetOne(i, j, 0, Make_Pixel(255, 255, 255));
			}
		}
	}
}

CHL_Ver Sub_ViewPort(CHL_Ver input, MGLsize width, MGLsize height)
{
//STEP 4 IN Vertex_Transformation
	cout<<input.x<<","<<input.y<<","<<input.z<<endl;
	input.x = (input.x * 0.5 + 0.5) * width;
	input.y = (input.y * 0.5 + 0.5) * height;
	input.z = (1.0 + input.z) * 0.5;
	return input;
}

void RasterizeTri(CHL_FrameBuf &buffer)
{
	cout<<"D"<<endl;
	if (Transed_VertexChain.size() % 3 != 0)
		MGL_ERROR("FATAL: Wrong Vertex counts, Gen by RasterizeTri");

	for (size_t i = 0; i < Transed_VertexChain.size(); i += 3)
	{
		FillTri(buffer,
				Sub_ViewPort(Transed_VertexChain[i], buffer.width,
						buffer.height),
				Sub_ViewPort(Transed_VertexChain[i + 1], buffer.width,
						buffer.height),
				Sub_ViewPort(Transed_VertexChain[i + 2], buffer.width,
						buffer.height));
	}

}

CHL_Ver Transform(CHL_Ver input)
{
	return input;
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
void mglReadPixels(MGLsize width, MGLsize height, MGLpixel *data)
{
	cout<<"mglReadPixels"<<endl;
	if (thisBegan)
		MGL_ERROR("GL_INVALID_OPERATION, Gen by mglReadPixels");

	CHL_FrameBuf thisBuf(width, height);
	cout<<"B4"<<endl;
	RasterizeTri(thisBuf);
	cout<<"AFT"<<endl;
	for (MGLsize i = 0; i < width; i++)
	{
		for (MGLsize j = 0; j < height; ++j)
		{
			//DANGEROUS~
			data[thisBuf.pos(i, j)] = thisBuf.GetOne(i, j);
		}
	}

}

/**
 * Start specifying the vertices for a group of primitives,
 * whose type is specified by the given mode.
 */
void mglBegin(MGLpoly_mode mode)
{
	cout<<"What"<<endl;
	if (thisBegan)
		MGL_ERROR("GL_INVALID_OPERATION, Gen by mglBegin");

	thisBegan = true;
	thisPoly = mode;
}

/**
 * Stop specifying the vertices for a group of primitives.
 */
void mglEnd()
{
	cout<<"end"<<endl;
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
void mglVertex2(MGLfloat x, MGLfloat y)
{
	mglVertex3(x, y, 0);
}

/**
 * Specify a three-dimensional vertex.  Must appear between
 * calls to mglBegin() and mglEnd().
 */
void mglVertex3(MGLfloat x, MGLfloat y, MGLfloat z)
{
	if (!thisBegan)
		MGL_ERROR("Wrong, Gen by mglVertex3");
	cout<<"mglVertex3 "<<x<<","<<y<<endl;
//Trans

	VertexChain_Buffer.push_back(CHL_Ver(x, y, z));

	if (thisPoly == MGL_TRIANGLES)
	{
		if (VertexChain_Buffer.size() == 3)
		{
			cout<<VertexChain_Buffer[0].x<<endl;
			Transed_VertexChain.push_back(VertexChain_Buffer[0]);
			Transed_VertexChain.push_back(VertexChain_Buffer[1]);
			Transed_VertexChain.push_back(VertexChain_Buffer[2]);
			VertexChain_Buffer.clear();
		}
	}

}

/**
 * Set the current matrix mode (modelview or projection).
 */
void mglMatrixMode(MGLmatrix_mode mode)
{
}

/**
 * Push a copy of the current matrix onto the stack for the
 * current matrix mode.
 */
void mglPushMatrix()
{
}

/**
 * Pop the top matrix from the stack for the current matrix
 * mode.
 */
void mglPopMatrix()
{
}

/**
 * Replace the current matrix with the identity.
 */
void mglLoadIdentity()
{
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
void mglLoadMatrix(const MGLfloat *matrix)
{
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
void mglMultMatrix(const MGLfloat *matrix)
{
}

/**
 * Multiply the current matrix by the translation matrix
 * for the translation vector given by (x, y, z).
 */
void mglTranslate(MGLfloat x, MGLfloat y, MGLfloat z)
{
}

/**
 * Multiply the current matrix by the rotation matrix
 * for a rotation of (angle) degrees about the vector
 * from the origin to the point (x, y, z).
 */
void mglRotate(MGLfloat angle, MGLfloat x, MGLfloat y, MGLfloat z)
{
}

/**
 * Multiply the current matrix by the scale matrix
 * for the given scale factors.
 */
void mglScale(MGLfloat x, MGLfloat y, MGLfloat z)
{
}

/**
 * Multiply the current matrix by the perspective matrix
 * with the given clipping plane coordinates.
 */
void mglFrustum(MGLfloat left, MGLfloat right, MGLfloat bottom, MGLfloat top,
		MGLfloat near, MGLfloat far)
{
}

/**
 * Multiply the current matrix by the orthographic matrix
 * with the given clipping plane coordinates.
 */
void mglOrtho(MGLfloat left, MGLfloat right, MGLfloat bottom, MGLfloat top,
		MGLfloat near, MGLfloat far)
{
}

/**
 * Set the current color for drawn shapes.
 */
void mglColor(MGLfloat red, MGLfloat green, MGLfloat blue)
{
}
