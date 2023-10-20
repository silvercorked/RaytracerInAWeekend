#pragma once

#include "common.hpp"
#include "STBImageHelper.hpp"
#include "Color.hpp"


struct Texture {
	virtual ~Texture() = default;

	virtual auto value(double u, double v, const Point3& p) const -> Color = 0;
};

class SolidColor : public Texture {
	Color colorValue;

public:
	SolidColor(Color c) : colorValue(c) {};
	SolidColor(double red, double green, double blue) : SolidColor(Color(red, green, blue)) {}

	auto value(double u, double v, const Point3& p) const -> Color override {
		return this->colorValue;
	}
};

class CheckerTexture : public Texture {
	double invScale;
	shared_ptr<Texture> even;
	shared_ptr<Texture> odd;

public:
	CheckerTexture(double _scale, shared_ptr<Texture> _even, shared_ptr<Texture> _odd) :
		invScale(1.0 / _scale),
		even(_even),
		odd(_odd) {}
	CheckerTexture(double _scale, Color c1, Color c2) :
		invScale(1.0 / _scale),
		even(make_shared<SolidColor>(c1)),
		odd(make_shared<SolidColor>(c2)) {}

	auto value(double u, double v, const Point3& p) const -> Color override {
		auto xInt = static_cast<int>(std::floor(invScale * p.x()));
		auto yInt = static_cast<int>(std::floor(invScale * p.y()));
		auto zInt = static_cast<int>(std::floor(invScale * p.z()));
		bool isEven = (xInt + yInt + zInt) % 2 == 0;
		return isEven ? this->even->value(u, v, p) : this->odd->value(u, v, p);
	}
};

class ImageTexture : public Texture {
	AlexSTBImage image;

public:
	ImageTexture(const char* filename) : image(filename) {}

	auto value(double u, double v, const Point3& p) const -> Color override {
		if (this->image.height() <= 0) return Color(0, 1, 1); // cyan debug aid for no texture data
		// Clamp input texture coordinates to [0, 1] x [1, 0]
		u = Interval(0, 1).clamp(u);
		v = 1.0 - Interval(0, 1).clamp(v); // Flip V to represent convention of inverted y image drawing

		auto i = static_cast<int>(u * this->image.width());
		auto j = static_cast<int>(v * this->image.height());
		auto pixel = this->image.pixelData(i, j);
		auto colorScale = 1.0 / 255.0;
		return Color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
	}
};

