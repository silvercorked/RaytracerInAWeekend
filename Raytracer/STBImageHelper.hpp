#pragma once

// Disable strict warnings for this header from Microsoft Visual C++ compiler.
#ifdef _MSC_VER
#pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "external/stb_image.h"

#include <cstdlib>
#include <iostream>

class AlexSTBImage {
	const int bytesPerPixel = 3;
	unsigned char* data;
	int imageWidth, imageHeight;
	int bytesPerScanline;

	static int clamp(int x, int low, int high) {
		if (x < low) return low; // returns in range [low, high)
		if (x < high) return x;
		return high - 1;
	}

public:
	AlexSTBImage() : data(nullptr) {}
	AlexSTBImage(const char* imageFilename) {
		auto filename = std::string(imageFilename);
		//auto imagedir = getenv("IMAGES_DIR_PATH");
		// Hunt for image files in some likely locations
		//if (imagedir && this->load(std::string(imagedir) + "/" + imageFilename)) return;
		if (this->load(filename)) return;
		if (this->load("images/" + filename)) return;
		if (this->load("../images/" + filename)) return;
		if (this->load("../../images/" + filename)) return;
		std::cerr << "ERROR: Could not load image file '" << imageFilename << "'.\n";
	}
	~AlexSTBImage() { STBI_FREE(this->data); }

	auto load(const std::string filename) -> bool {
		auto n = this->bytesPerPixel;
		this->data = stbi_load(filename.c_str(), &this->imageWidth, &this->imageHeight, &n, this->bytesPerPixel);
		this->bytesPerScanline = this->imageWidth * this->bytesPerPixel;
		return data != nullptr;
	}
	auto width() const -> int { return (data == nullptr) ? 0 : this->imageWidth; }
	auto height() const -> int { return (data == nullptr) ? 0 : this->imageHeight; }
	auto pixelData(int x, int y) const -> const unsigned char* {
		static unsigned char magenta[] = { 255, 0, 255 }; // error color (ie missing texture => magenta)
		if (this->data == nullptr) return magenta;
		x = this->clamp(x, 0, this->imageWidth);
		y = this->clamp(y, 0, this->imageHeight);
		return this->data + y * this->bytesPerScanline + x * this->bytesPerPixel;
	}
};

// Restore MSVC compiler warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif
