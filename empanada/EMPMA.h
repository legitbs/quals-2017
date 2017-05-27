#ifndef EMPMA_H_
#define EMPMA_H_

#define MMAP_ADDR 0x31337000
#define MMAP_SIZE 0x100000
#define BLOCK_SIZE 0x1000

using namespace std;

typedef struct EMPMA_PAGE {
	bool bUsed;
	int size;
	void *pNext;
} EMPMAPage;

class EMPMA {
	public:
		EMPMA();
		~EMPMA();
		void *alloc(int size);
		void free(void*);
	protected:
		vector<void*> pageTable;
		vector<void*>::iterator pti;
	private:
		bool bEnabled;
		void *pStart;
};	

EMPMA::EMPMA(void) {
	pStart = mmap((void*)MMAP_ADDR, MMAP_SIZE, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	if (pStart == MAP_FAILED || pStart == NULL) {
			if (DEBUG)
    			cout << "ERROR mmap failed\n";
	    	bEnabled = false;
			pStart = NULL;
	    	return;
    	}

    bEnabled = true;

    if (DEBUG)
    	cout << "pStart: " << pStart << "\n";

    pageTable.push_back(pStart);

}

EMPMA::~EMPMA() {

}

void* EMPMA::alloc(int size) {
	void *pHead;

	if ((uint32_t)pStart + size >= (uint32_t)pStart + MMAP_SIZE) {
		return NULL;
	}
	
	for(pti = pageTable.begin(); pti != pageTable.end(); pti++) {
		if (*pti != NULL) { 
			EMPMAPage *p = (EMPMAPage*)*pti;

			if (p == NULL) {
				return NULL;
			}
			
			if (!p->bUsed) {
				if (p->size == 0) {
					p->bUsed = true;
					p->size = size;
					p->pNext = p + 1 + (size/sizeof(EMPMAPage));
					pageTable.push_back(p->pNext);
					pHead = p+1;
					if (DEBUG) {
						cout << "[alloc] EMPMAPage size: " << p->size << "\n";
						cout << "[alloc] found uninitialized page at " << p << " with data starting at " << p+1 << "\n";
						cout << "[alloc] Used: " << p->bUsed << "\t Next: " << p->pNext << "\n"; 
					}
					break;
				} else if (p->size < size) {
					continue;
				} else {
					if (DEBUG)
						cout << "[alloc] found unused page at " << p << "\n";
					memset(p+1, 0, size);
					pHead = p+1;
					p->bUsed = true;
					break;
				}
			}
		}
	}

	if (DEBUG)
		cout << "[alloc] pHead: " << pHead << "\n";

	return pHead;
}

void EMPMA::free(void *ptr) {

	void *pHead = (char *)ptr-sizeof(EMPMAPage);

	if (DEBUG) {
		cout << "[free] looking for " << pHead << "\n";
	}

	if (find(pageTable.begin(), pageTable.end(), pHead) != pageTable.end())
	{
		EMPMAPage *p = (EMPMAPage*)pHead;

		if (DEBUG) {
			cout << "[free] Freeing memory at " << pHead << "\n";
			cout << "[free] size of " << p->size << "\n";
		}


		if (!p->bUsed) {
			if (DEBUG)
				cout << "[free] Memory is already freed\n";
			return;
		}

		p->bUsed = false;
	}
	else {
		if (DEBUG)
			cout << "[free] ERROR: " << pHead << " not allocated by EMPMA\n";
		return;
	}

}

#endif
