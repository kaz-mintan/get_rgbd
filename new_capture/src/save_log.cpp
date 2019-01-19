#include <stdio.h>

#include <time.h>
#include <sys/time.h>

void name_file(char *t_name, int mode)
{

// Create all required Nuitrack modules
  gettimeofday(&myTime,NULL);
  t_st=localtime(&myTime.tv_sec);

  time(&timer);
  t_st=localtime(&timer);

  sprintf(t_name,"../rgbd/%d-%d%02d%02d-%02d-%02d-%02d-%06d.csv",
    mode,
    t_st->tm_year+1900,
    t_st->tm_mon+1,
    t_st->tm_mday,
    t_st->tm_hour,
    t_st->tm_min,
    t_st->tm_sec,
    myTime.tv_usec);

}

void *save_log(int mode)
{
  char f_name[50];
  FILE *fp;
  name_file(f_name,mode);

  int i=0;
  switch(mode){
  case 0:
    for(i =0; i<640*480;i++)
    {
      fprintf(fp,"%d",dbuf[i]);
    }
  case 1:
    for(i =0; i<640*480*3;i++)
    {
      fprintf(fp,"%d",cbuf[i]);
    }
  }
}
