/*
* Copyright 2013 David Graeff
* BSD License
*/

/**
* This program reads a kconfig output file (usually .config) and a
* file that contains the relations between activated features and files
* that belong to a feature. The output is a list of used files, depending
* on the selected features. The output is either on stdout or to a file.
* 
* Arguments are:
* -o output_filename (may be omitted to output to stdout)
* -k kconfig_output_filename (usually .config)
* -f feature_to_files_relation_filename (json format)
* -a (select all features, -k is not neccessary and ignored)
*/

#include "picojson.h"
#include "parsekconfig.h"
#include "determine_files.h"
#include <unistd.h>
#include <getopt.h>
#include <iostream>     // std::ios, std::istream, std::cout
#include <fstream>      // std::filebuf

void usage(char** argv) {
	fprintf(stderr,
	"Any of the following modes may be combined (if non-exclusive options are set).\n"
	"Mode OUTPUT DEFINITIONS: %s [-d output_file_definitions] -k kconfig_output_filename \n"
	"	If you omit output_file_definitions the standard output is used.\n"
	"	The kconfig_output_filename is the output of the kconfig tool (usually .config).\n\n"
	"Mode OUTPUT ALL FILES: %s [-o output_file] -b base_directory -f feature_to_files_relation_filename -a\n"
	"	The feature_to_files_relation_filename is a json file. The syntax is explained in readme_familymodel.md\n",
	"	The kconfig selection is ignored and all files are selected.\n\n"
	"Mode OUTPUT FILES: %s [-o output_file] -b base_directory -k kconfig_output_filename -f feature_to_files_relation_filename\n"
	"	The kconfig_output_filename is the output of the kconfig tool (usually .config).\n"
	"	The feature_to_files_relation_filename is a json file. The syntax is explained in readme_familymodel.md\n",
		argv[0]);
}

int main(int argc, char** argv) {
	std::filebuf output_files_file;
	std::filebuf output_definitions_file;
	std::string kconfigfile;
	std::string base_directory;
	std::string featureToFilesRelationfile;
	bool all_features = false;
	bool only_definitions = true;

	// parse options
	int opt;
	while ((opt = getopt(argc, argv, "b:o:d:k:f:a")) != -1) {
			switch (opt) {
			case 'o':
				output_files_file.open (optarg,std::ios::out);
				if (!output_files_file.is_open()) {
					std::cerr << "Could not open output file for write!\n";
					exit(EXIT_FAILURE);
				}
				break;
			case 'd':
				output_definitions_file.open (optarg,std::ios::out);
				if (!output_definitions_file.is_open()) {
					std::cerr << "Could not open output file for write!\n";
					exit(EXIT_FAILURE);
				}
				break;
			case 'k':
				kconfigfile = std::string(optarg);
				break;
			case 'b':
				base_directory = std::string(optarg);
				break;
			case 'f':
				only_definitions = false;
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
		std::cerr << "kconfig_output file and all-features set. I'm confused now!\n";
		usage(argv);
		exit(EXIT_FAILURE);
	}
	
	if (!kconfigfile.size() && !all_features) {
		std::cerr << "kconfig_output file not set!\n";
		usage(argv);
		exit(EXIT_FAILURE);
	}

	if (!featureToFilesRelationfile.size()) {
		std::cerr << "feature_to_files_relation file not set!\n";
		usage(argv);
		exit(EXIT_FAILURE);
	}
	
	if (!base_directory.size()) {
		std::cerr << "base directory not set!\n";
		usage(argv);
		exit(EXIT_FAILURE);
	}
	
	if (!output_files_file.is_open() && !output_definitions_file.is_open()) {
		std::cerr << "You can't output files and definitions to standard output. Call the application twice or use files!\n";
		usage(argv);
		exit(EXIT_FAILURE);
	}

	// Output stream
	std::ostream output_files_stream(output_files_file.is_open() ? &output_files_file : std::cout.rdbuf());
	std::ostream output_definitions_stream(output_definitions_file.is_open() ? &output_definitions_file : std::cout.rdbuf());
	
	// read kconfig file
	kconfigparser kparser;
	if (!all_features && !kparser.load_kconfig_file(kconfigfile)) {
		std::cerr << "Failed to parse kconfig output file!\n";
		exit(EXIT_FAILURE);
	}
	
	// read feature_to_files_relation file
	std::filebuf fb;
	if (!fb.open (featureToFilesRelationfile.c_str(),std::ios::in)) {
		std::cerr << "Failed to open featureToFilesRelation file!\n";
		exit(EXIT_FAILURE);
	}
	
	int res = 0;
	{
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
		
		FeatureToFiles ftf(base_directory, featureToFilesRelation.get<picojson::object>());
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
	
	std::cerr << "finished" << std::endl;
	res = 1;
	return res;
}

