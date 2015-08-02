#include "MyDepo.h"

#include <iostream>
using namespace std;


std::ostream& operator<<(std::ostream& out, const WireCell::IDepo& depo)
{
    out << "<IDepo t:" << depo.time() << " q:" << depo.charge() << " pos:" << depo.pos() << ">";
    return out;
}



int main()
{
    MyDepoSrc ds;
    MyDepoFilter df(ds);

    WireCell::IDepo::const_ptr p;
    while ((p = df.depo_gen())) {
	cout << *p << endl;
    }

}
