#!/usr/bin/env python

import sys
import os
import glob
import subprocess

def remove_build_dirs(the_list):
  dot_dirs = ['.svn', '.deps', '.libs']
  for f in dot_dirs:
    try:
      the_list.remove(f)
    except ValueError:
      pass
  return the_list

def get_testdirs(dir):
  flist = os.listdir(dir)
  tmp_list = flist[:]
  tmp_list = remove_build_dirs(tmp_list)
  for f in flist:
    check_val = dir
    check_val += "/"
    check_val += f
    if os.path.isdir(check_val) != True:
      try:
        tmp_list.remove(f)
      except ValueError:
        pass
  return tmp_list

def merge_lists(orig_list, path, new_list):
  for item in new_list:
    new_item = item
    if path != None:
      new_item = path
      new_item += "/"
      new_item += item
    if orig_list == None:
      orig_list = [new_item]
    else:
      orig_list.append(new_item)

def add_c_file_list(dir, src_list):
  path = dir
  path += "/*.c"
  list = glob.glob(path)
  merge_lists(src_list, None, list)

def get_data(dir, dir_list, src_list):
  flist = os.listdir(dir)
  tmp_list = flist[:]
  tmp_list = remove_build_dirs(tmp_list)
  for f in flist:
    check_val = dir
    check_val += "/"
    check_val += f
    if os.path.isdir(check_val) != True:
      try:
        tmp_list.remove(f)
      except ValueError:
        pass

  if tmp_list == None:
    return None

  for pos in range(len(tmp_list)):
    tmp_dir = dir
    tmp_dir += "/"
    tmp_dir += tmp_list[pos]
    if "test" in tmp_list[pos]:
      add_c_file_list(dir, src_list)
      tmp_entries = get_testdirs(tmp_dir)
      if tmp_entries != None:
        test_dir_list = merge_lists(dir_list, tmp_dir, tmp_entries)
    else:
      tmp_entries = get_data(tmp_dir, dir_list, src_list)
      if tmp_entries != None:
        test_dir_list = merge_lists(dir_list, None, tmp_entries)

# Return a list of directores in the unit test directory, each directory representing a src file
def generate_data(dir, dir_list, src_list):
  get_data(dir, dir_list, src_list)
  return

  # Get a full directory list, and a working copy to prune
  flist = os.listdir(dir)
  worklist = flist[:]

  # Get rid of non-directories
  for f in flist:
    if os.path.isdir(f) != True:
      try:
        worklist.remove(f)
      except ValueError:
        pass

  worklist = remove_extra_test(worklist)

  flist = []
  for f in worklist:
    flist.append("../" + f + ".c")

  return flist

def remove_extra_test(list):
    # Get rid of known overhead directories
    extras = ['autom4te.cache', 'm4']
    for f in extras:
        try:
            list.remove(f)
        except ValueError:
            pass

    return list

def generate_harness_line_counts(list, ext):
    counts = {}

    for f in list:
        x_files = f
        x_files += ext
        cmd = "wc -l " + x_files
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, errors = p.communicate()

        item = output.split()

        # only process lines that have two fields - ie skip blank or 1 field lines
        # We expect lines:  <line count>   <file-name>
        if len(item) >= 2:
          counts[item[1]] = item[0]

    return counts

def generate_src_line_counts(list):
    counts = {}

    for f in list:
        cmd = "wc -l " + f 
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, errors = p.communicate()

        item = output.split()

        # only process lines that have two fields - ie skip blank or 1 field lines
        # We expect lines:  <line count>   <file-name>
        if len(item) >= 2:
          counts[item[1]] = item[0]

    return counts


def main():

    print "Information on the number of C files in this source tree and the number of File Test Harnesses for those files"
    print "--------------------------------------------------------------------------------------------------------------"
    dir_list = [];
    src_list = [];

    generate_data("src", dir_list, src_list)
#    print "dir list is:"
#    print dir_list
#    print "src list is:"
#    print src_list

    sizeTotal = len(src_list)
    sizeTests = len(dir_list)
    # The following line functions that have only a main that has a single call.
    # src/cmds/qsub.c
    # src/daemon_client/trq_main.c
    # And files that have no function implementations.
    # src/server/job_attr_def.c
    # src/server/node_attr_def.c
    # src/server/queue_attr_def.c
    # src/server/svr_attr_def.c
    # src/lib/Libifl/PBS_data.c
    # src/lib/Libdis/dis.c
    sizeTotal = sizeTotal - 8;

    print "Total C files: %s  Files with unit tests: %s   Percent of files with a File Test Harness: %s%%" % (sizeTotal,sizeTests, (sizeTests * 100) / sizeTotal)
    print ""


    # Get a dict of files and their wc -l output
    h_testCounts = generate_harness_line_counts(dir_list, "/*.h")
    c_testCounts = generate_harness_line_counts(dir_list, "/*.c")
    srcCounts = generate_src_line_counts(src_list)

    testTotal = 0
    for i in h_testCounts:
        testTotal += int(h_testCounts[i])

    for i in c_testCounts:
        testTotal += int(c_testCounts[i])

    print "Harness line count (wc -l) is:  %s" % str(testTotal)

    srcTotal = 0
    for i in srcCounts:
        srcTotal += int(srcCounts[i])

    print "Total   line count (wc -l) is:  %s" % str(srcTotal)

    percent = (float(testTotal) / float(srcTotal)) * 100.
    print "File line coverage is:          %.2f%%" % percent

    print "==================================================================="
    print "It should be noted that if the value of \'Files accounted for:\'"
    print "does not match the \'Files with unit tests:\' value there are broken unit tests"
    print "This will NOT generate correct numbers unless all the unit tests that are written function"


if __name__ == "__main__":
    main()
