Collaborate
===========
[back](../readme.md)

## Feature Model
As you already know we use kconfig for feature selection. kconfig uses the
[kconfig language](https://www.kernel.org/doc/Documentation/kbuild/kconfig-language.txt)
for describing features and dependecies. If you want to add/modify features,
you start by editing this **feature model** file.

> Related file: `libipstack/buildsystem/ipstack.fm`

## Add/remove/move files
The buildsystem have to know which files belong to feature. We do this by
using a so called **family model**. This is a json formatted file that
basically list all project files and their **feature model** dependecies.
The family models file syntax is explained in [Family Model](readme_familymodel.md).

> Related file: `libipstack/buildsystem/ipstack.json`