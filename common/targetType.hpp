#ifndef TARGET_TYPE_HPP
#define TARGET_TYPE_HPP

#include <stdexcept>
#include <string>

struct TargetType
{
  enum TargetTypeEnum{
    COLOR_BLOB,
    ARTP_MARK,

    UNDEFINED
  }state;

  TargetType()
    :state(UNDEFINED)
  {}

  TargetType(const std::string& targetType)
  {
    if(targetType == "artp_mark")
    {
      this->state = ARTP_MARK;
    }
    else if(targetType == "color_blob")
    {
      this->state = COLOR_BLOB;
    }
    else
    {
      this->state = UNDEFINED;
      throw std::invalid_argument("Unknown TargetType: " + targetType);
    }
  }

  TargetType(const TargetType& targetType)
  {
    this->state = targetType.state;
  }

  bool operator==(const TargetTypeEnum targetType)
  {
    return (targetType == this->state);
  }
};

#endif // TARGET_TYPE_HPP
