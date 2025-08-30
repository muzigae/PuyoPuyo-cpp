#pragma once

class cellData
{
public:
	cellData();
	cellData(int x, int y);

	int& operator[](const int i);
	int operator[](const int i) const;

private:
	int coor[2]; // ÁÂÇ¥ Á¤º¸
};