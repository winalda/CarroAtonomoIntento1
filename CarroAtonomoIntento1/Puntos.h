#pragma once

class Puntos {

	int i;
	int j;

public:

	Puntos(int i,int j) {
		this->i = i;
		this->j = j;
	}

	int getI(void) { return this->i; }
	int getJ(void) { return this->j; }

};