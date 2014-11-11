#include <typeinfo>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <vector>
using namespace std;

void order(vector<const char*> &list)
{
	int i,j,min;
	for(i=0;i<list.size();i++)
	{
		min=i;
		for(j=i+1;j<list.size();j++)
		{
			if(strcmp(list.at(j),list.at(min))<0) min=j;
		}
		//swap
		if(min!=i)
		{
			const char * temp=list.at(i);
			list.at(i)=list.at(min);
			list.at(min)=temp;
		}
	}
}

void listmaker(vector<const char*> &nl, vector<bool> &exec, vector<bool> &dir)
{
	struct stat outbuf;
  DIR* currdir=opendir(".");
  if(currdir==NULL) perror("Opendir");
  dirent * curdiren=readdir(currdir);
  if(curdiren ==0 && errno !=0) perror("read"); 
  while(curdiren!=0)
  { 
		nl.push_back(curdiren->d_name);	
    curdiren=readdir(currdir);
    if(curdiren ==0 && errno !=0) perror("read"); 
  }
	order(nl);
	for(int i=0;i<nl.size();i++)
	{
    int x=stat(nl.at(i),&outbuf);
    if(x==-1) perror("Stat");
    if((outbuf.st_mode)&S_IFDIR) dir.push_back(true);
    else dir.push_back(false);
    if((outbuf.st_mode)&S_IXUSR) exec.push_back(true);
    else exec.push_back(false);
	}
  if(closedir(currdir)==-1) perror("Closedir");
}

void normal(bool hid)
{
	vector<const char*> nl;
	vector<bool> exec,dir;
	listmaker(nl,exec,dir);
  for(int i=0;i<nl.size();i++)
  {
    if(exec.at(i)) cout << "\x1b[32;1m";
    if(dir.at(i)) cout << "\x1b[34m";
    if(nl.at(i)[0]=='.')
    {
      if(hid)
      {
        cout << "\x1b[40;1m";
        cout << nl.at(i) << endl;
      }
    }
    else cout << nl.at(i) << endl;
    cout << "\x1b[0m";
  }
}

int main()
{
	//normal(1);
	return 0;
}
