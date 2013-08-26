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

#include "determine_files.h"
#include <iostream>
#include <sstream>

FeatureToFiles::FeatureToFiles(const std::string& basedir, const picojson::value::object& obj,
	const std::set<std::string>& kconfig_enabled,
	std::ostream* output_files_stream, bool all_features) :
	mdir(basedir), mEnabled(kconfig_enabled), output_files_stream(output_files_stream), all_features(all_features), in(0)
{
	msuccess = node(obj);
}

bool FeatureToFiles::component(const picojson::value::array& obj) {
	bool result = true;
	for (picojson::value::array::const_iterator i = obj.begin(); i != obj.end(); ++i) {
		const picojson::value& value = *i;
		
		if (!value.is<picojson::object>()) {
			std::cerr << "[OBJECT] expected in comp" << std::endl;
			return false;
		}
		result &= node(value.get<picojson::object>());
		if (!result)
			return false;
	}
	return result;
}

std::list<std::string> FeatureToFiles::get_files(const picojson::value::object& obj) {
	std::list<std::string> files;
	if (obj.count("file")) {
		files.push_back(obj.at("file").get<std::string>());
	} else if (obj.count("files")) {
		const picojson::value::array& arr = obj.at("files").get<picojson::array>();
		for (picojson::value::array::const_iterator i = arr.begin(); i != arr.end(); ++i) {
			files.push_back(i->get<std::string>());
		}
	}
	return files;
}

std::string FeatureToFiles::getMapString(const picojson::value::object& obj, std::string name) {
	auto i = obj.find(name);
	if (i==obj.end())
		return std::string();
	else
		return i->second.get<std::string>();
}

bool FeatureToFiles::checkDepends(const std::string depends) {
	bool ok = true;
	bool negate_depend = false;
	bool all_depend = false;
	bool min_one_depend = false;
	std::istringstream iss(depends);
	while (iss) {
		std::string sub;
		iss >> sub;
		if (!sub.size())
			continue;
		if (sub=="not") {
			negate_depend=true;
		} else if(sub=="and") {
			all_depend=true;
			min_one_depend=false;
			negate_depend=false;
		} else if(sub=="or") {
			min_one_depend=true;
			all_depend=false;
			negate_depend=false;
		} else if (sub.compare(0,1,"&")==0) {
			sub.erase(0,1);
			bool enabled = mEnabled.count(sub);
			enabled = (enabled && !negate_depend) || (!enabled && negate_depend);
			if (all_depend)
				ok &= enabled;
			else if (min_one_depend)
				ok |= enabled;
		} else {
			std::cerr << "Did not understand depends string: " << depends << "; " << sub << std::endl;
		}
	};
	return ok;
}

bool FeatureToFiles::node(const picojson::value::object& obj) {
	++in;
	bool result = true;
	std::string vname = getMapString(obj, "vname");
	std::string name = getMapString(obj, "name");
	std::string subdir = getMapString(obj, "subdir");
	std::string depends = getMapString(obj, "depends");
	std::list<std::string> files = get_files(obj);
	
// 	std::cerr << std::string(in*2, ' ') << "NODE " << vname << ";" << name << std::endl;
	
	if (!all_features && depends.size() && !checkDepends(depends)) {
		std::cerr << "Dependency not meet! " << depends << std::endl;
		return result;
	}
	
	if (subdir.size())
		mdir.cd(subdir);

	for (auto i = files.begin(); i!=files.end();++i) {
		std::string& file = *i;
		file=mdir.dir+'/'+file;
		//std::cerr << std::string(in*2, ' ') << "file " << file << std::endl;
		*output_files_stream << file << ";";	
	}

	if (obj.count("comp")) {
		const picojson::value& value = obj.at("comp");
		if (!value.is<picojson::array>()) {
			std::cerr << "[ARRAY] expected as comp" << std::endl;
			return false;
		}
		result &= component(value.get<picojson::array>());
	}

	if (subdir.size())
		mdir.up();

	--in;
	return result;
}

bool FeatureToFiles::success() {
	return msuccess;
}