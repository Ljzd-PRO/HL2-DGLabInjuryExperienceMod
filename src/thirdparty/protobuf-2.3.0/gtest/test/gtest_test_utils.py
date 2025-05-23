#!/usr/bin/env python
#
# Copyright 2006, Google Inc.
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

"""Unit test utilities for Google C++ Testing Framework."""

__author__ = 'wan@google.com (Zhanyong Wan)'

import atexit
import os
import shutil
import sys
import tempfile
import unittest
_test_module = unittest

# Suppresses the 'Import not at the top of the file' lint complaint.
# pylint: disable-msg=C6204
try:
  import subprocess
  _SUBPROCESS_MODULE_AVAILABLE = True
except:
  import popen2
  _SUBPROCESS_MODULE_AVAILABLE = False
# pylint: enable-msg=C6204


IS_WINDOWS = os.name == 'nt'
IS_CYGWIN = os.name == 'posix' and 'CYGWIN' in os.uname()[0]

# Here we expose a class from a particular module, depending on the
# environment. The comment suppresses the 'Invalid variable name' lint
# complaint.
TestCase = _test_module.TestCase  # pylint: disable-msg=C6409

# Initially maps a flag to its default value. After
# _ParseAndStripGTestFlags() is called, maps a flag to its actual value.
_flag_map = {'gtest_source_dir': os.path.dirname(sys.argv[0]),
             'gtest_build_dir': os.path.dirname(sys.argv[0])}
_gtest_flags_are_parsed = False


def _ParseAndStripGTestFlags(argv):
  """Parses and strips Google Test flags from argv.  This is idempotent."""

  # Suppresses the lint complaint about a global variable since we need it
  # here to maintain module-wide state.
  global _gtest_flags_are_parsed  # pylint: disable-msg=W0603
  if _gtest_flags_are_parsed:
    return

  _gtest_flags_are_parsed = True
  for flag in _flag_map:
    # The environment variable overrides the default value.
    if flag.upper() in os.environ:
      _flag_map[flag] = os.environ[flag.upper()]

    # The command line flag overrides the environment variable.
    i = 1  # Skips the program name.
    while i < len(argv):
      prefix = '--' + flag + '='
      if argv[i].startswith(prefix):
        _flag_map[flag] = argv[i][len(prefix):]
        del argv[i]
        break
      else:
        # We don't increment i in case we just found a --gtest_* flag
        # and removed it from argv.
        i += 1


def GetFlag(flag):
  """Returns the value of the given flag."""

  # In case GetFlag() is called before Main(), we always call
  # _ParseAndStripGTestFlags() here to make sure the --gtest_* flags
  # are parsed.
  _ParseAndStripGTestFlags(sys.argv)

  return _flag_map[flag]


def GetSourceDir():
  """Returns the absolute path of the directory where the .py files are."""

  return os.path.abspath(GetFlag('gtest_source_dir'))


def GetBuildDir():
  """Returns the absolute path of the directory where the test binaries are."""

  return os.path.abspath(GetFlag('gtest_build_dir'))


_temp_dir = None

def _RemoveTempDir():
  if _temp_dir:
    shutil.rmtree(_temp_dir, ignore_errors=True)

atexit.register(_RemoveTempDir)


def GetTempDir():
  """Returns a directory for temporary files."""

  global _temp_dir
  if not _temp_dir:
    _temp_dir = tempfile.mkdtemp()
  return _temp_dir


def GetTestExecutablePath(executable_name):
  """Returns the absolute path of the test binary given its name.

  The function will print a message and abort the program if the resulting file
  doesn't exist.

  Args:
    executable_name: name of the test binary that the test script runs.

  Returns:
    The absolute path of the test binary.
  """

  path = os.path.abspath(os.path.join(GetBuildDir(), executable_name))
  if (IS_WINDOWS or IS_CYGWIN) and not path.endswith('.exe'):
    path += '.exe'

  if not os.path.exists(path):
    message = (
        'Unable to find the test binary. Please make sure to provide path\n'
        'to the binary via the --gtest_build_dir flag or the GTEST_BUILD_DIR\n'
        'environment variable. For convenient use, invoke this script via\n'
        'mk_test.py.\n'
        # TODO(vladl@google.com): change mk_test.py to test.py after renaming
        # the file.
        'Please run mk_test.py -h for help.')
    print >> sys.stderr, message
    sys.exit(1)

  return path


def GetExitStatus(exit_code):
  """Returns the argument to exit(), or -1 if exit() wasn't called.

  Args:
    exit_code: the result value of os.system(command).
  """

  if os.name == 'nt':
    # On Windows, os.WEXITSTATUS() doesn't work and os.system() returns
    # the argument to exit() directly.
    return exit_code
  else:
    # On Unix, os.WEXITSTATUS() must be used to extract the exit status
    # from the result of os.system().
    if os.WIFEXITED(exit_code):
      return os.WEXITSTATUS(exit_code)
    else:
      return -1


class Subprocess:
  def __init__(self, command, working_dir=None, capture_stderr=True):
    """Changes into a specified directory, if provided, and executes a command.
    Restores the old directory afterwards. Execution results are returned
    via the following attributes:
      terminated_by_sygnal   True iff the child process has been terminated
                             by a signal.
      signal                 Sygnal that terminated the child process.
      exited                 True iff the child process exited normally.
      exit_code              The code with which the child proces exited.
      output                 Child process's stdout and stderr output
                             combined in a string.

    Args:
      command:        The command to run, in the form of sys.argv.
      working_dir:    The directory to change into.
      capture_stderr: Determines whether to capture stderr in the output member
                      or to discard it.
    """

    # The subprocess module is the preferrable way of running programs
    # since it is available and behaves consistently on all platforms,
    # including Windows. But it is only available starting in python 2.4.
    # In earlier python versions, we revert to the popen2 module, which is
    # available in python 2.0 and later but doesn't provide required
    # functionality (Popen4) under Windows. This allows us to support Mac
    # OS X 10.4 Tiger, which has python 2.3 installed.
    if _SUBPROCESS_MODULE_AVAILABLE:
      if capture_stderr:
        stderr = subprocess.STDOUT
      else:
        stderr = subprocess.PIPE

      p = subprocess.Popen(command,
                           stdout=subprocess.PIPE, stderr=stderr,
                           cwd=working_dir, universal_newlines=True)
      # communicate returns a tuple with the file obect for the child's
      # output.
      self.output = p.communicate()[0]
      self._return_code = p.returncode
    else:
      old_dir = os.getcwd()
      try:
        if working_dir is not None:
          os.chdir(working_dir)
        if capture_stderr:
          p = popen2.Popen4(command)
        else:
          p = popen2.Popen3(command)
        p.tochild.close()
        self.output = p.fromchild.read()
        ret_code = p.wait()
      finally:
        os.chdir(old_dir)
      # Converts ret_code to match the semantics of
      # subprocess.Popen.returncode.
      if os.WIFSIGNALED(ret_code):
        self._return_code = -os.WTERMSIG(ret_code)
      else:  # os.WIFEXITED(ret_code) should return True here.
        self._return_code = os.WEXITSTATUS(ret_code)

    if self._return_code < 0:
      self.terminated_by_signal = True
      self.exited = False
      self.signal = -self._return_code
    else:
      self.terminated_by_signal = False
      self.exited = True
      self.exit_code = self._return_code


def Main():
  """Runs the unit test."""

  # We must call _ParseAndStripGTestFlags() before calling
  # unittest.main().  Otherwise the latter will be confused by the
  # --gtest_* flags.
  _ParseAndStripGTestFlags(sys.argv)
  _test_module.main()