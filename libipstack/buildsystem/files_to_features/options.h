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

#include <getopt.h>
#include <unistd.h>
#include <fstream>      // std::filebuf
#include <sstream>      // std::stringstream
#include <iostream>     // std::ios, std::istream, std::cout

class Options {
public:
    std::string kconfigfile;
    std::string base_directory;
    std::string featureToFilesRelationfile;
    bool failed;

    Options(int argc, char* argv[]) {
        failed = true;
        /*************** parse options ***************/
        int opt;
        while ((opt = getopt(argc, argv, "b:o:k:f:")) != -1) {
                switch (opt) {
                case 'k':
                    kconfigfile = std::string(optarg);
                    break;
                case 'b':
                    base_directory = std::string(optarg);
                    break;
                case 'f':
                    featureToFilesRelationfile = std::string(optarg);
                    break;
                default: /* '?' */
                    usage();
                    return;
                }
            }

        /*************** error handling ***************/
        if (!kconfigfile.size()) {
            std::cerr << "kconfig_output file not set!\n\n";
            usage();
            return;
        }

        if (!featureToFilesRelationfile.size()) {
            std::cerr << "feature_to_files_relation file not set!\n\n";
            usage();
            return;
        }
        
        if (!base_directory.size()) {
            std::cerr << "base_directory not set!\n\n";
            usage();
            return;
        }

        failed = false;
    }

    ~Options() {
    }

    void usage() {
        fprintf(stderr,
        "Usage: -o output_file -b base_directory -f files_to_features file -k kconfig_input_filename\n"
        "	The kconfig_input_filename is the input of the kconfig tool (usually filename.fm).\n"
        );
    }

};
