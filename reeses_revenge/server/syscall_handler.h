#ifndef __SYSCALL_HANDLER_H__
#define __SYSCALL_HANDLER_H__

class CSysCallHandler
{
public:
	void DoSyscall( CCPU *pCPU );

private:
	void DoNothing( CCPU *pCPU );
	void DoExit( CCPU *pCPU );
	void DoRead( CCPU *pCPU );
	void DoWrite( CCPU *pCPU );
	void DoTime( CCPU *pCPU );
	void DoRand( CCPU *pCPU );
	void DoMMAP( CCPU *pCPU );
};

void seed_prng( uint32_t value );
uint32_t prng( void );

#endif // __SYSCALL_HANDLER_H__
