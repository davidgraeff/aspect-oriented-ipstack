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
*/

#include "picojson.h"
#include "parsekconfig.h"
#include <unistd.h>
#include <getopt.h>
#include <iostream>     // std::ios, std::istream, std::cout
#include <fstream>      // std::filebuf

void usage(char** argv) {
	fprintf(stderr, "Usage: %s [-o output_file] -k kconfig_output_filename -f feature_to_files_relation_filename\n"
	"	The kconfig_output_filename is the output of the kconfig tool (usually .config).\n"
	"	The feature_to_files_relation_filename is a json file. The syntax is explained in readme_familymodel.md\n",
		argv[0]);
}

int main(int argc, char** argv) {
	std::string outputfile;
	std::string kconfigfile;
	std::string featureToFilesRelationfile;

	// parse options
	int opt;
	while ((opt = getopt(argc, argv, "o:k:f:")) != -1) {
			switch (opt) {
			case 'o':
				outputfile = std::string(optarg);
				break;
			case 'k':
				kconfigfile = std::string(optarg);
				break;
			case 'f':
				featureToFilesRelationfile = std::string(optarg);
				break;
			default: /* '?' */
				usage(argv);
				exit(EXIT_FAILURE);
			}
		}
		
	if (!kconfigfile.size()) {
		std::cerr << "kconfig_output file not set!\n";
		usage(argv);
		exit(EXIT_FAILURE);
	}

	if (!featureToFilesRelationfile.size()) {
		std::cerr << "feature_to_files_relation file not set!\n";
		usage(argv);
		exit(EXIT_FAILURE);
	}

	// Output stream
	std::ofstream internal_out;
	
	if (outputfile.size()) {
		internal_out.open (outputfile.c_str(),std::ios::out);
		if (!internal_out.is_open()) {
			std::cerr << "Could not open output file for write!\n";
			exit(EXIT_FAILURE);
		}
		std::cout.rdbuf(internal_out.rdbuf());
		// From here on all std::out is written to a file
	}

	// print out collected filenames
	std::cerr << "Output file: " << outputfile << "\n" << "kconfig_output file: " << kconfigfile << "\n" << "feature_to_files_relation file: " << featureToFilesRelationfile << std::endl;
	
	// read kconfig file
	if (!kconfigparser::load_kconfig_file(kconfigfile)) {
		std::cerr << "Failed to parse kconfig output file!\n";
		exit(EXIT_FAILURE);
	}
	
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
	
	return 0;
}