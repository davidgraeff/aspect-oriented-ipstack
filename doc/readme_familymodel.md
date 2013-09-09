Family model syntax
===================
[back](../readme.md)

A family model file format needs to fullfil these requirements:
* List project files (relative directories)
* Group files and add dependencies for those groups
* A dependeny string should allow basic operators expectially AND, OR, NOT

We decided to use json. To explain the syntax we will look on
the following example:

	{
		"subdir" : "api",
		"depends" : "&ipstack",
		"files" : ["Setup.h"]
		"comp" : [
		{
			"depends" : "&ipstack_udp",
			"file" : "UDP_Socket.h"
		},
		{
			"depends" : "&ipstack_tcp",
			"subdir" : "tcp",
			"files" : ["TCP_Socket.h"]
		}
		]
	}

File names are specified relativly. The program that reads in a family model
file therefore needs to know the base directory, we call this working directory.

We use the term "component" instead of *group of files*. A component is
declarated by a json object (`{ }`) and may have these memebers:
* subdir: Enters a sub directory. This changes the working directory for this and all children components.
* file: One file. The filename is relative to the current working directory.
* files: List of files. The filenames are relative to the current working directory.
* depends: A dependencies string. Use the ampersand `&` character to declare a dependency
    to one of the options of the **feature model**. You may also declare dependencies to multiple
    features by using `and`, `or` or `not`.
    * OR: `&ipstack_udp or &ipstack_tcp`.
    * AND, NOT: `&ipstack_udp and not &ipstack_tcp`.
* comp: A list of sub components. A sub component is only evaluated if the ancestor components dependencies are met.
    The current working directory is inherited by the ancestor component. If you look at the component in the example
    where the "TCP_Socket.h" file is declared, you will see another subdir member. The current working directory
    for "TCP_Socket.h" is therefore `{base_directory}/api/tcp`.