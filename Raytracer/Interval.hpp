#pragma once

struct Interval {
	double min, max;

	Interval() : min(+infinity), max(-infinity) {} // default interval is empty (infinity only defined because of where this is included)
	Interval(double _min, double _max) : min(_min), max(_max) {}
	Interval(const Interval& a, const Interval& b) : min{ fmin(a.min, b.min) }, max{ fmax(a.max, b.max) } {}
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
	auto size() const -> double {
		return max - min;
	}
	auto expand(double delta) const -> Interval {
		auto padding = delta / 2;
		return Interval(min - padding, max + padding);
	}

	static const Interval empty, universe;
};

const static Interval empty(+infinity, -infinity);
const static Interval universe(-infinity, +infinity);

auto operator+(const Interval& ival, double displacement) -> Interval {
	return Interval(ival.min + displacement, ival.max + displacement);
}
auto operator+(double displacement, const Interval& ival) -> Interval {
	return ival + displacement;
}
