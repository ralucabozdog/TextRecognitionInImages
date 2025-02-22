#pragma once

typedef struct {
	Mat_<int> matrix;
	int labelNumber;
}structTagImage;

typedef struct {
	int rmin;
	int rmax;
	int cmin;
	int cmax;
}structBoundingBox;

typedef struct {
	int rmin;
	int rmax;
}structRow;

typedef struct {
	int cmin;
	int cmax;
}structCol;

typedef struct {
	char matchChar;
	int matchPercentage;
}structMatch;