#pragma once

#include "picojson.h"
#include "parsekconfig.h"
#include <string>
#include <list>
#include <set>
#include <iostream>

class FeatureToFiles {
public:
	FeatureToFiles(const std::string& basedir, const picojson::value::object& obj,
				   const std::set<std::string>& kconfig_enabled,
				   std::ostream* output_files_stream, bool all_features);
	bool success();
private:
	std::string getMapString(const picojson::value::object& obj, std::string name);
	std::list<std::string> get_files(const picojson::value::object& obj);
	bool value(const picojson::value& value);
	bool node(const picojson::value::object& obj);
	bool component(const picojson::value::array& obj);
	bool checkDepends(const std::string depends);
	
	bool msuccess;
	std::set<std::string> mEnabled;
	std::ostream* output_files_stream;
	bool all_features;
	int in;
	
	class Dir {
	public:
		Dir(std::string dir) {
			this->dir = dir;
			size_t pos = 0;
			while((pos = this->dir.find('\\', pos)) != std::string::npos) {
				this->dir.replace(pos, 1, "/");
				pos += 1;
			}
		}
		std::string dir;
		void cd(const std::string& sub) {
			if (sub.size() && sub[0] != '/')
				dir += '/';
			dir += sub;
		}
		void up() {
			int i = dir.find_last_of('/');
			if (i!=std::string::npos)
				dir.erase(i);
		}
	};
	
	Dir mdir;
};
