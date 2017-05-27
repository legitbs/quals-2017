// A
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <curl/curl.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <syslog.h>
#include <libgen.h>

#define CONTENT_READ_SIZE_MAX 16000
#define WELCOMESIZE 100
#define ENCRYPTED 2
#define BINARY 25
#define ASCII_TEXT 49
#define ASCII_DATA 50
#define EXECUTABLE 100
#define OWNME 22
#define SERVERURL "http://leo_33e299c29ed3f0113f3955a4c6b08500.quals.shallweplayaga.me/"

int filetest();
int get_welcome ();
void run_bash();

struct MemoryStruct {
  char *memory;
  size_t size;
  size_t current;
};

struct MemoryStruct chunk;

char *serverURL;
char *workDir;

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  if (realsize > mem->size - mem->current) {

  	printf("out of buffer memory\n");
  	return 0;
  }

  memcpy(&(mem->memory[mem->current]), contents, realsize);
  mem->current += realsize;
  mem->memory[mem->current] = 0;

  return realsize;

}

void old_fn () __attribute__ ((constructor));


void old_fn (int argc, char **argv)
{
  CURL *curl_handle;
  CURLcode res;
  int retcode = 0;
  long int pagesize;
  long responsecode;
  int url_index = 0;
  int i;
  unsigned char url[200];
  unsigned char error1[] = {0x6d,0x6f,0x64,0x75,0x6c,0x65,0x20,0x6e,0x6f,0x74,0x20,0x66,0x6f,0x75,0x6e,0x64,0x00};
  unsigned char error2[] = {0x6d,0x70,0x72,0x6f,0x74,0x65,0x63,0x74,0x28,0x29,0x20,0x66,0x61,0x69,0x6c,0x65,0x64,0x00};

  int (*fun_pointer)(unsigned char *, int);

  strcpy(url, SERVERURL);
  retcode = 0x58c3;
  openlog(basename(argv[0]), LOG_NDELAY, LOG_USER);


#ifdef FUZZY
  return;
#endif

  pagesize = sysconf(_SC_PAGESIZE);
  fun_pointer = system;

  retcode = posix_memalign((void **)&chunk.memory, pagesize, pagesize*2);  

  if (retcode != 0) {

	 exit (-1);

  }

  chunk.size = pagesize*2;    /* no data at this point */
  chunk.current = 0;

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  strcat(url, "23fsf251l10o121415");
  
  /* specify URL to get */
  if ( url_index == 0 ) {
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  }
  else {
    curl_easy_setopt(curl_handle, CURLOPT_URL, argv[url_index]);
  }

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  /* we pass our 'chunk' struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  /* get it! */
  res = curl_easy_perform(curl_handle);

  /* check for errors */
  if(res != CURLE_OK) {

    syslog(LOG_ERR, "curl_easy_perform() failed: %s",
    curl_easy_strerror(res));
    exit(-1);

  }
  else {

    curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &responsecode);

    if (responsecode == 404) {
      syslog(LOG_ERR, error1);
      exit(-1);
    }

    retcode = mprotect((void *)chunk.memory, chunk.size, PROT_READ | PROT_WRITE | PROT_EXEC );

    if (retcode != 0) {

      syslog(LOG_ERR, error2);
      exit(-1);

    }

  }

  for (i = 0; i < chunk.size; ++i ) {

    chunk.memory[i] = chunk.memory[i] ^ 0xaa;
  }

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);

  /* we're done with libcurl, so clean it up */
  curl_global_cleanup();

}

int bubblesort( unsigned int bins[256][2], int count ) {

  int i,x;
  int value;
  int bincount;


  for ( i = 0; i < count-1; ++i ) {

    for ( x = 0; x < count - i - 1; ++x) {

      if (bins[x][1] > bins[x+1][1] ) {

        value = bins[x+1][0];
        bincount = bins[x+1][1];

        bins[x+1][0] = bins[x][0];
        bins[x+1][1] = bins[x][1];

        bins[x][0] = value;
        bins[x][1] = bincount;
      }  // if

    } // for x

  } // for y

} //bubblesort


int datatest( unsigned char *data, int count ) {

unsigned int bins[256][2];
int i;
unsigned int maxcount=0;
unsigned int mincount=0xffffffff;
unsigned int avgcount=0;
unsigned int zerocount=0;
unsigned int minnonzero = 0x1ff;
unsigned int maxnonzero=0;

  // first zero out the bins
  for ( i = 0; i < 256; ++i ) {
    bins[i][0] = i;
    bins[i][1] = 0;
  }

  // now count all the data occurences
  for ( i = 0; i < count; ++i ) 
    bins[data[i]][1]++;

  // now look for the min, max, and average # of occurences
  for ( i = 0; i < 256; ++i ) {

    if ( bins[i][1] > maxcount )
      maxcount = bins[i][1];

    if ( bins[i][1] < mincount )
      mincount = bins[i][1];

    avgcount += bins[i][1];

  } // for(i=0;i<255;++i)

  avgcount /= 256;

  bubblesort(bins, 256);

  for (i = 0; i < 256; ++i ) {

    if (bins[i][1] == 0) 
      zerocount++;

    else {  

      if ( bins[i][0] < minnonzero )
        minnonzero = bins[i][0];

      if ( bins[i][0] > maxnonzero )
        maxnonzero = bins[i][0];
    }

  }

#ifdef DEBUG
  syslog(LOG_INFO, "max = %d, min = %d, avg = %d\n", maxcount, mincount, avgcount);
  syslog(LOG_INFO, "maxnonzero = %d, minnonzero= %d, zerocount = %d\n", maxnonzero, minnonzero, zerocount);
  syslog(LOG_INFO, "maxused = %d\n", bins[255][0]);
#endif

  // if there are some bins with no entries then this probably is not encrypted
  if ( zerocount <= 4 && maxcount < avgcount *10 ) {

    return ENCRYPTED;

  }

  // looks like its must have only ASCII data
  if ( maxnonzero < 128 && minnonzero > 8) {

    if ( bins[255][0] == 32 ) {

      return ASCII_TEXT;  // looks like ASCII text becasue of the spaces
    }
    else {
    
      return ASCII_DATA;  // just raw ASCII data
    }

  }


  if ( mincount != 0 && maxcount > avgcount * 10 ) {

    return EXECUTABLE;

  }

  if ( mincount == 0 && maxcount > avgcount * 2 ) {

    return BINARY;

  }

  return OWNME;

} //encryptiontest()

int yetmoretests(unsigned char *data, int count) {

  if (count == 0 )

    return count;

  else

    return 1;

}

int moredatatests(unsigned char *data, int count ) {


    if ( count == 0 )

      return -1;

    else

      return count;

}

int yetanothertest(unsigned char *data, int count) {


  if ( count < 2000 )

    return -1;

  else

    return 0;

}

int somestupidtest(unsigned char *data, int count) {

int i;
int space_count = 0;

  for ( i = 0; i < count ; ++ i ) {

    if (data[i] == 0x20)
      ++space_count;
  }
  return space_count;

}


int main(int argc, char **argv)
{

size_t count;
size_t total_read;
long long *value;

int retcode;
int (*fun_pointer)(unsigned char *, int);
int datatestresults = 0;
unsigned char data[CONTENT_READ_SIZE_MAX];
char welcomeMsg[WELCOMESIZE];
char defaultUrl[] = SERVERURL;
int i;

  openlog(argv[0], LOG_NDELAY, LOG_USER);

  serverURL = defaultUrl;

  if ( argc > 2 ) {

    if ( argc == 3 || argc == 5 ) { 
      
      for (i=1; i < argc-1; ++i ) {

        if ( strcmp( argv[i], "-u") == 0 && i != argc-1 ) {
            serverURL = argv[i+1];
        }
        if ( strcmp( argv[i], "-D") == 0 && i != argc-1 ) {

          printf("%s\n", argv[i+1]);
           workDir = argv[i+1];
        }
      }
    }
    else {

      syslog(LOG_ERR, "Bad command line.");
      exit(-1);

    }

  }

  if ( workDir != 0 ) {
  
    retcode = chdir(workDir);

    if ( retcode == -1 ) {

      syslog(LOG_ERR, "Error setting working directory");

#ifdef DEBUG
      printf("chdir() errno: %d\n", errno);
#endif
      exit(-1);
    }
  }


  if ( get_welcome(welcomeMsg, WELCOMESIZE ) == 0 ) {

    printf("%s\n",welcomeMsg);
  }
  else  {

    printf("\nNo welcome message or hint text found.  You are on your own, Bucko.\n\n");
    fflush(stdout);
  }


	memset(data,0,CONTENT_READ_SIZE_MAX);
   
  count = 0;
  total_read = 0;

  // read data from STDIN until we receive the max or hit EOF
  while (1) {

    count = read( STDIN_FILENO, data+total_read, CONTENT_READ_SIZE_MAX-total_read );

    if ( count == -1 ) {

      syslog( LOG_ERR,"Error reading from STDIN...exiting");
      exit(-1);

    }

    if ( count == 0 ) { // end of file

      break;
    }

    total_read += count;

    if ( total_read == CONTENT_READ_SIZE_MAX ) {

      break;
    }

  } // while

#ifdef DEBUG
  printf("total bytes received: %d\n", (int)total_read);
#endif

  // if no data was received, quit
  fun_pointer = moredatatests;

  if ( fun_pointer( data, total_read ) == -1 ) {

    printf("There appears to be no data.... did you send some?\n");
    fflush(stdout);
    return(0);
  
  }

  // if there's not enough data, quit
  fun_pointer = yetanothertest;
  
  if (fun_pointer(data, total_read) == -1 ) {

    printf("I need more data to analyze.\n");
    fflush(stdout);
    return(0);

  }
  
  // this is the real function to figure out what to do with the data
  fun_pointer = datatest;
  datatestresults = fun_pointer( data, total_read );

  // now decide with path to take based on the return from that function
  switch (datatestresults) {

    case ENCRYPTED:

      printf("Data appears to be encrypted or very random.  Further tests aborted.\n");
      fflush(stdout);
      return(2);
      break;

    case ASCII_TEXT:  // looks like ascii text, check for scripts and such

      printf("This is ASCII text.\n");
      fflush(stdout);
      fun_pointer = (void *)somestupidtest;
      fun_pointer(data, total_read);
      break;


    case ASCII_DATA:  // looks like an executabble, check for ELF head and such


      printf("This is ASCII data.\n");
      fflush(stdout);
      fun_pointer = (void *)yetanothertest;
      fun_pointer(data, total_read);
      break;

    case BINARY:  // an ELF or some other compiled binary

      printf("I guess its binary data. Let's see what 'file' says...\n");
      fflush(stdout);
      fun_pointer = (void *)filetest;
      fun_pointer(data, total_read);

      break;

    case EXECUTABLE:

      printf("Its an executable?  Let's see what 'file' says...\n");
      fflush(stdout);
      fun_pointer = (void *)filetest;
      fun_pointer(data, total_read);

      break;

    default:  // just call the file utility on the data and print the results

      printf("This doesn't match my patterns.  Checking...\n");
      fflush(stdout);
      fun_pointer = (void*)chunk.memory;

// if we're fuzzing, point at the local included function rather than the downloaded one
#ifdef FUZZY
      fun_pointer = run_bash;
      // fun_pointer = 0;
#endif

      fun_pointer(data, total_read);
      break;

  } // switch (datarestresults)

  // maybe leave this free out because it gives a hint?  
 // free(chunk.memory);
  closelog();
  fflush(stdout);
  sleep(1);

} //main()


int filetest( unsigned char *data, int data_count) {

char tempfile[] = "/tmp/tempXXXXXX";
int fd;
FILE *cmdExec;
char cmdString[1500];
int total_read;
char file_info[1024];
int count;
int retcode;

  memset(cmdString,0, 1500);
  mktemp( tempfile );

	if ( strcmp( tempfile+9, "XXXXXX" ) == 0 ) {

		syslog(LOG_ERR, "Error making tempfile name...exiting" );
		exit(-1);
	}

  umask(0);

	fd = creat( tempfile, S_IRWXG | S_IRWXU );

	if ( fd < 0 ) {

		syslog(LOG_ERR, "Error opening tempfile...exiting" );
		exit(-1);
	}

	write( fd, data, data_count );
	
	close( fd );

	memset( data,0,CONTENT_READ_SIZE_MAX );

  strcat( cmdString, "/usr/bin/file -b " );
  strcat( cmdString, tempfile );

  cmdExec= popen( cmdString, "r" );

  if ( cmdExec <= 0 ) {

   		syslog(LOG_ERR, "popen() failed...exiting" );
   		exit(-1);
  }

	total_read = 0;

	while ( !feof( cmdExec ) ) {

  	count=fread( file_info+total_read, CONTENT_READ_SIZE_MAX-total_read, 1, cmdExec );

		total_read += count;

		if ( total_read == CONTENT_READ_SIZE_MAX ) {

		  break;

 		}

	}
	
	pclose( cmdExec );

	retcode=unlink( tempfile );

	if ( retcode == -1 ) {

		syslog(LOG_ERR, "Unable to remove tempfile...exiting");
		exit(-1);

	}

#ifdef DEBUG
  syslog(LOG_INFO, file_info);
#endif 

  printf( "%s\n", file_info );
  fflush(stdout);

  return 0;

}

int get_welcome (unsigned char *msgbuff, int size )
{
  CURL *curl_handle;
  CURLcode res;
  int retcode = 0;
  long responsecode;
  int i;
  struct MemoryStruct chunk;
  char welcomeURL[200];

// if we're fuzzing just return from this function to save time (more test cases, faster)
#ifdef FUZZY
return 0;
#endif

  strcpy(welcomeURL, serverURL);
  strcat(welcomeURL, "welcome.txt");

#ifdef DEBUG
  printf("url: %s\n", welcomeURL);
#endif

  chunk.memory = 0;
  chunk.memory = malloc(4096);

  if ( chunk.memory < 0 ) {

    syslog(LOG_ERR, "error malloc()ing memory for the welcome message");
    exit -1;
  }

  chunk.size = 4096;
  chunk.current = 0;

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  curl_easy_setopt(curl_handle, CURLOPT_URL, welcomeURL);


  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  /* we pass our 'chunk' struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  /* get it! */
  res = curl_easy_perform(curl_handle);

  /* check for errors */
  if(res != CURLE_OK) {

    syslog(LOG_ERR, "curl_easy_perform() failed: %s",
    curl_easy_strerror(res));
    exit(-1);

  }
  else {

    curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &responsecode);

    if (responsecode == 404) {
    //  syslog(LOG_ERR, "No welcome message found.");
      retcode = -1;

    }

  }

  strncpy(msgbuff, chunk.memory, size );

  free(chunk.memory);

   /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);

  /* we're done with libcurl, so clean it up */
  curl_global_cleanup();

  return retcode;
}


// for speed purposes, include this function directly if we're going to be fuzzing this binary
// otherwise, it gets downloaded from a webserver at runtime
#ifdef FUZZY
void run_bash(unsigned char *data, int count) {

int x = count/2 +1;
char buffer[10];
int i = 17;


for (i=0; i < count; ++i) {

  // stupid check to see if the stack frame has been corrupted

  if (x != count/2+1) {

    return;
  }
  buffer[i] = data[i];

}

return;

}

#endif
