#ifndef CUSTOM_VECTOR_STRUCT_H
#define CUSTOM_VECTOR_STRUCT_H

struct Coordinate_openGl {
public:

	Coordinate_openGl(float X, float Y, float Z) {
		x = X;
		y = Y;
		z = Z;
	}

	Coordinate_openGl(float X, float Y) {
		x = X;
		y = Y;
	}

	float x, y, z;
};

struct Coordinate_grid {
public:

	Coordinate_grid() {

	}

	Coordinate_grid(int r, int c) {
		row = r;
		col = c;
	}

	int row, col;
};

struct Coordinate_window {
public:

	Coordinate_window(float X, float Y) {
		x = X;
		y = Y;
	}

	int x, y;
};

struct MyTexture {
public:

	MyTexture() {

	}

	MyTexture(GLuint texId, double u, double v) {
		textureId = texId;
		u3 = u;
		v3 = v;
	}

	GLuint textureId;
	double u3;
	double v3;
};

#endif
