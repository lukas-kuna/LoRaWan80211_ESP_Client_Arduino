/* utilities.h -- Helper functions implementation
 *
 * Copyright (C) 2013 Semtech
 * Copyright (C) 2016 Lukáš Kuna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */
 
#ifndef _utilities_h
#define _utilities_h

#include <string.h>

#define memset1 memset
#define memcpy1 memcpy

/*!
 * \brief Returns the maximum value betwen a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval maxValue Maximum value
 */
 #define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )

#endif
