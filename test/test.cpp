#define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>

#include "microgeo.h"

BOOST_AUTO_TEST_SUITE(test_suite_microgeo)

BOOST_AUTO_TEST_CASE(microgeo_1)
{
    BOOST_CHECK_EQUAL(DistanceSegmentPoint(0,0,1,2,3,4),sqrt(5));
    BOOST_CHECK_EQUAL(DistanceSegmentPoint(0,1,-1,0,1,0),1);
}

BOOST_AUTO_TEST_CASE(microgeo_2)
{
    BOOST_CHECK_EQUAL(ProduitScalaire(1,0,1,1),1);
}

BOOST_AUTO_TEST_CASE(microgeo_3)
{
    double x1,y1;
    InterieurVirage(0,0,2,1,4,0,2,x1,y1);
    BOOST_CHECK_EQUAL(x1,2);
    BOOST_CHECK_EQUAL(y1,-1);

    ExterieurVirage(-1,1,0,0,1,1,1,x1,y1);
    BOOST_CHECK_EQUAL(x1,0);
    BOOST_CHECK_EQUAL(y1,-1);
}

BOOST_AUTO_TEST_CASE(microgeo_4)
{
    // Cas qui ma fait taper l'imprimante!
    // Si le premier point est le même que le troisième
    // UM2_XGearCover nLayer 86
    double xp,yp;
    ExterieurVirage(99.466,109.75,100.465,110.476,99.466,109.75,0.17,xp,yp);
    BOOST_CHECK(xp >= 0);
    BOOST_CHECK(xp < 200);
    BOOST_CHECK(yp >= 0);
    BOOST_CHECK(yp < 200);
}

/*
BOOST_AUTO_TEST_CASE(test_segment)
{
    BOOST_CHECK_EQUAL(1,1);
}
*/

BOOST_AUTO_TEST_SUITE_END()

