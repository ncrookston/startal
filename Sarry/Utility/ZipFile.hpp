#ifndef SARRY_UTILITY_ZIPFILE_HPP
#define SARRY_UTILITY_ZIPFILE_HPP

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

namespace Sarry
{
  class ZipFile
  {
  public:
    explicit ZipFile(const boost::filesystem::path& filePath);

    ~ZipFile();

    const boost::filesystem::path& getPath() const;

    /** Adds a file to this zip file.
     * @param[in] name - name of the file which may include subfolders.  E.g.,
     *   'sub_folder/tmp.txt' would create a folder inside the zip file
     *   named sub_folder with a file named tmp.txt.  There is no need to
     *   create the sub-folder first.
     * @param[in] pFile - contents of the file.
     * @param[in] size - number of bytes in pFile.
     */
    void addFile(const std::string& name, const void* pFile, std::size_t size);

  private:
    class ZipFileImpl;

    boost::scoped_ptr<ZipFileImpl> m_pZipFile;

  };//end ZipFile

}//end Sarry

#endif
