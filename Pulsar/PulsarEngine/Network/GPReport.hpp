#ifndef _GPREPORT_
#define _GPREPORT_
#include <types.hpp>

namespace Pulsar {
namespace Network {

void Report(const char* key, const char* string);
void ReportU32(const char* key, u32 uint);

}//namespace Network
}//namespace Pulsar
#endif
