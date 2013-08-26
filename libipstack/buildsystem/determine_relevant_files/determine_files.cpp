#include "determine_files.h"
#include <iostream>
#include <sstream>

FeatureToFiles::FeatureToFiles(const std::string& basedir, const picojson::value::object& obj,
	const std::set<std::string>& kconfig_enabled) :
	mdir(basedir) {
	mEnabled=kconfig_enabled;
	in=0;
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
	
	std::cerr << std::string(in*2, ' ') << "NODE " << vname << ";" << name << std::endl;
	
	if (depends.size() && !checkDepends(depends)) {
		std::cerr << "Dependency not meet! " << depends << std::endl;
		return result;
	}
	
	if (subdir.size())
		mdir.cd(subdir);

	for (auto i = files.begin(); i!=files.end();++i) {
		std::string& file = *i;
		file=mdir.dir+'/'+file;
		std::cerr << std::string(in*2, ' ') << "file " << file << std::endl;
		//TODO add file to output stream
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