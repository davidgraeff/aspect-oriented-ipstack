/*
 * Copyright 2013 David Graeff
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE REGENTS AND CONTRIBUTORS, INC. OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <fstream>

/**
 * Parse a .config output file of the kconfig linux kernel configuration tool
 * Result is hash "kconfig_enabled" that contains a key if it is enabled in the .config file like
 * CONFIG_KERNEL_GZIP=y.
 * Values are stored in a map "kconfig_keyvalue".
 */
class kconfigparser {
public:
	// Result hash and map
	std::set<std::string> kconfig_enabled;
	std::map<std::string, std::string> kconfig_keyvalue;
	
	inline bool hasEnding (std::string const &fullString, std::string const &ending)
	{
		if (fullString.length() >= ending.length()) {
			return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
		} else {
			return false;
		}
	}

	// Load file and add values to hash and map
	bool load_kconfig_file(const std::string& filename) {
		std::ifstream file;
		
		file.open (filename.c_str(),std::ios::out);
		if (!file.is_open()) {
			std::cerr << "Could not open kconfig file for read!\n";
			return false;
		}

		std::string line;
		while ( file.good() ) {
			getline (file,line);
			// jump over comment lines and empty lines
			if (line.size()<2 || line[0]=='#')
				continue;
			
			std::string key = line;
			key.erase(0,sizeof("CONFIG_")-1);
			
			// detect yes/no option
			if (hasEnding(key, "=y")) {
				key.resize(key.size()-2);
				kconfig_enabled.insert(key);
				//std::cerr << "found option! " << key << std::endl;
			} else { // detect string/int/whatever option
				std::string value = key;
				key.resize(key.find_first_of('='));
				value.erase(0,key.size()+1);
				//std::cerr << "found value! " << key << ": " << value << std::endl;
				kconfig_keyvalue.insert(std::pair<std::string, std::string>(key,value));
			}
		}
		file.close();
		return true;
	}
};