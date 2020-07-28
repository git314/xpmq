/* 
* Compute the quantile function for the normal distribution. 
* Copyright (C) 2020 developers at deepsleeping.xyz
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Personal Notes:
*
* Credit of this function should not go to me. I only assembled the parts.
*
* This impplimentation is a mix of the qnorm() function seen in 
* base R. I found the qnorm() C code here. https://svn.r-project.org/R/trunk/src/nmath/qnorm.c
* The code is under GNU General Public License
*
* This approximation to the inverse normal CDF was provided by V. Natarajan's C translation of 
* Peter John Acklam's algorithm. I chose this algorithm because I liked the speed and accuracy
*
* REFERENCE
*
* Beasley, J. D. and S. G. Springer (1977).
* Algorithm AS 111: The percentage points of the normal distribution,
* Applied Statistics, 26, 118-121.
* 
* Wichura, M.J. (1988).
* Algorithm AS 241: The Percentage Points of the Normal Distribution.
* Applied Statistics, 37, 477-484.
*
*/
#ifndef QNORM_H
#define QNORM_H

long double qnorm(long double p, long double mu, long double sigma);

#endif

