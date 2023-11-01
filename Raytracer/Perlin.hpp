#pragma once

#include "common.hpp"

class Perlin {
	static const int pointCount = 256;
	Vec3* ranVec; // rather than floats, Perlin uses random unit vectors
	int* permX;
	int* permY;
	int* permZ;

public:
	Perlin() {
		this->ranVec = new Vec3[Perlin::pointCount];
		for (int i = 0; i < Perlin::pointCount; i++) {
			this->ranVec[i] = unitVector(Vec3::random(-1, 1));
		}
		this->permX = Perlin::generatePerm();
		this->permY = Perlin::generatePerm();
		this->permZ = Perlin::generatePerm();
	}
	~Perlin() {
		delete[] ranVec;
		delete[] permX;
		delete[] permY;
		delete[] permZ;
	}
	auto noise(const Point3& p) const -> double {
		auto u = p.x() - floor(p.x());
		auto v = p.y() - floor(p.y());
		auto w = p.z() - floor(p.z());
		auto i = static_cast<int>(floor(p.x()));
		auto j = static_cast<int>(floor(p.y()));
		auto k = static_cast<int>(floor(p.z()));
		Vec3 c[2][2][2];

		for (int di = 0; di < 2; di++) {
			for (int dj = 0; dj < 2; dj++) {
				for (int dk = 0; dk < 2; dk++) {
					c[di][dj][dk] = this->ranVec[
						this->permX[(i + di) & 255]
						^ this->permY[(j + dj) & 255]
						^ this->permZ[(k + dk) & 255]
					];
				}
			}
		}
		return Perlin::perlinInterpolate(c, u, v, w);
	}
	auto turbulence(const Point3& p, int depth = 7) const -> double {
		auto accum = 0.0;
		auto tempP = p;
		auto weight = 1.0;
		for (int i = 0; i < depth; i++) {
			accum += weight * noise(tempP);
			weight *= 0.5;
			tempP *= 2;
		}
		return fabs(accum);
	}
private:
	static auto generatePerm() -> int* {
		auto p = new int[Perlin::pointCount];
		for (int i = 0; i < Perlin::pointCount; i++) {
			p[i] = i;
		}
		Perlin::permute(p, Perlin::pointCount);
		return p;
	}
	static auto permute(int* p, int n) -> void {
		for (int i = n - 1; i > 0; i--) {
			int target = randomInt(0, i);
			int tmp = p[i];	// swap between i and target
			p[i] = p[target];
			p[target] = tmp;
		}
	}
	/* c contains rectangle coordinates (https://en.wikipedia.org/wiki/Trilinear_interpolation#/media/File:3D_interpolation2.svg)
			c011________c111
		   /|          /|
		c001________c101|
		|	|		|   |
		|   c010____|___c110
		|  /		|  /
		c000________c100
	*/
	static auto trilinearInterpolate(double c[2][2][2], double u, double v, double w) -> double { // aka linear interpolation in 3d https://en.wikipedia.org/wiki/Trilinear_interpolation
		auto accum = 0.0;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					accum += (i * u + (1 - i) * (1 - u))
						* (j * v + (1 - j) * (1 - v))
						* (k * w + (1 - k) * (1 - w))
						* c[i][j][k];
				}
			}
		}
		return accum;
	}
	/*
		variation of trilinear interpolation which uses a vector in place of a double
	*/
	static auto perlinInterpolate(Vec3 c[2][2][2], double u, double v, double w) -> double {
		auto uu = u * u * (3 - 2 * u); // Hermite Cubic to round interpolation to avoid Mach Bands (https://en.wikipedia.org/wiki/Mach_bands)
		auto vv = v * v * (3 - 2 * v);		// causes some smoothing
		auto ww = w * w * (3 - 2 * w);
		auto accum = 0.0;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					Vec3 weightV(u - i, v - j, w - k);
					accum += (i * uu + (1 - i) * (1 - uu))
						* (j * vv + (1 - j) * (1 - vv))
						* (k * ww + (1 - k) * (1 - ww))
						* dot(c[i][j][k], weightV);
				}
			}
		}
		return accum;
	}
};
