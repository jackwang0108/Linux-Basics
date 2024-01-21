#pragma once

#include <iostream>
#include <sstream>
#include <string>

struct Person {
	int id;
	std::string name;
};

std::ostream &operator<<(std::ostream &os, const Person p) {
	os << "Person No=" << p.id << ", name=" << p.name;
	return os;
}

std::string to_string(Person &p) {
	std::ostringstream oss;
	oss << p;
	return std::move(oss).str();
}