/*
History
=======


Notes
=======

*/
#ifndef __TASK_H
#define __TASK_H
#include "schedule.h"
namespace Pol {
  namespace Core {
	void update_rpm();
	void regen_stats();
	void regen_resources();
	void update_sysload();
  }
}
#endif
