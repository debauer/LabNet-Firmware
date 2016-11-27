
#ifdef ino_file
	#define EXTERN 
#else
	#define EXTERN extern
#endif

EXTERN float temperatur = 0;
EXTERN int nodeStatus = 1; // 1=ok, 2=warn, 3=error
EXTERN float voltage24;
