#ifndef __FILE_STATS_H__
#define __FILE_STATS_H__

#include <boost/shared_ptr.hpp>
#include <vector>
#include <stdint.h>

class FileStats
{
public:

  typedef boost::shared_ptr<FileStats> Ref;

  FileStats(const std::string& filename);
  const std::vector<double>& entropy1d() const {return m_entropy1d;}
  const std::vector<double>& entropy2d() const {return m_entropy2d;}
  std::string filename() const {return m_filename;}
  double totalEntropy() const {return m_totalEntropy;}
  uint64_t fileSize() const {return m_fileSize;}
  bool accessError() const {return m_accessError;}

private:

  std::vector<double> m_entropy1d;
  std::vector<double> m_entropy2d; /* 256x256 */
  std::string m_filename;
  double m_totalEntropy;
  uint64_t m_fileSize;
  bool m_accessError;

};

#endif // __FILE_STATS_H__
