#include "WCPNav/Depositor.h"

WCP::Depositor::~Depositor()
{
}
 
const std::vector<int>& WCP::Depositor::timeoffset() const
{
  return timeo;
}
