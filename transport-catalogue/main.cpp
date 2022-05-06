#include <iostream>
#include <fstream>

#include "input_reader.h"
#include "transport_catalogue.h"

using namespace std;

int main(int argc, char** argv)
{
    transport_catalogue::TransportCatalogue catalogue;

    if (1 < argc) {
        ifstream in(argv[1]);
        ofstream out(argv[2]);
        transport_catalogue::parsing::ReadData(catalogue, in);
        transport_catalogue::parsing::ReadAllRequestsAndPrint(catalogue, in, out);
    } else {
        transport_catalogue::parsing::ReadData(catalogue, cin);
        transport_catalogue::parsing::ReadAllRequestsAndPrint(catalogue, cin, cout);
    }
}
