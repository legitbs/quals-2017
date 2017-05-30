#include "common.h"

#include <linux/perf_event.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) 
{
	int ret;

	ret = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);

	return ret;
}

LinuxPerformanceCounter::LinuxPerformanceCounter( )
{
	memset(&m_pe, 0, sizeof(struct perf_event_attr));
	m_pe.type = PERF_TYPE_SOFTWARE; // PERF_TYPE_HARDWARE;
	m_pe.size = sizeof(struct perf_event_attr);
	m_pe.config = PERF_COUNT_SW_TASK_CLOCK; //PERF_COUNT_HW_BRANCH_INSTRUCTIONS; // PERF_COUNT_SW_CPU_CLOCK; // PERF_COUNT_HW_INSTRUCTIONS;
	m_pe.disabled = 1;
	m_pe.exclude_kernel = 1;
	m_pe.exclude_hv = 1;
	m_pe.exclude_idle = 1;
	m_pe.enable_on_exec = 1;

	m_perfFD = perf_event_open(&m_pe, 0, -1, -1, 0);
	if ( m_perfFD == -1 ) 
		LogDebug( LOG_PRIORITY_HIGH, "Error opening performance counter %llx\n", m_pe.config);

}

LinuxPerformanceCounter::~LinuxPerformanceCounter( )
{
	if ( m_perfFD )
		close( m_perfFD );

	m_perfFD = -1;	
}

void LinuxPerformanceCounter::StartTimer( void )
{
	if ( m_perfFD )
	{
		LogDebug( LOG_PRIORITY_LOW, "Performance event StartTimer()\n" );
		ioctl( m_perfFD, PERF_EVENT_IOC_RESET, 0 );
		ioctl( m_perfFD, PERF_EVENT_IOC_ENABLE, 0 );
	}
}

void LinuxPerformanceCounter::EndTimer( void )
{
	if ( m_perfFD )
	{
		LogDebug( LOG_PRIORITY_LOW, "Performance event EndTimer()\n" );
		ioctl( m_perfFD, PERF_EVENT_IOC_DISABLE, 0 );
	}
}

uint64_t LinuxPerformanceCounter::GetTime( void )
{
	uint64_t count;
	if ( m_perfFD )
	{
		read( m_perfFD, &count, sizeof(count) );
		LogDebug( LOG_PRIORITY_LOW, "Performance event GetTime() %lld\n", count );
		return (count);
	}
	else
		return (0);
}
