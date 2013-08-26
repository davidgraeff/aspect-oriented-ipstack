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

#include "picojson.h"
#include "parsekconfig.h"
#include <string>
#include <list>
#include <set>
#include <iostream>

/**
 * Output files separated by spaces that are relevant for the selected feature-set.
 * If all_features is set, the feature-set is ignored and all files are relevant.
 */
class FeatureToFiles {
public:
	/**
	 * Output files separated by spaces that are relevant for the selected feature-set.
	 * If all_features is set, the feature-set is ignored and all files are relevant.
	 */
	FeatureToFiles(const std::string& basedir, const picojson::value::object& obj,
				   const std::set<std::string>& kconfig_enabled,
				   std::ostream* output_files_stream, bool all_features);
	/**
	 * Return true if the feature-set to files transformation has been successfull.
	 * A parse error or non-expected data types may lead to an error and false
	 * is returned.
	 */
	bool success();
private:
	/**
	 * Return the string of the map at position "name". Return an empty string
	 * if the map does not contain that key.
	 */
	std::string getMapString(const picojson::value::object& obj, std::string name);
	/**
	 * Return a list of files either via the "files" key of the obj map or via
	 * the "file" key. An empty list is returned if neither of those keys is existing.
	 */
	std::list<std::string> get_files(const picojson::value::object& obj);
	/**
	 * Parse a json object. The FeatureToFiles json objects are those with keys
	 * like those: name, vname, file, files, subdir, comp.
	 * Nodes are traversed recursively.
	 */
	bool node(const picojson::value::object& obj);
	/**
	 * A "comp" element of a node is parsed via this method.
	 * Usually a component contains several (sub-)nodes.
	 */
	bool component(const picojson::value::array& obj);
	/**
	 * Return true if the dependency given by the depends string
	 * is met. kconfig_enabled is used for this. The depends string
	 * may contain "not", "and", "or" and "&feature" expressions.
	 */
	bool checkDepends(const std::string depends);
	
	bool msuccess;
	std::set<std::string> mEnabled;
	std::ostream* output_files_stream;
	bool all_features;
	int in;

	/**
	 * The directory class supports cd(subdir) and up() methods,
	 * the current directory can be accessed by "dir".
	 */
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
