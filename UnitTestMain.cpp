#include <iostream>
#include <string>
#include <list>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <netinet/in.h>

#include "TableF.h"
#include "Helpers.h"

using namespace CppUnit;
using namespace std;

//-----------------------------------------------------------------------------

class Suite : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(Suite);
    CPPUNIT_TEST(testCheckTableF);
    CPPUNIT_TEST(testCheckHammingDist);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testCheckTableF(void);
    void testCheckHammingDist(void);

private:

    TableF *TableOfValuesF;
    
};

//-----------------------------------------------------------------------------

void Suite::testCheckTableF(void)
{
    int position_zero = TableOfValuesF->checkItem(2);
    int position_one = TableOfValuesF->checkItem(14);
    int position_minus = TableOfValuesF->checkItem(33);

    // simple asserts
    // CPPUNIT_ASSERT(position == 18);
    // CPPUNIT_ASSERT_MESSAGE("Must be 18", position == 18);

    // // asserting equality:
    // CPPUNIT_ASSERT_EQUAL(position, 18);
    
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning bucket #0", position_zero, 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning bucket #1", position_one, 1);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning -1 for non-existing item", position_minus, -1);
    

    // exception asserts:
    //CPPUNIT_ASSERT_THROW(ClassImTesting - > testAge(age - 1), WrongAgeException);
    //CPPUNIT_ASSERT_NO_THROW(ClassImTesting - > testAge(age), WrongAgeException);

    // inverse asserts:
    // if (position != 18)
    //   CPPUNIT_FAIL("Must be 18");
    // CPPUNIT_ASSERT_ASSERTION_FAIL(CPP_UNIT_ASSERT(position != 18));

    // CPPUNIT_ASSERT(5 == TableOfValuesF->Addition(2,3));testCheckHyperCube
}

void Suite::testCheckHammingDist(void)
{

    int dist1 = hammingDistance("000","000");
    int dist2 = hammingDistance("000","001");
    int dist3 = hammingDistance("000","011");

    // simple asserts
    // CPPUNIT_ASSERT(position == 18);
    // CPPUNIT_ASSERT_MESSAGE("Must be 18", position == 18);

    // // asserting equality:
    // CPPUNIT_ASSERT_EQUAL(position, 18);
    
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning hamming dist 0", dist1, 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning hamming dist 1", dist2, 1);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning hamming dist 2", dist3, 2);


}

void Suite::setUp(void)
{
    TableOfValuesF = new TableF();
    
    for (int i =0; i < 10; i++){
        TableOfValuesF->insertItem(i, 0);
    }

    for (int i =11; i < 20; i++){
        TableOfValuesF->insertItem(i, 1);
    }

    
}

void Suite::tearDown(void)
{
    delete TableOfValuesF;

}

//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION( Suite );

int main(int argc, char* argv[])
{
    // informs test-listener about testresults
    CPPUNIT_NS::TestResult testresult;

    // register listener for collecting the test-results
    CPPUNIT_NS::TestResultCollector collectedresults;
    testresult.addListener (&collectedresults);

    // register listener for per-test progress output
    CPPUNIT_NS::BriefTestProgressListener progress;
    testresult.addListener (&progress);

    // insert test-suite at test-runner by registry
    CPPUNIT_NS::TestRunner testrunner;
    testrunner.addTest (CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    testrunner.run(testresult);

    // output results in compiler-format
    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write ();

    // Output XML for Jenkins CPPunit plugin
    ofstream xmlFileOut("cppTableFTest.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    // return 0 if tests were successful
    return collectedresults.wasSuccessful() ? 0 : 1;
}

