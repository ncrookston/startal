#ifndef SARRY_UTILITY_PRINTTOSCREEN_HPP
#define SARRY_UTILITY_PRINTTOSCREEN_HPP

#include <boost/filesystem.hpp>

namespace Sarry
{
  /** Prints the given file to another file.  Path must not be a directory.
   *  This should be called prior to anything else having been printed to the
   *  screen.
   */
  void printToStdout(const boost::filesystem::path& file);

}//end Sarry

#endif

