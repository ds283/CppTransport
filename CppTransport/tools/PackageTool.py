__author__ = 'ds283'

import os
import tarfile
import subprocess


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


def add_folder(tree_path, archive_path, archive):
    # get list of files at this level
    files = list_files(tree_path)

    # get list of directories at this level
    dirs = list_directories(tree_path)

    for file in files:

        file_lower = file.lower()

        # don't emplace CMakeLists.txt; we will do it separately using a special package-ready version
        if file_lower != 'cmakelists.txt' \
            and file_lower != '.DS_Store':

            tree_name = os.path.join(tree_path, file)
            archive_name = os.path.join(archive_path, file)

            archive.add(name=tree_name, arcname=archive_name, recursive=False)

    for dir in dirs:

        dir_lower = dir.lower()

        if dir[0] != '.' \
            and dir_lower != '.idea' \
            and dir_lower != 'build' \
            and dir_lower != 'build-clang' \
            and dir_lower != 'build-icpc' \
            and dir_lower != 'build-gcc' \
            and dir_lower != 'cmake-build-debug' \
            and dir_lower != 'build-gcc' \
            and dir_lower != 'tools' \
            and dir_lower != 'test-canonical' \
            and dir_lower != 'test-nontrivial-metric' \
            and dir_lower != 'tests' \
            and dir_lower != 'packages' \
            and dir_lower != 'bison':  # don't descend into unneeded directories

            tree_name = os.path.join(tree_path, dir)
            archive_name = os.path.join(archive_path, dir)

            # add this directory to the archive
            archive.add(name=tree_name, arcname=archive_name, recursive=False)

            # and recursively add its contents
            add_folder(tree_name, archive_name, archive)


def package_CppTransport(package_dir, archive_file, version_string):

    cwd = os.getcwd()
    abs_package_dir = os.path.join(cwd, package_dir)

    # tree_path points to the position of some object within the source tree
    tree_path = os.path.join("..", "CppTransport")

    # archive path is the corresponding position of the object within the tar archive
    archive_path = "CppTransport" + "_" + version_string

    # bison output files
    bison_parser_h = os.path.join(abs_package_dir, "y_parser.hpp")
    bison_parser_cpp = os.path.join(abs_package_dir, "y_parser.cpp")
    bison_location = os.path.join(abs_package_dir, "location.hh")
    bison_position = os.path.join(abs_package_dir, "position.hh")
    bison_stack = os.path.join(abs_package_dir, "stack.hh")
    bison_graph = os.path.join(abs_package_dir, "grammar.dot")

    # corresponding locations within the archive tree
    archive_parser_hpp = os.path.join(archive_path, "translator", "parser", "y_parser.hpp")
    archive_parser_cpp = os.path.join(archive_path, "translator", "parser", "y_parser.cpp")
    archive_location = os.path.join(archive_path, "translator", "parser", "location.hh")
    archive_position = os.path.join(archive_path, "translator", "parser", "position.hh")
    archive_stack = os.path.join(archive_path, "translator", "parser", "stack.hh")

    # bison source grammar
    tree_grammar = os.path.join(cwd, "translator", "parser", "bison", "y_parser.yy")

    # packageable CMakeLists.txt position in tree
    tree_cmakelists = os.path.join("packages", "assets", "CMakeLists.txt")

    # and corresponding position within archive
    archive_cmakelists = os.path.join(archive_path, "CMakeLists.txt")

    # write out all variables
    tarname = os.path.join(package_dir, archive_file)
    with tarfile.open(name=tarname, mode='w:gz', format=tarfile.PAX_FORMAT) as archive:
        # add top-level directory to archive
        archive.add(name=tree_path, arcname=archive_path, recursive=False)

        add_folder(tree_path, archive_path, archive)

        # now use Bison to produce the various parser files
        subprocess.check_call(
            "bison -v --output=\"{outfile}\" --graph=\"{graph}\" \"{grammar}\"".format(outfile=bison_parser_cpp,
                                                                                       grammar=tree_grammar,
                                                                                       graph=bison_graph),
            shell=True)

        archive.add(name=bison_parser_h, arcname=archive_parser_hpp)
        archive.add(name=bison_parser_cpp, arcname=archive_parser_cpp)
        archive.add(name=bison_location, arcname=archive_location)
        archive.add(name=bison_position, arcname=archive_position)
        archive.add(name=bison_stack, arcname=archive_stack)

        archive.add(name=tree_cmakelists, arcname=archive_cmakelists)

        archive.close()


def package_examples(package_dir, archive_file, version_string):

    # tree_path points to the position of some object within the source tree
    tree_path = os.path.join("..", "Documentation", "Examples")

    # archive path is the corresponding position of the object within the tar archive
    archive_path = "UserGuideExamples" + "_" + version_string

    tree_cmakelists = os.path.join(tree_path, "CMakeLists.txt")
    archive_cmakelists = os.path.join(archive_path, "CMakeLists.txt")

    tarname = os.path.join(package_dir, archive_file)
    with tarfile.open(name=tarname, mode='w:gz', format=tarfile.PAX_FORMAT) as archive:
        # add top-level directory to archive
        archive.add(name=tree_path, arcname=archive_path, recursive=False)

        add_folder(tree_path, archive_path, archive)

        archive.add(name=tree_cmakelists, arcname=archive_cmakelists)

        archive.close()


version = "2017_01_beta1_Apollo"

package_dir = os.path.join("packages", "pkg")
version_dir = os.path.join(package_dir, version)

# ensure packaging directory exists
if not os.path.exists(version_dir):
    os.makedirs(version_dir)

package_CppTransport(version_dir, "CppTransport" + "_" + version + ".tar.gz", version)

package_examples(version_dir, "UserGuideExamples" + "_" + version + ".tar.gz", version)
