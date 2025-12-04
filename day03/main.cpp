#include "live_data.hpp"
#include "test_data.hpp"
#include <algorithm>
#include <array>
#include <cstdio>
#include <span>
#include <string_view>

constexpr size_t rangeCount(const std::string_view data, const char delim)
{
	auto elems = std::ranges::count(data, delim) + 1;
	if(delim == '\n' && data.back() == delim)
	{
		--elems;
	}
	return elems;
}

template<size_t N>
constexpr auto parse(const std::string_view data)
{
	std::array<std::string_view, N> results;
	size_t index = 0;
	size_t from = 0;
	do
	{
		size_t till = data.find_first_of('\n', from);
		results[index++] = data.substr(from, till - from);
		from = till + 1;
	} while(from != 0);

	return results;
}

template<const char* Text>
constexpr auto parse()
{
	return parse<rangeCount(Text, '\n')>(Text);
}

constexpr auto part1(std::span<const std::string_view> rows)
{
	int64_t res = 0;
	for(auto row : rows)
	{
		auto first_max = std::ranges::max_element(row.begin(), row.end() - 1);
		auto second_max = std::ranges::max_element(first_max + 1, row.end());
		res += (*first_max - '0') * 10 + *second_max - '0';
	}
	return res;
}

constexpr auto part2(std::span<const std::string_view> rows)
{
	int64_t res = 0;
	for(auto row : rows)
	{
		int64_t m = 0;
		auto max = row.begin();
		for(int i = 0; i < 12; ++i)
		{
			max = std::ranges::max_element(max + (i != 0), row.end() - 11 + i);
			m = m * 10 + (*max - '0');
		}
		res += m;
	}
	return res;
}

int main()
{
	constexpr int64_t testAnswer1 = part1(parse<testData>());
	static_assert(testAnswer1 == 357);

	constexpr int64_t liveAnswer1 = part1(parse<liveData>());
	static_assert(liveAnswer1 == 16854);

	constexpr int64_t testAnswer2 = part2(parse<testData>());
	static_assert(testAnswer2 == 3121910778619);

	constexpr int64_t liveAnswer2 = part2(parse<liveData>());
	static_assert(liveAnswer2 == 167526011932478);
}