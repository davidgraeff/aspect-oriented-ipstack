#pragma once
/*
 * Copyright 2013 David Graeff
 * BSD License
 */

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
namespace kconfigparser {
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
				std::cout << "found option! " << key << std::endl;
			} else { // detect string/int/whatever option
				std::string value = key;
				key.resize(key.find_first_of('='));
				value.erase(0,key.size()+1);
				std::cout << "found value! " << key << ": " << value << std::endl;
				kconfig_keyvalue.insert(std::pair<std::string, std::string>(key,value));
			}
		}
		file.close();
		return true;
	}
}