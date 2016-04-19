#!/usr/bin/python

import os, sys
import time
import re

debug = False
gcov_suffix = '.gcov'
gcov_suffix_offset = -5
coverage_map = {}
file_set = []

class LineInfo:
    def __init__(self, raw_line):
        self.is_blank = False
        trimmed = raw_line.strip()
        
        if debug == True:
            print 'raw: ' + raw_line
            print 'trimmed: \'' + trimmed + '\''

        nums = re.findall('\d+', trimmed)
        self.num_exercized = 0

        if trimmed[0] == '#':
            self.line_num = int(nums[0])
        elif trimmed[0] == '-' or trimmed[0] == '=':
            self.is_blank = True
            self.line_num = int(nums[0])
        else:
            self.num_exercized = int(nums[0])
            self.line_num = int(nums[1])

        if debug == True:
            if self.is_blank == False:
                print 'Line ' + str(self.line_num) + ' has been exercized ' + str(self.num_exercized) + ' times.'
            else:
                print 'Line ' + str(self.line_num) + ' is blank: ' + trimmed

    def add_to_exercized_count(self, to_add):
        self.num_exercized += to_add

    def exercized_times(self):
        return self.num_exercized

    def is_exercized(self):
        return self.num_exercized != 0

    def blank_line(self):
        return self.is_blank


class CoverageFile:
    def __init__(self, filename):
        self.path = filename
        self.lines = []
        self.exercized_lines = 0
        self.non_blank_lines = 0
        tokens = filename.split('/')
        last = tokens[len(tokens) - 1]
        if last.endswith(gcov_suffix):
            self.filename = last[:gcov_suffix_offset]
        else:
            self.filename = last

    def parse_file(self):
        f = open(self.path, 'r')
        content = f.readlines()

        for line in content:
            l = LineInfo(line)
            self.lines.append(l)

            if l.is_exercized():
                self.exercized_lines += 1
                self.non_blank_lines += 1
            elif not l.blank_line():
                self.non_blank_lines += 1

    def append_file(self, file_to_append):
        f = open(file_to_append, 'r')
        content = f.readlines()
        current_line = 0
       
        for line in content:
            l = LineInfo(line)
            self.lines[current_line].add_to_exercized_count(l.exercized_times())
            current_line += 1

    def get_total_executable_lines(self):
        return self.non_blank_lines

    def get_total_lines(self):
        return len(self.lines)

    def get_total_blank_lines(self):
        return len(self.lines) - self.non_blank_lines

    def get_total_exercized_lines(self):
        return self.exercized_lines

    def get_filename(self):
        return self.filename

# Get a list of our files so that we aren't reporting coverage for the STL
def populate_file_set(start_path):
    for root, dirs, files in os.walk(start_path):
        for d in dirs:
            next_path = start_path + d
            populate_file_set(next_path)

        for f in files:
            if f.endswith('.c'):
                file_set.append(f)
            elif f.endswith('.cpp'):
                file_set.append(f)
            elif f.endswith('.h'):
                file_set.append(f)
            elif f.endswith('.hpp'):
                file_set.append(f)

def look_for_coverage_files(start_path):
    for root, dirs, files in os.walk(start_path):
        for d in dirs:
            next_path = start_path + d
            look_for_coverage_files(next_path)

        for f in files:
            if f.endswith(gcov_suffix):
                base_name = f[:gcov_suffix_offset]

                # Only add files with base names in our file set
                if base_name in file_set:
                    path = root + '/' + f
                    if coverage_map.get(base_name) == None:
                        if debug == True:
                            print 'Parsing file ' + base_name + ' for the first time.'
                        cf = CoverageFile(path)
                        cf.parse_file()
                        coverage_map[base_name] = cf
                        
                        if debug == True:
                            print base_name + ' has ' + str(cf.get_total_lines()) + ' lines.'
                    else:
                        if debug == True:
                            print 'Appending file ' + base_name + '.'
                        coverage_map[base_name].append_file(path)
    

start_path = os.path.curdir + '/src/'
populate_file_set(start_path)
look_for_coverage_files(start_path)

total_lines = 0
total_exercized_lines = 0.0
alphabetized = []

for key in coverage_map.iterkeys():
    one_total = coverage_map[key].get_total_executable_lines()
    one_exercized = coverage_map[key].get_total_exercized_lines()
    total_lines += one_total
    total_exercized_lines += one_exercized
    pct = 100.0 * one_exercized / one_total
    alphabetized.append([key, one_exercized, one_total, pct])

alphabetized.sort()

for entry in alphabetized:
    print "%35s covered %d of %d lines for %.02f%%" % (tuple(entry))

total_pct = 100.0 * total_exercized_lines / total_lines
print "\n"
print "In total covered %d of %d lines in %d files for %.02f%% coverage" % (total_exercized_lines, total_lines, len(coverage_map), total_pct)

            
