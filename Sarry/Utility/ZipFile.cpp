
#include "ZipFile.hpp"
#include "contrib/minizip/zip.h"

class Sarry::ZipFile::ZipFileImpl
{
public:
  ZipFileImpl(const boost::filesystem::path& filePath)
    : m_filePath(filePath),
      m_zip(zipOpen(filePath.string().c_str(), APPEND_STATUS_CREATE))
  {}

  ~ZipFileImpl()
  { zipClose(m_zip, NULL); }

  const boost::filesystem::path& getPath() const
  { return m_filePath; }

  /** Adds a file to this zip file.
   * @param[in] name - name of the file which may include subfolders.  E.g.,
   *   'sub_folder/tmp.txt' would create a folder inside the zip file
   *   named sub_folder with a file named tmp.txt.  There is no need to
   *   create the sub-folder first.
   * @param[in] pFile - contents of the file.
   * @param[in] size - number of bytes in pFile.
   */
  void addFile(const std::string& name, const void* pFile, std::size_t size)
  {
    zipOpenNewFileInZip(m_zip, name.c_str(), NULL, NULL, 0, NULL, 0, NULL,
        Z_DEFLATED, Z_DEFAULT_COMPRESSION);
    zipWriteInFileInZip(m_zip, pFile, static_cast<unsigned int>(size));
    zipCloseFileInZip(m_zip);
  }

private:
  const boost::filesystem::path m_filePath;
  zipFile m_zip;
};

Sarry::ZipFile::ZipFile(const boost::filesystem::path& filePath)
  : m_pZipFile(new ZipFileImpl(filePath))
{}

Sarry::ZipFile::~ZipFile()
{}

const boost::filesystem::path& Sarry::ZipFile::getPath() const
{ return m_pZipFile->getPath(); }

void Sarry::ZipFile::addFile(const std::string& name,
    const void* pFile, std::size_t size)
{ return m_pZipFile->addFile(name, pFile, size); }

