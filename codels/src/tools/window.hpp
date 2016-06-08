#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <stdint.h>
#include <vector>
#include <math.h> /* M_PI */
#include <cmath> /* cos */
#include <assert.h> /* cos */

/*!
 * 
 * This file is modified from : IT++ library : http://itpp.sourceforge.net/4.3.1/
 * 
 * \file
 * \brief Implementation of window functions
 * \author Tony Ottosson, Tobias Ringstrom, Pal Frenger, Adam Piatyszek
 *         and Kumar Appaiah
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) 1995-2010  (see AUTHORS file for a list of contributors)
 * -------------------------------------------------------------------------
 */
 
namespace openAFE {

	template<typename T = double>	
	std::vector<T> hamming( size_t n ) {
	  std::vector<T> t(n);

	  if (n == 1)
		t[0] = 0.08;
	  else
		for ( size_t i = 0 ; i < n ; i++ )
		  t[i] = (0.54 - 0.46 * std::cos(2.0 * M_PI * i / (n - 1)));

	  return t;
	}

	template<typename T = double>	
	std::vector<T> hanning( size_t n ) {
	  std::vector<T> t(n);

	  for ( size_t i = 0 ; i < n ; i++ )
		t[i] = 0.5 * (1.0 - std::cos(2.0 * M_PI * (i + 1) / (n + 1)));

	  return t;
	}

	// matlab version
	template<typename T = double>		
	std::vector<T> hann(size_t n) {
		std::vector<T>  t(n);

		for ( size_t i = 0 ; i < n ; i++ )
			t[i] = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / (n - 1)));

	  return t;
	}

	template<typename T = double>	
	std::vector<T> blackman(size_t n) {
	  std::vector<T>  t(n);

	  for ( size_t i = 0 ; i < n ; i++ )
		t[i] = 0.42 - 0.5 * std::cos(2.0 * M_PI * i / (n - 1)) + 0.08 * std::cos(4.0 * M_PI * i / (n - 1));

	  return t;
	}

	template<typename T = double>
	std::vector<T> triang(size_t n) {
	  std::vector<T> t(n);

	  if (n % 2) { // Odd
		for (size_t i = 0; i < n / 2; i++)
		  t[i] = t[n - i - 1] = 2.0 * (i + 1) / (n + 1);
		t[n / 2] = 1.0;
	  }
	  else
		for (size_t i = 0; i < n / 2; i++)
		  t[i] = t[n - i - 1] = (2.0 * i + 1) / n;

	  return t;
	}

	template<typename T = double>
	std::vector<T> sqrt_win(size_t n) {
	  std::vector<T> t(n);

	  if (n % 2) { // Odd
		for (size_t i = 0; i < n / 2; i++)
		  t[i] = t[n - i - 1] = std::sqrt(2.0 * (i + 1) / (n + 1));
		t[n / 2] = 1.0;
	  }
	  else
		for (size_t i = 0; i < n / 2; i++)
		  t[i] = t[n - i - 1] = std::sqrt((2.0 * i + 1) / n);

	  return t;
	}

}; //namespace openAFE

#endif // #ifndef WINDOW_HPP
