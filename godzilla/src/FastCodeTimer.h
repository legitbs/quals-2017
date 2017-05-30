/*
 ***************************************************************************************
 * Compatibility:
 *      * Tested on MSVC 6.0 and works.  Fairly accurately too.
 *      * Tested on GCC 2.9 for redhat 6.1 and works.
 *
 *
 ***************************************************************************************
 * Notes:
 *     Feel free to modify this class to use an average of the FlushSpeed to get a more
 * accurate flush speed.  I found that this is overkill as the flush speed should be the
 * quickest possible speed that is found.  IE when everything is aligned right in cache
 * etc the flush speed will be the lowest value we observe.
 *
 *
 * Revision 1.0.0.0
 * Date: 5/03/2003
 *
 ***************************************************************************************
 * Notes:
 *      Ported to work with Linux GCC compiler.  Inorder to use this library for the GNU
 * GCC compiler you must define _COMPILER_GCC.  If this is not defined it is assumed the
 * compiler is MSVC for windows.  I have not tested it for any other compilers.
 *
 * Revision 1.0.0.1
 * Date: 9/09/2003
 *
 ***************************************************************************************
 * Quick Reference:
 *     CCodeTimer oTimer;
 *     oTimer.StartTimer();
 *     <---- Put code you want measured here ---->
 *     oTimer.EndTimer();
 *
 *     oTimer.GetTime(); oTimer.GetTimeString(); oTimer.PrintTime();
 *
 ***************************************************************************************/

#ifndef __FAST_CODE_TIMER_H
#define __FAST_CODE_TIMER_H

#include <stdlib.h>
#include <linux/perf_event.h>

#if defined(__GNUC__)
extern __inline__ unsigned long long int ReadPerfReg()
   {
     unsigned long long int x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
     return x;
   }

#else // Assume MSVC for windows
#pragma warning( push )
#pragma warning( disable : 4035 4244) 
    // This works because MSVC 6.0 returns int64 as the edx:eax which is what RDTSC sets.  How cool right?
	inline uint64_t ReadPerfReg( void ) {
		__asm 
		{
			RDTSC
		}
	}
#pragma warning( pop )
#endif

class SimplePerformanceCounter
{
public:
	SimplePerformanceCounter( )
	{
		m_performanceCounter = 0;
	}

	~SimplePerformanceCounter( )
	{
		m_performanceCounter = 0;
	}
	
	void StartTimer( void )
	{
		m_performanceCounter = 0;
	}

	void EndTimer( void )
	{
		
	}
	
	uint64_t GetTime( void )
	{
		return (m_performanceCounter);
	}

	void AddTime( uint64_t amount )
	{
		m_performanceCounter += amount;
	}

private:
	uint64_t m_performanceCounter;
};

class LinuxPerformanceCounter
{
public:
	LinuxPerformanceCounter( );
	~LinuxPerformanceCounter( );

	void StartTimer( void );
	void EndTimer( void );

	uint64_t GetTime( void );

private:
	struct perf_event_attr m_pe;
	int m_perfFD;
};

class FastCodeTimer
{
private:
	uint64_t m_startTime;
	uint64_t m_endTime;

	uint64_t m_flushSpeed;

	uint64_t GetTimeDifference( void )
	{
		//return ((m_endTime - m_startTime) - m_flushSpeed);
		return (m_endTime - m_startTime);
	}

public:
	FastCodeTimer()
	{
		uint64_t flushSpeedLow = 0;

		m_flushSpeed = 0;
		m_startTime = 0;
		m_endTime = 0;
#if 0
		// Determine the Flush Speed (The time it takes to call ourselves to get a timing measurement).
		for ( int i = 0; i < 20; i++ )
		{
			m_flushSpeed = 0;

#if defined(__GNUC__)
			// Call the CPUID function to flush the cache lines.  We must declare eax,ebx,ecx,and edx clobbered.
			// Notice: We declare this volatile so GCC won't optimize it out of the loop etc.
			__asm__ __volatile__ ("cpuid; cpuid; cpuid" : : : "%eax", "%ebx", "%ecx", "%edx" );
#else
			__asm {
				cpuid
				cpuid
				cpuid
			}
#endif
			StartTimer();
			EndTimer();

#if defined(__GNUC__)
			__asm__ __volatile__ ("cpuid" : : : "%eax", "%ebx", "%ecx", "%edx" );
#else
			__asm {
				cpuid
			}
#endif
			// We are interested in the lowest possible flush speed we can get since this is most
			// likely the actual speed it will take.  In all ideal conditions of course.
			if ( i == 0 )
				flushSpeedLow = GetTimeDifference();
			else if ( GetTimeDifference() < flushSpeedLow )
				flushSpeedLow = GetTimeDifference();

		}

		m_flushSpeed = flushSpeedLow;
#endif

		m_startTime = 0;
		m_endTime = 0;
	}

	void SetTimer( FastCodeTimer &oTimer )
	{
		m_startTime = oTimer.GetStartTime();
		m_endTime = oTimer.GetEndTime();
	}


	uint64_t GetStartTime( void ) const
	{
		return (m_startTime);
	}

	uint64_t GetEndTime( void ) const
	{
		return (m_endTime);
	}

	uint64_t GetFlushSpeed( void ) const
	{
		return (m_flushSpeed);
	}

	// Called to start the timer.
	void StartTimer( void )
	{
		m_startTime = ReadPerfReg();
	}	
	
	// Called to end the timer.
	void EndTimer( void )
	{
		m_endTime = ReadPerfReg();
	}

	// Returns the amount of time execution took.
	uint64_t GetTime( void )
	{
		return (GetTimeDifference());
	}	


	// Returns a string for time difference
	std::string GetTimeString( void )
	{
		char pszString[100];

#if defined(__GNUC__)
		sprintf( pszString, "%llu", GetTimeDifference() );
#else
		sprintf( pszString, "%I64d", GetTimeDifference() );
#endif 

		return pszString;
	}

};

#endif // __FAST_CODE_TIMER_H
