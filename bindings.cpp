//
// Created by tomas on 11/30/18.
//

#include <boost/python.hpp>

using namespace boost::python;

void main_rx();

BOOST_PYTHON_MODULE(cmpxchg)
{
    def("main_rx", main_rx);
}