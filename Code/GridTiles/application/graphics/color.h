#pragma once
#include "imgui/imgui.h"

template <typename T>
struct ColorTemplate {
	union {
		struct {
			T r;
			T g;
			T b;
			T a;
		};

		T data[4];
	};

	constexpr ColorTemplate() : r(static_cast<T>(0))
	                          , g(static_cast<T>(0))
	                          , b(static_cast<T>(0))
	                          , a(static_cast<T>(1)) { }

	constexpr ColorTemplate(const T& r, const T& g, const T& b) : r(r)
	                                       , g(g)
	                                       , b(b)
	                                       , a(a) { }

	constexpr ColorTemplate(const T& r, const T& g, const T& b, const T& a) : r(r)
	                                            , g(g)
	                                            , b(b)
	                                            , a(a) { }

	constexpr ColorTemplate(const T& value) : r(value)
	                                 , g(value)
	                                 , b(value)
	                                 , a(static_cast<T>(1)) { }

	constexpr ColorTemplate(const T& value, const T& a) : r(value)
	                                      , g(value)
	                                      , b(value)
	                                      , a(a) { }

	constexpr ColorTemplate(const Vector<T, 3>& rgb) : r(rgb.x)
	                                                 , g(rgb.y)
	                                                 , b(rgb.z)
	                                                 , a(static_cast<T>(1)) { }

	constexpr ColorTemplate(const Vector<T, 4> rgba) : r(rgba.x)
	                                                 , g(rgba.y)
	                                                 , b(rgba.z)
	                                                 , a(rgba.w) { }

	static constexpr ColorTemplate<T> hsva(const T& h, const T& s, const T& v, const T& a = static_cast<T>(1)) {
		return ColorTemplate<T>::hsvaToRgba(ColorTemplate<T>(h, s, v, a));
	}

	constexpr operator Vector<T, 3>() const {
		return Vector<T, 3>{r, g, b};
	}

	constexpr operator Vector<T, 4>() const {
		return Vector<T, 4>{r, g, b, a};
	}

	constexpr T& operator[](size_t index) noexcept {
		return data[index];
	}

	constexpr const T& operator[](size_t index) const noexcept {
		return data[index];
	}

	constexpr ColorTemplate<T> withOpacity(const T& opacity) const {
		return ColorTemplate<T>(r, g, b, opacity);
	}

	constexpr ImU32 u32() const noexcept {
		return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
	}

	constexpr ImVec4 iv4() const noexcept {
		return ImVec4(r, g, b, a);
	}

	constexpr cv::Scalar cv() const noexcept {
		return cv::Scalar(cvRound(b * 255), cvRound(g * 255), cvRound(r * 255));
	}

	// format: 0xRRGGBBAA
	template <int Hex, bool Alpha = false>
	constexpr static ColorTemplate<T> get() {
		if constexpr (Alpha == true) {
			constexpr int value = Hex;
			return ColorTemplate<T>{
				static_cast<T>(value >> 24 & 0xFF) / static_cast<T>(255),
				static_cast<T>(value >> 16 & 0xFF) / static_cast<T>(255),
				static_cast<T>(value >> 8 & 0xFF) / static_cast<T>(255),
				static_cast<T>(value & 0xFF) / static_cast<T>(255)
			};
		} else {
			constexpr int value = Hex << 8 | 0xFF;
			return ColorTemplate<T>{
				static_cast<T>(value >> 24 & 0xFF) / static_cast<T>(255),
				static_cast<T>(value >> 16 & 0xFF) / static_cast<T>(255),
				static_cast<T>(value >> 8 & 0xFF) / static_cast<T>(255),
				static_cast<T>(value & 0xFF) / static_cast<T>(255)
			};
		}
	}

	constexpr static ColorTemplate<T> hsvaToRgba(const ColorTemplate<T>& hsva) {
		constexpr T h = hsva.r * static_cast<T>(360);
		constexpr T s = hsva.g;
		constexpr T v = hsva.b;
		constexpr T a = hsva.a;

		if (s == static_cast<T>(0))
			return ColorTemplate<T>(v, v, v, a);

		constexpr int hi = static_cast<int>(h / static_cast<T>(60)) % 6;
		constexpr T f = h / static_cast<T>(60) - static_cast<float>(hi);
		constexpr T p = v * (static_cast<T>(1) - s);
		constexpr T q = v * (static_cast<T>(1) - s * f);
		constexpr T t = v * (static_cast<T>(1) - s * (static_cast<T>(1) - f));

		switch (hi) {
		case 0:
			return ColorTemplate<T>(v, t, p, a);
		case 1:
			return ColorTemplate<T>(q, v, p, a);
		case 2:
			return ColorTemplate<T>(p, v, t, a);
		case 3:
			return ColorTemplate<T>(p, q, v, a);
		case 4:
			return ColorTemplate<T>(t, p, v, a);
		case 5:
			return ColorTemplate<T>(v, p, q, a);
		default:
			return ColorTemplate<T>();
		}
	}

	constexpr static ColorTemplate<T> rgbaToHsva(const ColorTemplate<T>& rgba) {
		constexpr T r = rgba.r;
		constexpr T g = rgba.g;
		constexpr T b = rgba.b;
		constexpr T a = rgba.a;


		constexpr T min = fmin(fmin(r, g), b);
		constexpr T max = fmax(fmax(r, g), b);
		constexpr T d = max - min;

		if (d == static_cast<T>(0))
			return ColorTemplate<T>(static_cast<T>(0), static_cast<T>(0), max);

		constexpr T dr = ((max - r) / static_cast<T>(6) + max / static_cast<T>(2)) / d;
		constexpr T dg = ((max - g) / static_cast<T>(6) + max / static_cast<T>(2)) / d;
		constexpr T db = ((max - b) / static_cast<T>(6) + max / static_cast<T>(2)) / d;

		T h = static_cast<T>(0);
		if (r == max)
			h = db - dg;
		else if (g == max)
			h = static_cast<T>(1) / static_cast<T>(3) + dr - db;
		else if (b == max)
			h = static_cast<T>(2) / static_cast<T>(3) + dg - dr;

		if (h < static_cast<T>(0))
			h += static_cast<T>(1);
		if (h > static_cast<T>(1))
			h -= static_cast<T>(1);

		return ColorTemplate<T>(h, d / max, max);
	}

	constexpr static ColorTemplate<T> sum(const ColorTemplate<T>& color1, const ColorTemplate<T>& color2) {
		T a = color1.a + color2.a * (static_cast<T>(1) - color1.a);
		return ColorTemplate<T>(
			(color1.r + color1.a + color2.r * color2.a * (static_cast<T>(1) - color1.a)) / a,
			(color1.g + color1.a + color2.g * color2.a * (static_cast<T>(1) - color1.a)) / a,
			(color1.b + color1.a + color2.b * color2.a * (static_cast<T>(1) - color1.a)) / a,
			a
		);
	}

	constexpr static ColorTemplate<T> blend(const ColorTemplate<T>& color1, const ColorTemplate<T>& color2, const T& blend) {
		return ColorTemplate<T>(
			(static_cast<T>(1) - blend) * color1.r + blend * color2.r,
			(static_cast<T>(1) - blend) * color1.g + blend * color2.g,
			(static_cast<T>(1) - blend) * color1.b + blend * color2.b,
			(static_cast<T>(1) - blend) * color1.a + blend * color2.a
		);
	}
};

template <typename T>
constexpr ColorTemplate<T>& operator+=(ColorTemplate<T>& first, const ColorTemplate<T>& second) {
	first.r += second.r;
	first.g += second.g;
	first.b += second.b;
	first.a += second.a;
	return first;
}

template <typename T>
constexpr ColorTemplate<T> operator+(const ColorTemplate<T>& first, const
                                     ColorTemplate<T>& second) {
	return ColorTemplate<T>{
		first.r + second.r,
		first.g + second.g,
		first.b + second.b,
		first.a + second.a
	};
}

template <typename T>
constexpr ColorTemplate<T> operator*(const ColorTemplate<T>& color, const T& factor) {
	return ColorTemplate<T>{
		color.r * factor,
		color.g * factor,
		color.b * factor,
		color.a * factor
	};
}

template <typename T>
constexpr ColorTemplate<T> operator*(const T& factor, const ColorTemplate<T>& color) {
	return ColorTemplate<T>{
		factor * color.r,
		factor * color.g,
		factor * color.b,
		factor * color.a
	};
}

typedef ColorTemplate<float> Color;

namespace Colors {
	constexpr static Color DISABLED = Color::get<0xA0A0A0>();
	constexpr static Color ACCENT = Color::get<0x1F6678>();
	constexpr static Color BACK = Color::get<0x4D4D4D>();
	constexpr static Color ALPHA = Color::get<0x0, true>();

	constexpr static Color RGB_R = Color::get<0xFF0000>();
	constexpr static Color RGB_G = Color::get<0x00FF00>();
	constexpr static Color RGB_B = Color::get<0x0000FF>();
	constexpr static Color A = Color::get<0x0, true>();

	constexpr static Color NAVY = Color::get<0x001F3F>();
	constexpr static Color BLUE = Color::get<0x0074D9>();
	constexpr static Color AQUA = Color::get<0x7FDBFF>();
	constexpr static Color TEAL = Color::get<0x39CCCC>();
	constexpr static Color OLIVE = Color::get<0x3D9970>();
	constexpr static Color GREEN = Color::get<0x2ECC40>();
	constexpr static Color LIME = Color::get<0x01FF70>();
	constexpr static Color YELLOW = Color::get<0xFFDC00>();
	constexpr static Color ORANGE = Color::get<0xFF851B>();
	constexpr static Color RED = Color::get<0xFF4136>();
	constexpr static Color MAROON = Color::get<0x85144b>();
	constexpr static Color FUCHSIA = Color::get<0xF012BE>();
	constexpr static Color PURPLE = Color::get<0xB10DC9>();
	constexpr static Color BLACK = Color::get<0x111111>();
	constexpr static Color GRAY = Color::get<0xAAAAAA>();
	constexpr static Color SILVER = Color::get<0xDDDDDD>();
	constexpr static Color WHITE = Color::get<0xFFFFFF>();
}
