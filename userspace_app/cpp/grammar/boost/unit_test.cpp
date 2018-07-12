// https://github.com/jsankey/boost.test-examples
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MAIN
#include <boost/test/unit_test.hpp>
#include <exception>
#include <unistd.h>

// g++ unit_test.cpp -std=c++11 -Iopt/boost_1_57_0/include/ -L opt/boost_1_57_0/lib/ -lboost_system -lboost_wave -lboost_iostreams -lboost_unit_test_framework
// LD_LIBRARY_PATH="opt/boost_1_57_0/lib/" ./a.out

int add(int i, int j)
{
    return i + j;
}

BOOST_AUTO_TEST_SUITE(VariantsSuite)

BOOST_AUTO_TEST_CASE(simplePass)
{
}

BOOST_AUTO_TEST_CASE(checkFailure)
{
    BOOST_CHECK(add(2, 2) == 5);
}

BOOST_AUTO_TEST_CASE(multipleCheckFailures)
{
    BOOST_CHECK(add(2, 2) == 1);
    BOOST_CHECK(add(2, 2) == 2);
    BOOST_CHECK(add(2, 2) == 3);
}

BOOST_AUTO_TEST_CASE(requireFailure)
{
    BOOST_REQUIRE(add(2, 2) == 5);
}

BOOST_AUTO_TEST_CASE(explicitError)
{
    BOOST_ERROR("Error message");
}

BOOST_AUTO_TEST_CASE(explicitFailure)
{
    BOOST_FAIL("Failure message");
}

BOOST_AUTO_TEST_CASE(errorThenFailure)
{
    BOOST_FAIL("Error message");
    BOOST_FAIL("Failure message");
}

BOOST_AUTO_TEST_CASE(uncaughtException)
{
    throw "Catch me if you can!";
}

BOOST_AUTO_TEST_CASE(stdException)
{
    throw new std::exception();
}

BOOST_AUTO_TEST_CASE(checkMessageFailure)
{
    BOOST_CHECK_MESSAGE(add(2, 2) == 5, "add(..) result: " << add(2, 2));
}

BOOST_AUTO_TEST_CASE(checkEqualFailure)
{
    BOOST_CHECK_EQUAL(add( 2,2 ), 5);
}

BOOST_AUTO_TEST_CASE(threeSeconds)
{
    sleep(3);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(PassingSuite)

BOOST_AUTO_TEST_CASE(pass1)
{
}

BOOST_AUTO_TEST_CASE(pass2)
{
}

BOOST_AUTO_TEST_CASE(pass3)
{
}

BOOST_AUTO_TEST_SUITE_END()
