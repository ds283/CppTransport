__author__ = 'ds283'

import os
import collections

def list_files(path):
    # returns a list of names (with extension, without full path) of all files
    # in folder path
    files = []
    for name in os.listdir(path):
        if os.path.isfile(os.path.join(path, name)):
            if name[0] != '.':  # ignore system files beginning with a .
                files.append(name)
    return files

def list_directories(path):
    dirs = []
    for name in os.listdir(path):
        if os.path.isdir(os.path.join(path, name)):
            dirs.append(name)
    return dirs

def add_folder(path, relative_path, cmake_root, cmake_variables):

    # get list of files at this level
    files = list_files(path)

    # get list of directories at this level
    dirs = list_directories(path)

    if len(files) > 0:

        cmake_variable = '{root}_FILES'.format(root=cmake_root)

        files_with_path = [ os.path.join(relative_path, file) for file in files ]
        cmake_variables.update({ cmake_variable: files_with_path })

    for dir in dirs:

        if dir[0] != '.':     # don't descend into system directories

            add_folder(os.path.join(path, dir), os.path.join(relative_path, dir), '{root}_{leaf}'.format(root=cmake_root, leaf=dir.upper().replace("-", "_")), cmake_variables)

top_path = ".."

# get files in the top level
top_level_files = list_files(top_path)

# get directories in the top level
top_level_dirs = list_directories(top_path)

cmake_variables = { 'TOP_LEVEL_FILES': top_level_files }

for dir in top_level_dirs:

    if dir[0] != '.' \
            and dir.lower() != 'thirdparty' \
            and dir.lower() != 'cmake' \
            and dir.lower() != 'build-clang' \
            and dir.lower() != 'build-icpc' \
            and dir.lower() != 'build-gcc' \
            and dir.lower() != 'tools' \
            and dir.lower() != 'splinter' \
            and dir.lower() != 'html_assets' \
            and dir.lower() != 'test-target':  # don't descend into thirdparty directories

        add_folder(os.path.join(top_path, dir), dir, dir.upper().replace("-", "_"), cmake_variables)


# write out all variables
with open("CMakeLists.txt", "w") as output_file:

    ordered_list = collections.OrderedDict(sorted(cmake_variables.items()))

    for variable, contents in ordered_list.iteritems():

        output_file.write('set({var}\n'.format(var=variable))

        for file in contents:

            output_file.write('  {0}\n'.format(file))

        output_file.write(')\n\n')

    output_file.write('set(SOURCE_FILES\n')

    for variable in ordered_list:

        output_file.write('  ${{{0}}}\n'.format(variable))

    output_file.write(')\n')
