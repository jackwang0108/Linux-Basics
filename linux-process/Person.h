#pragma once

#include <iostream>

struct Person {
	int no;
	char name[51];
};

std::ostream &operator<<(std::ostream &os, const Person &p) {
	os << "Person No=" << p.no << ", name=" << p.name;
	return os;
}