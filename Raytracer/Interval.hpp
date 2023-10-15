#pragma once

struct Interval {
	double min, max;

	Interval() : min(+infinity), max(-infinity) {} // default interval is empty (infinity only defined because of where this is included)
	Interval(double _min, double _max) : min(_min), max(_max) {}
	auto contains(double x) const -> bool {
		return min <= x && x <= max;
	}
	auto surrounds(double x) const -> bool {
		return min < x && x < max;
	}
	auto clamp(double x) const -> double {
		if (x < this->min) return this->min;
		if (x > this->max) return this->max;
		return x;
	}
	static const Interval empty, universe;
};

const static Interval empty(+infinity, -infinity);
const static Interval universe(-infinity, +infinity);
