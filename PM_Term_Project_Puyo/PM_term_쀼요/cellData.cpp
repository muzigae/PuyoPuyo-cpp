#include "cellData.h"

cellData::cellData() {
	coor[0] = 0;
	coor[1] = 0;
}

cellData::cellData(int x, int y) {
	coor[0] = x;
	coor[1] = y;
}

int& cellData::operator[](const int i) { return coor[i]; }
int cellData::operator[](const int i) const { return coor[i]; }