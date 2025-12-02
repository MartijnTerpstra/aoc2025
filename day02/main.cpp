#include "live_data.hpp"
#include "test_data.hpp"
#include <algorithm>
#include <array>
#include <cstdio>
#include <span>
#include <stdexcept>
#include <string_view>
#include <ranges>

constexpr size_t rangeCount(std::string_view data)
{
	return std::ranges::count(data, ',') + 1;
}

template<size_t N>
constexpr auto parse(std::string_view data)
{
	std::array<std::pair<int64_t, int64_t>, N> results;
	size_t index = 0;
	int64_t left_value = 0;
	int64_t value = 0;
	for(char c : data)
	{
		switch(c)
		{
		case '-':
			left_value = value;
			value = 0;
			break;
		case ',':
			results[index++] = { left_value, value };
			value = 0;
			break;
		case '0' ... '9':
			value = value * 10 + (c - '0');
			break;
		case '\n':
			break;
		default:
			throw std::runtime_error("Invalid character");
		}
	}
	if(value)
	{
		results[index] = { left_value, value };
	}
	return results;
}

template<const char* Text>
constexpr auto parse()
{
	return parse<rangeCount(Text)>(Text);
}

constexpr std::array<int64_t, 10> pow10{ 1, 10, 100, 1000, 10000, 100000 };

constexpr auto part1(std::span<const std::pair<int64_t, int64_t>> values)
{
	int64_t res = 0;
	for(auto [b, e] : values)
	{
		for(int64_t v = b; v <= e; ++v)
		{
			for(int64_t i = 1; i < 6; ++i)
			{
				if(v % pow10[i] == v / pow10[i] && v / pow10[i] >= pow10[i - 1])
				{
					res += v;
					break;
				}
			}
		}
	}
	return res;
}

constexpr auto part2(std::span<const std::pair<int64_t, int64_t>> values)
{
	int64_t res = 0;
	for(auto [b, e] : values)
	{
		for(int64_t v = b; v <= e; ++v)
		{
			for(int64_t i = 1; i < 6; ++i)
			{
				const int64_t chunk = v % pow10[i];
				int64_t val = v;
				bool success = v >= pow10[i];
				while((val /= pow10[i]) != 0)
				{
					if(chunk != val % pow10[i] || val < pow10[i - 1])
					{
						success = false;
						break;
					}
				}
				if(success)
				{
					res += v;
					break;
				}
			}
		}
	}
	return res;
}

int main()
{
	constexpr int64_t testAnswer1 = part1(parse<testData>());
	static_assert(testAnswer1 == 1227775554);

	constexpr int64_t liveAnswer1 = part1(parse<liveData>());
	static_assert(liveAnswer1 == 21139440284);

	constexpr int64_t testAnswer2 = part2(parse<testData>());
	static_assert(testAnswer2 == 4174379265);

	constexpr int64_t liveAnswer2 = part2(parse<liveData>());
	static_assert(liveAnswer2 == 38731915928);
}