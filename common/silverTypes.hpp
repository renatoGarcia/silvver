/**
 * @file   silverTypes.hpp
 *
 * @brief Especificação dos tipos e constantes usados para a comunicação entre os módulos de silvver.
 *
 */
#ifndef SILVER_TYPES_HPP
#define SILVER_TYPES_HPP

namespace silver
{
  //------------------------------ Position
  struct Position
  {
    double x;
    double y;
    double z;

    Position(const double x=0.0, const double y=0.0, const double z=0.0)
      :x(x)
      ,y(y)
      ,z(z)
    {}

    Position(const Position& position)
      :x(position.x)
      ,y(position.y)
      ,z(position.z)
    {}
  };

  template <class charT, class traits>
  inline
  std::basic_ostream<charT,traits>&
  operator << (std::basic_ostream<charT,traits>& strm,
               const Position& position)
  {
    strm << position.x << '\t' << position.y << '\t' << position.z;
    return strm;
  }

  //------------------------------ Pose
  struct Pose : public Position
  {
    // double yaw;
    // double pitch;
    // double roll;

    double rotationMatrix[3][3];

    Pose()
      :Position()
      // ,yaw(0.0)
      // ,pitch(0.0)
      // ,roll(0.0)
    {}

    Pose(const Position& position,
         const double yaw=0.0, const double pitch=0.0, const double roll=0.0)
      :Position(position)
      // ,yaw(yaw)
      // ,pitch(pitch)
      // ,roll(roll)
    {}
  };

  template <class charT, class traits>
  inline
  std::basic_ostream<charT,traits>&
  operator << (std::basic_ostream<charT,traits>& strm,
               const Pose& pose)
  {
    const Position& position = static_cast<const Position&>(pose);
    strm  << position;
    for(int i = 0; i < 3; ++i)
      for(int j = 0; j < 3; ++j)
        strm  << '\t' << pose.rotationMatrix[i][j];

    return strm;
  }

  //------------------------------ Identity
  template<class BaseClass>
  struct Identity : public BaseClass
  {
    unsigned uid;

    Identity()
      :BaseClass()
      ,uid(0)
    {}

    Identity(const BaseClass& base, const unsigned uid)
      :BaseClass(base)
      ,uid(uid)
    {}
  };

  template <class charT, class traits, class BaseClass>
  inline
  std::basic_ostream<charT,traits>&
  operator << (std::basic_ostream<charT,traits>& strm,
               const Identity<BaseClass>& identity)
  {
    const BaseClass& base = static_cast<const BaseClass&>(identity);
    strm << identity.uid << '\t' << base;
    return strm;
  }

} // namespace silver

#endif
