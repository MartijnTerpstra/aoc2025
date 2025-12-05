#include "live_data.hpp"
#include "test_data.hpp"
#include <algorithm>
#include <stdexcept>
#include <string_view>

constexpr std::pair<int, int> mapSize(const std::string_view data)
{
	auto columns = std::ranges::find(data, '\n') - data.begin();
	auto rows = std::ranges::count(data, '\n') + 1;
	if(data.back() == '\n')
	{
		--rows;
	}
	return { columns, rows };
}

template<int Columns, int Rows>
struct Map
{
public:
	constexpr bool& operator[](int c, int r)
	{
		if(!inRange(c, r))
		{
			throw std::runtime_error("Outside range");
		}
		return theOccupied[r + 1][c + 1];
	}

	constexpr bool operator[](int c, int r) const
	{
		if(!inRange(c, r))
		{
			throw std::runtime_error("Outside range");
		}
		return theOccupied[r + 1][c + 1];
	}

private:
	constexpr bool inRange(int c, int r) const
	{
		return c >= -1 && r >= -1 && c <= Columns && r <= Rows;
	}

	bool theOccupied[Rows + 2][Columns + 2] = {};
};

template<std::pair<int, int> Size>
constexpr auto parse(const std::string_view data)
{
	Map<Size.first, Size.second> results;
	int row = 0;
	int column = 0;

	for(char d : data)
	{
		switch(d)
		{
		case '@':
			results[column++, row] = true;
			break;
		case '.':
			results[column++, row] = false;
			break;
		case '\n':
			++row;
			column = 0;
			break;
		default:
			throw std::runtime_error("Invalid character");
		}
	}

	return results;
}

template<const char* Text>
constexpr auto parse()
{
	return parse<mapSize(Text)>(Text);
}

template<int Columns, int Rows>
constexpr auto part1(const Map<Columns, Rows> map)
{
	int res = 0;
	for(int r = 0; r < Rows; ++r)
	{
		for(int c = 0; c < Columns; ++c)
		{
			if(!map[c, r])
			{
				continue;
			}
			int numOccupieds = 0;
			for(int r_off = -1; r_off <= 1; ++r_off)
				for(int c_off = -1; c_off <= 1; ++c_off)
				{
					numOccupieds += map[c + c_off, r + r_off];
				}
			res += (numOccupieds < 4 + 1);
		}
	}
	return res;
}

template<int Columns, int Rows>
constexpr auto part2(Map<Columns, Rows> map)
{
	int removed = 0;
	int old_removed;
	Map<Columns, Rows> previouslyVisited = map;
	do
	{
		old_removed = removed;
		for(int r = 0; r < Rows; ++r)
		{
			for(int c = 0; c < Columns; ++c)
			{
				if(!previouslyVisited[c, r])
				{
					continue;
				}
				int numOccupieds = 0;
				for(int r_off = -1; r_off <= 1; ++r_off)
					for(int c_off = -1; c_off <= 1; ++c_off)
					{
						numOccupieds += map[c + c_off, r + r_off];
					}
				if(numOccupieds < 4 + 1)
				{
					++removed;
					map[c, r] = false;
					for(int r_off = -1; r_off <= 1; ++r_off)
						for(int c_off = -1; c_off <= 1; ++c_off)
						{
							previouslyVisited[c + c_off, r + r_off] = map[c + c_off, r + r_off];
						}
				}
				else
				{
					previouslyVisited[c, r] = false;
				}
			}
		}
	} while(old_removed != removed);
	return removed;
}

int main()
{
	constexpr int64_t testAnswer1 = part1(parse<testData>());
	static_assert(testAnswer1 == 13);

	constexpr int64_t liveAnswer1 = part1(parse<liveData>());
	static_assert(liveAnswer1 == 1540);

	constexpr int64_t testAnswer2 = part2(parse<testData>());
	static_assert(testAnswer2 == 43);

	constexpr int64_t liveAnswer2 = part2(parse<liveData>());
	static_assert(liveAnswer2 == 8972);
}