#ifndef MARKER_EXTRACTOR_HPP
#define MARKER_EXTRACTOR_HPP

#include <stdexcept>
#include <string>
#include <vector>

#include <boost/scoped_ptr.hpp>

#include <ARToolKitPlus/TrackerSingleMarker.h>
#include <opencv/cv.h>

#include "scene.hpp"
#include "silverTypes.hpp"

class MarkerExtractor
{
public:

  class initialize_error : public std::logic_error
  {
  public:
    initialize_error(const std::string& what)
      :logic_error(what)
    {}
  };

  class detect_marker_error : public std::logic_error
  {
  public:
    detect_marker_error(const std::string& what)
      :logic_error(what)
    {}
  };

  /// Struct to handle the interest points of a found marker
  struct MarkerPoints
  {
    silver::Position primaryVertex;
    silver::Position secondaryVertex;
    silver::Identity<silver::Pose> center;
  };

  MarkerExtractor(int width, int height,
                  const std::vector<scene::Target>& targets);

  void initialize();

  void extract(const IplImage* const imgEntrada,
               std::vector<MarkerPoints>& vecMarkerPoints) const;

private:

  class MyLogger : public ARToolKitPlus::Logger
  {
  public:

    void artLog(const char* const nStr)
    {
      this->message = nStr;
    }

    std::string message;
  };

  static const int MAX_TARGETS = 40;

  MyLogger logger;

  const boost::scoped_ptr<ARToolKitPlus::TrackerSingleMarker> tracker;

  std::vector<std::string> vecDescriptionFilePath;

  /// Translates the inner artkp id to silver id
  boost::array<int, MAX_TARGETS> idMap;

};

#endif
