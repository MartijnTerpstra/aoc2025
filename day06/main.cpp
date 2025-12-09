#include "live_data.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include "test_data.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <array>
#include <span>

constexpr std::pair<int64_t, int64_t> boardSize(const std::string_view data)
{
	auto rows = std::ranges::count(data, '\n');
	if(data.back() == '\n')
	{
		--rows;
	}
	int64_t columns = 0;
	bool has_number = false;
	for(auto c : data)
	{
		switch(c)
		{
		case '0' ... '9':
			has_number = true;
			break;
		case ' ':
			columns += has_number;
			has_number = false;
			break;
		case '\n':
			columns += has_number;
			return { columns, rows };
		default:
			throw std::runtime_error("Invalid character");
		}
	}

	throw std::runtime_error("Invalid character");
}

enum class Operator : uint8_t
{
	Plus,
	Multiply
};

template<int64_t ColumnCount>
class RowIterator
{
public:
	using value_type = const int64_t;
	using difference_type = ptrdiff_t;

	constexpr RowIterator() = default;

	constexpr RowIterator(int64_t i, std::span<const int64_t> e)
		: idx(i)
		, elems(e)
	{ }

	constexpr RowIterator& operator++()
	{
		idx += ColumnCount;
		return *this;
	}

	constexpr RowIterator operator++(int)
	{
		auto ret = *this;
		idx += ColumnCount;
		return ret;
	}

	constexpr RowIterator operator+(ptrdiff_t o) const
	{
		auto ret = *this;
		idx += o * ColumnCount;
		return ret;
	}

	constexpr RowIterator& operator+=(ptrdiff_t value)
	{
		idx += value * ColumnCount;
		return *this;
	}

	constexpr RowIterator& operator--()
	{
		idx -= ColumnCount;
		return *this;
	}

	constexpr RowIterator operator--(int)
	{
		auto ret = *this;
		idx -= ColumnCount;
		return ret;
	}

	constexpr RowIterator operator-(ptrdiff_t o) const
	{
		auto ret = *this;
		ret -= o * ColumnCount;
		return ret;
	}

	constexpr ptrdiff_t operator-(const RowIterator& o) const
	{
		return (idx - o.idx) / ColumnCount;
	}

	constexpr RowIterator& operator-=(ptrdiff_t value)
	{
		idx -= value * ColumnCount;
		return *this;
	}

	constexpr const value_type& operator*() const
	{
		return elems[idx];
	}

	constexpr const value_type& operator*()
	{
		return elems[idx];
	}

	constexpr const value_type& operator[](ptrdiff_t o) const
	{
		return elems[idx + o * ColumnCount];
	}

	constexpr const value_type& operator[](ptrdiff_t o)
	{
		return elems[idx + o * ColumnCount];
	}

	constexpr bool operator==(const RowIterator& o) const
	{
		return idx == o.idx;
	}

	constexpr auto operator<=>(const RowIterator& o) const
	{
		return idx <=> o.idx;
	}

private:
	int64_t idx = -1;
	std::span<const int64_t> elems = {};
};

template<int64_t ColumnCount>
RowIterator<ColumnCount> operator+(ptrdiff_t l, const RowIterator<ColumnCount>& r)
{
	return r + l;
}

static_assert(std::random_access_iterator<RowIterator<4>>);

template<int64_t ColumnCount, int64_t RowCount>
struct MathBoard
{
	constexpr auto begin(int64_t column) const
	{
		return RowIterator<ColumnCount>(column, numbers);
	}

	constexpr auto end(int64_t column) const
	{
		return RowIterator<ColumnCount>(column + ColumnCount * RowCount, numbers);
	}

	std::array<int64_t, ColumnCount * RowCount> numbers = {};
	std::array<Operator, ColumnCount> operations;
};

template<std::pair<int64_t, int64_t> Counts>
constexpr auto parse1(const std::string_view data)
{
	constexpr static int64_t Columns = Counts.first;
	constexpr static int64_t Rows = Counts.second;

	MathBoard<Columns, Rows> results;
	int64_t row = 0;
	int64_t column = 0;
	int64_t value = 0;

	for(char d : data)
	{
		switch(d)
		{
		case '0' ... '9':
			value = value * 10 + d - '0';
			break;
		case ' ':
			if(value)
			{
				results.numbers[row * Columns + column++] = value;
				value = 0;
			}
			break;
		case '\n':
			if(value)
			{
				results.numbers[row * Columns + column++] = value;
				value = 0;
			}
			++row;
			column = 0;
			break;
		case '+':
			results.operations[column++] = Operator::Plus;
			break;
		case '*':
			results.operations[column++] = Operator::Multiply;
			break;
		default:
			throw std::runtime_error("Invalid character");
		}
	}
	return results;
}

template<const char* Text>
constexpr auto parse1()
{
	return parse1<boardSize(Text)>(Text);
}

template<std::pair<int64_t, int64_t> Counts>
constexpr auto parse2(const std::string_view data)
{
	constexpr static int64_t Columns = Counts.first;
	constexpr static int64_t Rows = Counts.second;

	const int64_t lineLength = data.find_first_of('\n') + 1;

	MathBoard<Columns, Rows> results;
	int64_t row = 0;
	int64_t column = 0;
	int64_t value = 0;
	int64_t idx = 0;

	for(char d : data)
	{
		if(d == '\n')
			break;

		for(int r = 0; r < Rows; ++r)
		{
			switch(data[idx + r * lineLength])
			{
			case '0' ... '9':
				value = value * 10 + data[idx + r * lineLength] - '0';
				break;
			}
		}
		if(value)
		{
			results.numbers[row++ * Columns + column] = value;
		}
		else
		{
			row = 0;
			++column;
		}
		value = 0;
		idx++;
	}

	column = 0;

	for(char d : data)
	{
		switch(d)
		{
		case '+':
			results.operations[column++] = Operator::Plus;
			break;
		case '*':
			results.operations[column] = Operator::Multiply;
			for(int r = 0; r < Rows; ++r)
			{
				if(!results.numbers[r * Columns + column])
				{
					results.numbers[r * Columns + column] = 1;
				}
			}
			column++;
			break;
		}
	}
	return results;
}

template<const char* Text>
constexpr auto parse2()
{
	return parse2<boardSize(Text)>(Text);
}

template<int64_t ColumnCount, int64_t RowCount>
constexpr auto solve(const MathBoard<ColumnCount, RowCount> map)
{
	int64_t res = 0;
	for(size_t c = 0; c < ColumnCount; ++c)
	{
		switch(map.operations[c])
		{
		case Operator::Plus:
			res += std::accumulate(map.begin(c), map.end(c), 0LL, std::plus<>{});
			break;
		case Operator::Multiply:
			res += std::accumulate(map.begin(c), map.end(c), 1LL, std::multiplies<>{});
			break;
		}
	}
	return res;
}

int main()
{
	constexpr int64_t testAnswer1 = solve(parse1<testData>());
	static_assert(testAnswer1 == 4277556);

	constexpr int64_t liveAnswer1 = solve(parse1<liveData>());
	static_assert(liveAnswer1 == 6172481852142);

	constexpr int64_t testAnswer2 = solve(parse2<testData>());
	static_assert(testAnswer2 == 3263827);

	constexpr int64_t liveAnswer2 = solve(parse2<liveData>());
	static_assert(liveAnswer2 == 10188206723429);
}