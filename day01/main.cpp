#include "live_data.hpp"
#include "test_data.hpp"
#include <algorithm>
#include <array>
#include <span>
#include <string_view>

consteval size_t lineCount(std::string_view text)
{
	size_t count = 1;
	for(char c : text)
	{
		count += (c == '\n');
	}
	if(text.back() == '\n') // ignore empty endlines
	{
		--count;
	}
	return count;
}

template<size_t N>
consteval auto parse(std::string_view lines)
{
	std::array<int, N> results;
	size_t index = 0;
	int value = 0;
	int direction = 0;
	for(char c : lines)
	{
		switch(c)
		{
		case 'L':
			direction = -1;
			break;
		case 'R':
			direction = 1;
			break;
		case '0' ... '9':
			value = value * 10 + (c - '0');
			break;
		case '\n':
			results[index++] = direction * value;
			direction = 0;
			value = 0;
			break;
		}
	}
	if(value)
	{
		results[index++] = direction * value;
	}
	return results;
}

template<const char* Text>
consteval auto parse()
{
	return parse<lineCount(Text)>(Text);
}

consteval auto part1(std::span<const int> values)
{
	const int offset = 10000000;
	int value = offset + 50;
	int zeroes = 0;
	for(auto v : values)
	{
		if((value += v) % 100 == 0)
		{
			++zeroes;
		}
	}
	return zeroes;
}

consteval auto part2(std::span<const int> values)
{
	const int offset = 10000000;
	int value = offset + 50;
	int zeroes = 0;
	for(auto v : values)
	{
		int old_value = value;
		int new_value = value += v;
		int min_rotation = std::min(old_value / 100, new_value / 100);
		int max_rotation = std::max(old_value / 100, new_value / 100);
		// Spins of the wheel
		zeroes += max_rotation - min_rotation;
		// Starts from zero
		zeroes -= (old_value % 100 == 0 && v < 0);
		// Ends in zero
		zeroes += (new_value % 100 == 0 && v < 0);
	}
	return zeroes;
}

int main()
{
	constexpr int testAnswer1 = part1(parse<testData>());
	static_assert(testAnswer1 == 3);

	constexpr int liveAnswer1 = part1(parse<liveData>());
	static_assert(liveAnswer1 == 1007);

	constexpr int testAnswer2 = part2(parse<testData>());
	static_assert(testAnswer2 == 6);

	constexpr int liveAnswer2 = part2(parse<liveData>());
	static_assert(liveAnswer2 == 5820);
}