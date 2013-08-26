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

/**
* This program reads a kconfig output file (usually .config) and a
* file that contains the relations between activated features and files
* that belong to a feature. The output is a list of used files, depending
* on the selected features. The output is either on stdout or to a file.
* Additionally all configuration variables of .config can be written to
* to a c file as #define's.
* 
* Arguments are:
* -b base_directory (may be ommitted, "" is used then)
* -o output_filename (may be omitted to output to stdout)
* -d output_definitions_filename (may be set to "" to output to stdout)
* -k kconfig_output_filename (usually .config)
* -f feature_to_files_relation_filename (json format, may be omitted if -d is set)
* -a (select all features, -k is not neccessary and ignored)
*/

#include "picojson.h"
#include "parsekconfig.h"
#include "determine_files.h"
#include <unistd.h>
#include <getopt.h>
#include <iostream>     // std::ios, std::istream, std::cout
#include <fstream>      // std::filebuf
#include <sstream>      // std::stringstream

void usage(char** argv) {
	fprintf(stderr,
	"Any of the following modes may be combined (if non-exclusive options are set).\n"
	"Mode OUTPUT DEFINITIONS: [-d output_file_definitions] -k kconfig_output_filename \n"
	"	If you set output_file_definitions to \"\" the standard output is used.\n"
	"	The kconfig_output_filename is the output of the kconfig tool (usually .config).\n\n"
	"Mode OUTPUT ALL FILES: -o output_file [-b base_directory] -f feature_to_files_relation_filename -a\n"
	"	You may set output_file to \"\" to use the standard output.\n"
	"	You may optionally set base_directory as a prefix for all files.\n"
	"	The feature_to_files_relation_filename is a json file. The syntax is explained in readme_familymodel.md\n"
	"	The kconfig selection is ignored and all files are selected.\n\n"
	"Mode OUTPUT FILES: -o output_file [-b base_directory] -f feature_to_files_relation_filename -k kconfig_output_filename\n"
	"	The kconfig_output_filename is the output of the kconfig tool (usually .config).\n"
	"	It is used to determine the files to ouput in contrast to OUTPUT ALL FILES mode.\n"
	);
}

int main(int argc, char** argv) {
	std::filebuf output_files_file;
	std::filebuf output_definitions_file;
	std::string kconfigfile;
	std::string base_directory;
	std::string featureToFilesRelationfile;
	bool all_features = false;
	bool definitions = false;
	bool featureFiles = false;

	// parse options
	int opt;
	while ((opt = getopt(argc, argv, "b:o:d:k:f:a")) != -1) {
			switch (opt) {
			case 'o':
				featureFiles = true;
				if (strlen(optarg)>0) {
					output_files_file.open (optarg,std::ios::out);
					if (!output_files_file.is_open()) {
						std::cerr << "Could not open output file for write!\n";
						exit(EXIT_FAILURE);
					}
				}
				break;
			case 'd':
				definitions = true;
				if (strlen(optarg)>0) {
					output_definitions_file.open (optarg,std::ios::out);
					if (!output_definitions_file.is_open()) {
						std::cerr << "Could not open output file for write!\n";
						exit(EXIT_FAILURE);
					}
				}
				break;
			case 'k':
				kconfigfile = std::string(optarg);
				break;
			case 'b':
				base_directory = std::string(optarg);
				break;
			case 'f':
				featureToFilesRelationfile = std::string(optarg);
				break;
			case 'a':
				all_features = true;
				break;
			default: /* '?' */
				usage(argv);
				exit(EXIT_FAILURE);
			}
		}
	
	if (kconfigfile.size() && all_features) {
		std::cerr << "kconfig_output file and all-features set. all-features overwrite kconfig_output!\n\n";
	}
	
	if (!kconfigfile.size() && definitions) {
		std::cerr << "kconfig_output file not set and definitions expected. I'm confused now!\n\n";
		usage(argv);
		exit(EXIT_FAILURE);
	}
	
	if (!kconfigfile.size() && !all_features) {
		std::cerr << "kconfig_output file not set!\n\n";
		usage(argv);
		exit(EXIT_FAILURE);
	}

	if (!featureToFilesRelationfile.size() && (!definitions || featureFiles)) {
		std::cerr << "feature_to_files_relation file not set and also no definitions wanted. No output!\n\n";
		usage(argv);
		exit(EXIT_FAILURE);
	}
	
	if (!output_files_file.is_open() && !output_definitions_file.is_open()) {
		std::cerr << "You can't output files and definitions to standard output. Call the application twice or use files!\n\n";
		usage(argv);
		exit(EXIT_FAILURE);
	}

	// Output stream
	std::ostream output_files_stream(output_files_file.is_open() ? &output_files_file : std::cout.rdbuf());
	std::ostream output_definitions_stream(output_definitions_file.is_open() ? &output_definitions_file : std::cout.rdbuf());
	
	// read kconfig file
	kconfigparser kparser;
	if ((definitions || !all_features) && !kparser.load_kconfig_file(kconfigfile)) {
		std::cerr << "Failed to parse kconfig output file!\n";
		exit(EXIT_FAILURE);
	}
	
	if (definitions) {
		for (auto i = kparser.kconfig_keyvalue.begin(); i!=kparser.kconfig_keyvalue.end();++i) {
			const std::string& key = i->first;
			const std::string& value = i->second;
// 			double x ;
// 			std::stringstream ss;
// 			ss << value;
// 			ss >> x;
// 			if (ss.fail()) // no number
// 				output_definitions_stream << "#define " << key << " \"" << value << "\"" << std::endl;
// 			else // number
			output_definitions_stream << "#define " << key << " " << value << std::endl;
		}
	}

	int res = 0;
	if (featureFiles) {
		// read feature_to_files_relation file
		std::filebuf fb;
		if (!fb.open (featureToFilesRelationfile.c_str(),std::ios::in)) {
			std::cerr << "Failed to open featureToFilesRelation file!\n";
			exit(EXIT_FAILURE);
		}

		std::istream featureToFilesRelation_FileStream(&fb);
		picojson::value featureToFilesRelation;
		featureToFilesRelation_FileStream >> featureToFilesRelation;
		fb.close();
		
		std::string err = picojson::get_last_error();
		if (! err.empty()) {
			std::cerr << "Failed to parse featureToFilesRelation file!\n";
			std::cerr << err << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// check if the type of the value is "object"
		if (! featureToFilesRelation.is<picojson::object>()) {
			std::cerr << "featureToFilesRelation::JSON is not an object" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		FeatureToFiles ftf(base_directory, featureToFilesRelation.get<picojson::object>(),
						   kparser.kconfig_enabled, &output_files_stream, all_features);
		// obtain a const reference to the map, and print the contents
		if (! ftf.success() )
			res = 1;
	}
	
	if (output_files_file.is_open()) {
		output_files_stream.rdbuf(std::cout.rdbuf());
		output_files_file.close();
	}
	if (output_definitions_file.is_open()) {
		output_definitions_stream.rdbuf(std::cout.rdbuf());
		output_definitions_file.close();
	}
	
	return res;
}

