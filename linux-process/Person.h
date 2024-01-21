#pragma once

#include <iostream>
#include <sstream>

struct Person {
	int no;
	char name[51];
};

std::ostream &operator<<(std::ostream &os, const Person &p) {
	os << "Person No=" << p.no << ", name=" << p.name;
	return os;
}

std::string to_string(Person &p) {
	std::ostringstream oss;
	oss << p;
	return std::move(oss).str();
}