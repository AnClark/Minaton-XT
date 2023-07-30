/*
 *  binary_to_compressed_c.hpp
 *
 *  Copyright (c) 2022-2023 AnClark Liu
 *
 *  License: MIT
 */

#pragma once

#include <assert.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <string>

// stb_compress* from stb.h - declaration
typedef unsigned int stb_uint;
typedef unsigned char stb_uchar;
stb_uint stb_compress(stb_uchar* out, stb_uchar* in, stb_uint len);

bool binary_to_compressed_c_local(const char* filename, const char* symbol, std::stringstream& output_code, int& output_array_size, std::string& output_array_symbol, bool use_compression = true);
