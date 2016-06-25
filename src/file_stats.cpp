#include "file_stats.h"
#include <boost/foreach.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>

double hlog(const double x)
{
  if (x == 0) {
    return 0;
  }
  return log2(x);
}

double calcEntropy(const std::vector<double>& d)
{
  double h = 0;
  BOOST_FOREACH(double x, d) {
    h += x * hlog(x);
  }
  return -h;
}

FileStats::FileStats(const std::string& filename, const boost::atomic<bool>& abort) : m_filename(filename), m_accessError(false)
{
  if (abort) {
    m_accessError = true;
    return;
  }

  std::vector<uint8_t> buffer(1024 * 1024);
  std::ifstream file(filename.c_str(), std::ios::binary);
  if (!file.is_open()) {
    m_accessError = true;
    return;
  }

  file.seekg(0, std::ios_base::end);
  m_fileSize = file.tellg();
  file.seekg(0, std::ios_base::beg);

  /* histogram buffers */
  std::vector<double> hg(256);
  std::vector<double> shg(256);
  std::vector<double> h3d_buffer(256 * 256 * 256);
  double (&h3d)[256][256][256] = *(double (*)[256][256][256])&h3d_buffer[0];

  /* divide the file into slices to generate entropy graphs */
  const uint64_t sliceCount = 256;
  const uint64_t samplesPerSlice = m_fileSize / sliceCount;
  uint64_t sampleCount = 0;

  /* keep track of the last three bytes */
  uint8_t bytes[3];
  uint64_t byteCount = 0;

  while (!file.eof()) {
    file.read((char*)&buffer[0], buffer.size());
    for (size_t i = 0; i < file.gcount(); ++i) {
      bytes[0] = bytes[1];
      bytes[1] = bytes[2];
      bytes[2] = buffer[i];

      /* update 3d histrogram */
      if (++byteCount >= 3) {
        h3d[bytes[0]][bytes[1]][bytes[2]]++;
      }

      /* update other histograms */
      hg[buffer[i]]++;
      shg[buffer[i]]++;

      /* track slices and reset if needed */
      if (++sampleCount >= samplesPerSlice) {
        BOOST_FOREACH(double& x, shg) {
          x /= sampleCount;
        }
        m_entropy1d.push_back(calcEntropy(shg));
        shg = std::vector<double>(256);
        sampleCount = 0;
      }
    }
    if (abort) {
      /* allow user to cancel long running operation */
      m_accessError = true;
      return;
    }
  }

  /* trailing bytes of 1d histogram */
  if (sampleCount) {
    BOOST_FOREACH(double& x, shg) {
      x /= sampleCount;
    }
    m_entropy1d.push_back(calcEntropy(shg));
  }

  /* project the 3d histogram so we can display it in 2d */
  double maxEntropy = 0;
  m_entropy2d = std::vector<double>(256 * 256);
  for (int x = 0; x < 256; ++x) {
    for (int y = 0; y < 256; ++y) {
      /* calculate normalizing constant */
      double z = 0;
      for (int i = 0; i < 256; ++i) {
        z += h3d[x][y][i];
      }

      if (z == 0) {
        continue;
      }

      /* entropy of this lane */
      double h = 0;
      for (int i = 0; i < 256; ++i) {
        double k = h3d[x][y][i] / z;
        h += k * hlog(k);
      }
      m_entropy2d[y*256+x] = -h;
      maxEntropy = std::max(maxEntropy, -h);
    }
  }

  /* normalize histogram */
  BOOST_FOREACH(double& x, m_entropy2d) {
    x /= maxEntropy;
  }

  /* entropy of the whole file */
  BOOST_FOREACH(double& x, hg) {
    x /= m_fileSize;
  }
  m_totalEntropy = calcEntropy(hg);
  m_histogram = hg;
}
