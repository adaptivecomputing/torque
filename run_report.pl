#!/usr/bin/env python

import sys
from subprocess import *

coverage_file="coverage_results.file";

def get_compile_output():
  make_res=Popen(["make", "check"], stdout=PIPE, stderr=PIPE);
  res,res_err=make_res.communicate();
  res_file = open(coverage_file, 'w');
  res_file.write(res);
  res_file.close();

def main():
  get_compile_output();
  print "Summary page of the unit test run.";
  print "   Individual file code coverage and a Total";
  print "----------------------------------------------------------------------------";
  print coverage_file;
  parse_res=Popen(["./parse_cov_results.pl", coverage_file]);
  res=parse_res.communicate();
  file_res=Popen(["./cov_file_results.pl"]);
  res=file_res.communicate();

if __name__ == "__main__":
  main()
