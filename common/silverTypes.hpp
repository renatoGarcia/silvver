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

  struct Pose : public Position
  {
    double yaw;
    double pitch;
    double roll;

    Pose()
      :Position()
      ,yaw(0.0)
      ,pitch(0.0)
      ,roll(0.0)
    {}

    Pose(const Position& position,
         const double yaw=0.0, const double pitch=0.0, const double roll=0.0)
      :Position(position)
      ,yaw(yaw)
      ,pitch(pitch)
      ,roll(roll)
    {}
  };

  template<class T>
  struct Identity : public T
  {
    unsigned uid;

    Identity()
      :T()
      ,uid(0)
    {}

    Identity(const T& t, const unsigned uid)
      :T(t)
      ,uid(uid)
    {}
  };

} // namespace silver

#endif
