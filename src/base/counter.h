/*	
	This file is part of Ingnomia https://github.com/rschurade/Ingnomia
    Copyright (C) 2017-2020  Ralph Schurade, Ingnomia Team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <absl/container/btree_map.h>
#include <ranges>

template <class T>
class Counter
{
public:
	void add( T id )
	{
		++m_counts[id];
	}

	auto keys()
	{
		return m_counts | std::views::keys;
	}

	unsigned int count( T key )
	{
		return m_counts.at( key );
	}

	void reset()
	{
		m_counts.clear();
	}

private:
	absl::btree_map<T, unsigned int> m_counts;
};