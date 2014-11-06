#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <unistd.h>
#include "Timer.h"
using namespace std;

void method1(char **argv)
{
  cout << "Method 1" << endl;
  Timer t;
  double etime;
  t.start();
  ifstream ifs;
  ifs.open(argv[1]);
  if(!ifs.is_open()) perror("Open");
  ofstream ofs;
  ofs.open(argv[2]);
  if(!ofs.is_open()) perror("Open");
  char swap;
  while(1)
  {
    if(ifs.eof()) break;
    ifs.get(swap);
    ofs.put(swap);
  }
  t.elapsedUserTime(etime);
  cout << "Elapsed User Time: " << etime << endl;
  t.elapsedSystemTime(etime);
  cout << "Elapsed System Time: " << etime << endl;
  t.elapsedWallclockTime(etime);
  cout << "Elapsed Wallclock Time: " << etime << endl;
  ifs.close();
  ofs.close();
}
void method2(char ** argv)
{
  cout << "Method 2" << endl;
  Timer t;
  double etime;
  t.start();
  int fi=open(argv[1],O_RDONLY);
  if(fi==-1) perror("Open");
  int fo=open(argv[2],O_RDWR | O_CREAT);
  if(fo==-1) perror("Open");
  char * pswap=new char;
  int rsize,wsize;
  while(1)
  {
    rsize=read(fi,pswap,1);
    if(rsize==0) break;
    else if (rsize==-1)
    {
      perror("Read");
      exit(1);
    }
    wsize=write(fo,pswap,1);
    if(wsize==-1)
    {
      perror("Write");
      exit(1);
    }
  }
  t.elapsedUserTime(etime);
  cout << "Elapsed User Time: " << etime << endl;
  t.elapsedSystemTime(etime);
  cout << "Elapsed System Time: " << etime << endl;
  t.elapsedWallclockTime(etime);
  cout << "Elapsed Wallclock Time: " << etime << endl;
  delete pswap;
  if(close(fi)==-1) perror("Close");
  if(close(fo)==-1) perror("Close");
}
void method3(char ** argv)
{
  cout << "Method 3" << endl;
  Timer t;
  double etime;
  t.start();
  int fi=open(argv[1],O_RDONLY);
  if(fi==-1) perror("Open");
  int fo=open(argv[2],O_RDWR | O_CREAT);
  if(fo==-1) perror("Open");
  char * pswap=new char[BUFSIZ];
  int rsize,wsize;
  while(1)
  {
    rsize=read(fi,pswap,BUFSIZ);
    if(rsize==0) break;
    else if (rsize==-1)
    {
      perror("Read");
      exit(1);
    }
    wsize=write(fo,pswap,BUFSIZ);
    if(wsize==-1)
    {
      perror("Write");
      exit(1);
    }
  }
  t.elapsedUserTime(etime);
  cout << "Elapsed User Time: " << etime << endl;
  t.elapsedSystemTime(etime);
  cout << "Elapsed System Time: " << etime << endl;
  t.elapsedWallclockTime(etime);
  cout << "Elapsed Wallclock Time: " << etime << endl;
  delete[] pswap;
  if(close(fi)==-1) perror("Close");
  if(close(fo)==-1) perror("Close");
}

int main(int c, char ** args)
{
  /*if(c!=3)
  {
    cerr << "Invalid Syntax";
    exit(1);
  }*/
  //method1(args);
  method2(args);
  //method3(args);
}
