/*
 * MilkdropCompatability.hpp
 *
 *  Created on: Jun 18, 2008
 *      Author: pete
 */

#ifndef MILKDROPCOMPATABILITY_HPP_
#define MILKDROPCOMPATABILITY_HPP_

inline float sign(float a)
{
	return a < 0.0 ? -1.0 : 1.0;
}

inline float above(float a, float b)
{
	return a > b ? 1 : 0;
}
inline float equal(float a, float b)
{
	return a == b ? 1 : 0;
}
inline float below(float a, float b)
{
	return a < b ? 1 : 0;
}
inline float min(float a, float b)
{
	return a < b ? a : b;
}
#endif /* MILKDROPCOMPATABILITY_HPP_ */
