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


class Suite : public CppUnit::TestFixture {
    
    CPPUNIT_TEST_SUITE(Suite);
    CPPUNIT_TEST(testCheckTableF);
    CPPUNIT_TEST(testCheckHammingDist);
    CPPUNIT_TEST(testCheckBinarySearch);
    CPPUNIT_TEST_SUITE_END();

private:

    TableF *TableOfValuesF;

protected:
    void testCheckTableF(void);
    void testCheckHammingDist(void);
    void testCheckBinarySearch(void);

public:
    void setUp(void);
    void tearDown(void);
    
};


void Suite::testCheckTableF(void){


    int position_zero = TableOfValuesF->checkItem(2);
    int position_one = TableOfValuesF->checkItem(14);
    int position_minus = TableOfValuesF->checkItem(33);

    
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning bucket #0", position_zero, 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning bucket #1", position_one, 1);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning -1 for non-existing item", position_minus, -1);
    
}

void Suite::testCheckHammingDist(void)
{

    int dist1 = hammingDistance("000","000");
    int dist2 = hammingDistance("000","001");
    int dist3 = hammingDistance("000","011");


    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning hamming dist 0", dist1, 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning hamming dist 1", dist2, 1);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning hamming dist 2", dist3, 2);

}

void Suite::testCheckBinarySearch(void)
{

    double arr_check[6];
    for (int i = 0; i < 6; i++){
        arr_check[i] = i; 
    }

    int idx1 = binarySearch(arr_check,-1,6); //0
    int idx2 = binarySearch(arr_check, 0,6); //0
    int idx3 = binarySearch(arr_check, 7,6); //5
    int idx4 = binarySearch(arr_check, 3,6); //3
    int idx5 = binarySearch(arr_check, 1.5,6); //2
    int idx6 = binarySearch(arr_check, 3.5,6); //4

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning index 0", idx1, 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning index 0", idx2, 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning index 5", idx3, 5);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning index 3", idx4, 3);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning index 2", idx5, 2);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Returning index 4", idx6, 4);

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
    //informs test-listener about testresults
    CPPUNIT_NS::TestResult test_result_obj;

    //register listener for collecting the test-results
    CPPUNIT_NS::TestResultCollector collected_results;
    test_result_obj.addListener(&collected_results);

    //insert test-suite at test-runner by registry
    CPPUNIT_NS::TestRunner runner;

    runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    runner.run(test_result_obj);

    //output results in compiler-format
    CPPUNIT_NS::CompilerOutputter cli_output(&collected_results, cerr);
    cli_output.write();

    //Output XML
    ofstream xml_out_obj("Project2UnitTests.xml");
    XmlOutputter xml_out(&collected_results, xml_out_obj);
    xml_out.write();

    //return 0 if tests were successful
    if (collected_results.wasSuccessful()){
        return 0;
    }else{
        return 1;
    }

    return -1;

}

