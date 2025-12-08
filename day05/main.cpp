#include "live_data.hpp"
#include <cstdlib>
#include "test_data.hpp"
#include <algorithm>
#include <stdexcept>
#include <string_view>
#include <array>
#include <span>

constexpr std::pair<int64_t, int64_t> ingredientSize(const std::string_view data)
{
	auto transition_point = data.find("\n\n");

	auto freshCount = std::ranges::count(data.begin(), data.begin() + transition_point, '\n') + 1;
	auto availableSize =
		std::ranges::count(data.begin() + transition_point + 2, data.end(), '\n') + 1;
	return { freshCount, availableSize };
}

template<int64_t FreshCount, int64_t AvailableCount>
struct IngredientMap
{
	std::array<std::pair<int64_t, int64_t>, FreshCount> fresh;
	std::array<int64_t, AvailableCount> available;
};

template<std::pair<int64_t, int64_t> Counts>
constexpr auto parse(const std::string_view data)
{
	IngredientMap<Counts.first, Counts.second> results;
	int64_t row = 0;
	int64_t value = 0;

	auto transition_point = data.find("\n\n");

	for(char d : data.substr(0, transition_point + 1))
	{
		switch(d)
		{
		case '-':
			results.fresh[row].first = value;
			value = 0;
			break;
		case '0' ... '9':
			value = value * 10 + d - '0';
			break;
		case '\n':
			results.fresh[row++].second = value;
			value = 0;
			break;
		default:
			throw std::runtime_error("Invalid character");
		}
	}

	row = 0;
	value = 0;

	for(char d : data.substr(transition_point + 2))
	{
		switch(d)
		{
		case '0' ... '9':
			value = value * 10 + d - '0';
			break;
		case '\n':
			results.available[row++] = value;
			value = 0;
			break;
		default:
			throw std::runtime_error("Invalid character");
		}
	}
	results.available[row] = value;

	return results;
}

template<const char* Text>
constexpr auto parse()
{
	return parse<ingredientSize(Text)>(Text);
}

template<int64_t FreshCount, int64_t AvailableCount>
constexpr auto part1(const IngredientMap<FreshCount, AvailableCount> map)
{
	int64_t res = 0;
	for(auto ingredient_id : map.available)
	{
		bool is_fresh = std::ranges::any_of(map.fresh, [=](const std::pair<int64_t, int64_t> r) {
			return ingredient_id >= r.first && ingredient_id <= r.second;
		});

		res += is_fresh;
	}
	return res;
}

constexpr int64_t solve(
	std::pair<int64_t, int64_t> range, const std::span<const std::pair<int64_t, int64_t>> fresh)
{
	for(auto it = fresh.begin(); it != fresh.end(); ++it)
	{
		auto [begin, end] = *it;
		if(range.first > end || range.second < begin)
		{
			continue;
		}
		if(range.first >= begin && range.second <= end) // Other range bigger
		{
			return 0;
		}
		if(begin > range.first && end < range.second) // Other range smaller
		{
			return solve({ range.first, begin - 1 }, std::span{ it + 1, fresh.end() }) +
				   solve({ end + 1, range.second }, std::span{ it + 1, fresh.end() });
		}
		if(range.first >= begin && range.first <= end) // Splice begin
		{
			range.first = end + 1;
		}
		else // Splice end
		{
			range.second = begin - 1;
		}
	}
	return range.second - range.first + 1;
}

template<int64_t FreshCount, int64_t AvailableCount>
constexpr auto part2(const IngredientMap<FreshCount, AvailableCount> map)
{
	int64_t res = 0;
	for(auto it = map.fresh.begin(); it != map.fresh.end(); ++it)
	{
		res += solve(*it, std::span{ map.fresh.begin(), it });
	}
	return res;
}

int main()
{
	constexpr int64_t testAnswer1 = part1(parse<testData>());
	static_assert(testAnswer1 == 3);

	constexpr int64_t liveAnswer1 = part1(parse<liveData>());
	static_assert(liveAnswer1 == 613);

	constexpr int64_t testAnswer2 = part2(parse<testData>());
	static_assert(testAnswer2 == 14);

	constexpr int64_t liveAnswer2 = part2(parse<liveData>());
	static_assert(liveAnswer2 == 336495597913098);
}