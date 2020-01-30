/*
 *  SillyClass.cpp
 *  Templates
 *
 *  Created by jean-yves hervé on 2009/03/11.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "SillyClass.h"

#include <sstream>
using namespace std;

SillyClass::SillyClass(int m, int n)
	:	m_(m),
		n_(n) {
}
		
SillyClass::~SillyClass(void) {
}

int SillyClass::getM(void) const {
	return m_;
}
		
int SillyClass::getN(void) const {
	return n_;
}
		
void SillyClass::setM(int m) {
	m_ = m;
}
		
void SillyClass::setN(int n) {
	n_ = n;
}
		

string SillyClass::toString(void) const {
	string s;
	stringstream sstr;
	
	sstr << "(" << m_ << "," << n_ << ")";
	sstr >> s;
	return s;
}

bool SillyClass::operator >= (const SillyClass& b) const {
	return (m_+n_) >= (b.m_ + b.n_);
}

bool operator < (const SillyClass& a, const SillyClass& b) {
	return (a.getM()+a.getN()) < (b.getM()+b.getN()) ? true : false;
}