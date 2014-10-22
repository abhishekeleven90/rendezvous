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

	Coordinate_grid(int R, int C) {
		row = R;
		col = C;
	}

	int row, col;
};
