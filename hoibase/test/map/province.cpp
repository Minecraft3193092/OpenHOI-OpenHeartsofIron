// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include <gtest/gtest.h>

#include <hoibase/map/province.hpp>

namespace openhoi {

// Test the Province constructor
TEST(Hoibase, MapProvinceCtor) {
  auto vec = std::vector<Ogre::Vector2>();
  vec.push_back(Ogre::Vector2(-55.5f, 79.5f));
  vec.push_back(Ogre::Vector2(45.1f, 80.77f));
  vec.push_back(Ogre::Vector2(45.1f, -80.77f));

  auto coords = std::vector<std::vector<Ogre::Vector2>>();
  coords.push_back(vec);

  auto center = Ogre::Vector2(-5.0f, 20.0f);

  auto province = new Province("some_province", coords, center);

  EXPECT_EQ(province->getID(), "some_province");
  EXPECT_FLOAT_EQ(province->getCenter().x, -5.0f);
  EXPECT_FLOAT_EQ(province->getCenter().y, 20.0f);
  EXPECT_EQ(province->getCoordinates(), coords);
}

// The the province triangulation
TEST(Hoibase, MapProviceTriangulate) {
  auto vec1 = std::vector<Ogre::Vector2>();
  vec1.push_back(Ogre::Vector2(-13.18359375f, 62.186013857194226f));
  vec1.push_back(Ogre::Vector2(-22.587890625f, 50.45750402042058f));
  vec1.push_back(Ogre::Vector2(14.677734375000002f, 34.95799531086792f));
  vec1.push_back(Ogre::Vector2(28.388671875f, 52.3755991766591f));
  vec1.push_back(Ogre::Vector2(9.052734375f, 65.58572002329473f));

  auto vec2 = std::vector<Ogre::Vector2>();
  vec2.push_back(Ogre::Vector2(26.630859375f, 41.902277040963696f));
  vec2.push_back(Ogre::Vector2(35.419921875f, 37.85750715625203f));
  vec2.push_back(Ogre::Vector2(31.289062500000004f, 46.01222384063236f));

  auto coords = std::vector<std::vector<Ogre::Vector2>>();
  coords.push_back(vec1);
  coords.push_back(vec2);

  auto center = Ogre::Vector2(6.416015625f, 51.998410382390325f);

  auto province = new Province("some_province", coords, center);

  auto triangles = province->getTriangulatedVertices();

  EXPECT_EQ(triangles.size(), 54);
  EXPECT_FLOAT_EQ(triangles.at(0), 0.0815429688f);
  EXPECT_FLOAT_EQ(triangles.at(1), 0.411270559f);
  EXPECT_FLOAT_EQ(triangles.at(3), 0.119628906f);
  EXPECT_FLOAT_EQ(triangles.at(4), 0.513727009f);
  EXPECT_FLOAT_EQ(triangles.at(6), -0.0219726563f);
  EXPECT_FLOAT_EQ(triangles.at(7), 0.502444148f);
  EXPECT_FLOAT_EQ(triangles.at(9), 0.157714844f);
  EXPECT_FLOAT_EQ(triangles.at(10), 0.616183519f);
  EXPECT_FLOAT_EQ(triangles.at(12), 0.0502929688f);
  EXPECT_FLOAT_EQ(triangles.at(13), 0.771596670f);
  EXPECT_FLOAT_EQ(triangles.at(15), 0.119628906f);
  EXPECT_FLOAT_EQ(triangles.at(16), 0.513727009f);
  EXPECT_FLOAT_EQ(triangles.at(18), 0.119628906f);
  EXPECT_FLOAT_EQ(triangles.at(19), 0.513727009f);
  EXPECT_FLOAT_EQ(triangles.at(21), 0.0502929688f);
  EXPECT_FLOAT_EQ(triangles.at(22), 0.771596670f);
  EXPECT_FLOAT_EQ(triangles.at(24), -0.0219726563f);
  EXPECT_FLOAT_EQ(triangles.at(25), 0.502444148f);
  EXPECT_FLOAT_EQ(triangles.at(27), 0.196777344f);
  EXPECT_FLOAT_EQ(triangles.at(28), 0.445382416f);
  EXPECT_FLOAT_EQ(triangles.at(30), 0.173828125f);
  EXPECT_FLOAT_EQ(triangles.at(31), 0.541320264f);
  EXPECT_FLOAT_EQ(triangles.at(33), 0.147949219f);
  EXPECT_FLOAT_EQ(triangles.at(34), 0.492967993f);
  EXPECT_FLOAT_EQ(triangles.at(36), 0.0502929688f);
  EXPECT_FLOAT_EQ(triangles.at(37), 0.771596670f);
  EXPECT_FLOAT_EQ(triangles.at(39), -0.0732421875f);
  EXPECT_FLOAT_EQ(triangles.at(40), 0.731600165f);
  EXPECT_FLOAT_EQ(triangles.at(42), -0.0219726563f);
  EXPECT_FLOAT_EQ(triangles.at(43), 0.502444148f);
  EXPECT_FLOAT_EQ(triangles.at(45), -0.0732421875f);
  EXPECT_FLOAT_EQ(triangles.at(46), 0.731600165f);
  EXPECT_FLOAT_EQ(triangles.at(48), -0.125488281f);
  EXPECT_FLOAT_EQ(triangles.at(49), 0.593617678f);
  EXPECT_FLOAT_EQ(triangles.at(51), -0.0219726563f);
  EXPECT_FLOAT_EQ(triangles.at(52), 0.502444148f);
}


}  // namespace openhoi