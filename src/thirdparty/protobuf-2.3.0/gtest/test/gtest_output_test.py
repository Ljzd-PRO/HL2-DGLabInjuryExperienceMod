#!/usr/bin/env python
#
# Copyright 2008, Google Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""Tests the text output of Google C++ Testing Framework.

SYNOPSIS
       gtest_output_test.py --gtest_build_dir=BUILD/DIR --gengolden
         # where BUILD/DIR contains the built gtest_output_test_ file.
       gtest_output_test.py --gengolden
       gtest_output_test.py
"""

__author__ = 'wan@google.com (Zhanyong Wan)'

import os
import re
import sys
import gtest_test_utils


# The flag for generating the golden file
GENGOLDEN_FLAG = '--gengolden'

IS_WINDOWS = os.name == 'nt'

if IS_WINDOWS:
  GOLDEN_NAME = 'gtest_output_test_golden_win.txt'
else:
  GOLDEN_NAME = 'gtest_output_test_golden_lin.txt'

PROGRAM_PATH = gtest_test_utils.GetTestExecutablePath('gtest_output_test_')

# At least one command we exercise must not have the
# --gtest_internal_skip_environment_and_ad_hoc_tests flag.
COMMAND_LIST_TESTS = ({}, [PROGRAM_PATH, '--gtest_list_tests'])
COMMAND_WITH_COLOR = ({}, [PROGRAM_PATH, '--gtest_color=yes'])
COMMAND_WITH_TIME = ({}, [PROGRAM_PATH,
                          '--gtest_print_time',
                          '--gtest_internal_skip_environment_and_ad_hoc_tests',
                          '--gtest_filter=FatalFailureTest.*:LoggingTest.*'])
COMMAND_WITH_DISABLED = (
    {}, [PROGRAM_PATH,
         '--gtest_also_run_disabled_tests',
         '--gtest_internal_skip_environment_and_ad_hoc_tests',
         '--gtest_filter=*DISABLED_*'])
COMMAND_WITH_SHARDING = (
    {'GTEST_SHARD_INDEX': '1', 'GTEST_TOTAL_SHARDS': '2'},
    [PROGRAM_PATH,
     '--gtest_internal_skip_environment_and_ad_hoc_tests',
     '--gtest_filter=PassingTest.*'])

GOLDEN_PATH = os.path.join(gtest_test_utils.GetSourceDir(), GOLDEN_NAME)


def ToUnixLineEnding(s):
  """Changes all Windows/Mac line endings in s to UNIX line endings."""

  return s.replace('\r\n', '\n').replace('\r', '\n')


def RemoveLocations(test_output):
  """Removes all file location info from a Google Test program's output.

  Args:
       test_output:  the output of a Google Test program.

  Returns:
       output with all file location info (in the form of
       'DIRECTORY/FILE_NAME:LINE_NUMBER: 'or
       'DIRECTORY\\FILE_NAME(LINE_NUMBER): ') replaced by
       'FILE_NAME:#: '.
  """

  return re.sub(r'.*[/\\](.+)(\:\d+|\(\d+\))\: ', r'\1:#: ', test_output)


def RemoveStackTraceDetails(output):
  """Removes all stack traces from a Google Test program's output."""

  # *? means "find the shortest string that matches".
  return re.sub(r'Stack trace:(.|\n)*?\n\n',
                'Stack trace: (omitted)\n\n', output)


def RemoveStackTraces(output):
  """Removes all traces of stack traces from a Google Test program's output."""

  # *? means "find the shortest string that matches".
  return re.sub(r'Stack trace:(.|\n)*?\n\n', '', output)


def RemoveTime(output):
  """Removes all time information from a Google Test program's output."""

  return re.sub(r'\(\d+ ms', '(? ms', output)


def RemoveTestCounts(output):
  """Removes test counts from a Google Test program's output."""

  output = re.sub(r'\d+ tests, listed below',
                  '? tests, listed below', output)
  output = re.sub(r'\d+ FAILED TESTS',
                  '? FAILED TESTS', output)
  output = re.sub(r'\d+ tests from \d+ test cases',
                  '? tests from ? test cases', output)
  output = re.sub(r'\d+ tests from ([a-zA-Z_])',
                  r'? tests from \1', output)
  return re.sub(r'\d+ tests\.', '? tests.', output)


def RemoveMatchingTests(test_output, pattern):
  """Removes output of specified tests from a Google Test program's output.

  This function strips not only the beginning and the end of a test but also
  all output in between.

  Args:
    test_output:       A string containing the test output.
    pattern:           A regex string that matches names of test cases or
                       tests to remove.

  Returns:
    Contents of test_output with tests whose names match pattern removed.
  """

  test_output = re.sub(
      r'.*\[ RUN      \] .*%s(.|\n)*?\[(  FAILED  |       OK )\] .*%s.*\n' % (
          pattern, pattern),
      '',
      test_output)
  return re.sub(r'.*%s.*\n' % pattern, '', test_output)


def NormalizeOutput(output):
  """Normalizes output (the output of gtest_output_test_.exe)."""

  output = ToUnixLineEnding(output)
  output = RemoveLocations(output)
  output = RemoveStackTraceDetails(output)
  output = RemoveTime(output)
  return output


def GetShellCommandOutput(env_cmd):
  """Runs a command in a sub-process, and returns its output in a string.

  Args:
    env_cmd: The shell command. A 2-tuple where element 0 is a dict of extra
             environment variables to set, and element 1 is a string with
             the command and any flags.

  Returns:
    A string with the command's combined standard and diagnostic output.
  """

  # Spawns cmd in a sub-process, and gets its standard I/O file objects.
  # Set and save the environment properly.
  old_env_vars = dict(os.environ)
  os.environ.update(env_cmd[0])
  p = gtest_test_utils.Subprocess(env_cmd[1])

  # Changes made by os.environ.clear are not inheritable by child processes
  # until Python 2.6. To produce inheritable changes we have to delete
  # environment items with the del statement.
  for key in os.environ.keys():
    del os.environ[key]
  os.environ.update(old_env_vars)

  return p.output


def GetCommandOutput(env_cmd):
  """Runs a command and returns its output with all file location
  info stripped off.

  Args:
    env_cmd:  The shell command. A 2-tuple where element 0 is a dict of extra
              environment variables to set, and element 1 is a string with
              the command and any flags.
  """

  # Disables exception pop-ups on Windows.
  os.environ['GTEST_CATCH_EXCEPTIONS'] = '1'
  return NormalizeOutput(GetShellCommandOutput(env_cmd))


def GetOutputOfAllCommands():
  """Returns concatenated output from several representative commands."""

  return (GetCommandOutput(COMMAND_WITH_COLOR) +
          GetCommandOutput(COMMAND_WITH_TIME) +
          GetCommandOutput(COMMAND_WITH_DISABLED) +
          GetCommandOutput(COMMAND_WITH_SHARDING))


test_list = GetShellCommandOutput(COMMAND_LIST_TESTS)
SUPPORTS_DEATH_TESTS = 'DeathTest' in test_list
SUPPORTS_TYPED_TESTS = 'TypedTest' in test_list
SUPPORTS_THREADS = 'ExpectFailureWithThreadsTest' in test_list
SUPPORTS_STACK_TRACES = False

CAN_GENERATE_GOLDEN_FILE = SUPPORTS_DEATH_TESTS and SUPPORTS_TYPED_TESTS


class GTestOutputTest(gtest_test_utils.TestCase):
  def RemoveUnsupportedTests(self, test_output):
    if not SUPPORTS_DEATH_TESTS:
      test_output = RemoveMatchingTests(test_output, 'DeathTest')
    if not SUPPORTS_TYPED_TESTS:
      test_output = RemoveMatchingTests(test_output, 'TypedTest')
    if not SUPPORTS_THREADS:
      test_output = RemoveMatchingTests(test_output,
                                        'ExpectFailureWithThreadsTest')
      test_output = RemoveMatchingTests(test_output,
                                        'ScopedFakeTestPartResultReporterTest')
      test_output = RemoveMatchingTests(test_output,
                                        'WorksConcurrently')
    if not SUPPORTS_STACK_TRACES:
      test_output = RemoveStackTraces(test_output)

    return test_output

  def testOutput(self):
    output = GetOutputOfAllCommands()

    golden_file = open(GOLDEN_PATH, 'rb')
    # A mis-configured source control system can cause \r appear in EOL
    # sequences when we read the golden file irrespective of an operating
    # system used. Therefore, we need to strip those \r's from newlines
    # unconditionally.
    golden = ToUnixLineEnding(golden_file.read())
    golden_file.close()

    # We want the test to pass regardless of certain features being
    # supported or not.
    if CAN_GENERATE_GOLDEN_FILE:
      self.assert_(golden == output)
    else:
      normalized_actual = RemoveTestCounts(output)
      normalized_golden = RemoveTestCounts(self.RemoveUnsupportedTests(golden))

      # This code is very handy when debugging test differences so I left it
      # here, commented.
      # open(os.path.join(
      #     gtest_test_utils.GetSourceDir(),
      #     '_gtest_output_test_normalized_actual.txt'), 'wb').write(
      #         normalized_actual)
      # open(os.path.join(
      #     gtest_test_utils.GetSourceDir(),
      #     '_gtest_output_test_normalized_golden.txt'), 'wb').write(
      #         normalized_golden)

      self.assert_(normalized_golden == normalized_actual)


if __name__ == '__main__':
  if sys.argv[1:] == [GENGOLDEN_FLAG]:
    if CAN_GENERATE_GOLDEN_FILE:
      output = GetOutputOfAllCommands()
      golden_file = open(GOLDEN_PATH, 'wb')
      golden_file.write(output)
      golden_file.close()
    else:
      message = (
          """Unable to write a golden file when compiled in an environment
that does not support all the required features (death tests""")
      if IS_WINDOWS:
        message += (
            """\nand typed tests). Please check that you are using VC++ 8.0 SP1
or higher as your compiler.""")
      else:
        message += """\nand typed tests).  Please generate the golden file
using a binary built with those features enabled."""

      sys.stderr.write(message)
      sys.exit(1)
  else:
    gtest_test_utils.Main()