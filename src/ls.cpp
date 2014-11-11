#include <typeinfo>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
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
			if(strcasecmp(list.at(j),list.at(min))<0) min=j;
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

void normal(bool showhid)
{
	struct winsize w;
	int used=0;
	if(ioctl(0,TIOCGWINSZ,&w)==-1) perror("ioctl");
	vector<const char*> nl;
	vector<bool> exec,dir;
	listmaker(nl,exec,dir);
  for(int i=0;i<nl.size();i++)
  {
    if(exec.at(i)) cout << "\x1b[32;1m";
    if(dir.at(i)) cout << "\x1b[34m";
    if(nl.at(i)[0]=='.')
    {
      if(showhid)
      {
        cout << "\x1b[40;1m";
				if((w.ws_col-used)<strlen(nl.at(i)))
				{
					cout << endl << nl.at(i) << "\x1b[0m" << "  ";	
					used=0;
				}
				else
				{
					cout << nl.at(i) << "\x1b[0m" << "  ";
					used+=(strlen(nl.at(i))+2);
				}
			}
    }
    else
		{
			if((w.ws_col-used)<strlen(nl.at(i)))
			{
				cout << endl << nl.at(i) << "\x1b[0m" << "  ";	
				used=0;
			}
			else
			{
				cout << nl.at(i) << "\x1b[0m" << "  ";
				used+=(strlen(nl.at(i))+2);
			}
		}	
		cout << "\x1b[0m";
  }
	cout << endl;
}

void minilistmaker(vector<const char*> &nl)
{
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
	if(closedir(currdir)==-1) perror("Closedir");
}

void dprinter(const char * path)
{
	struct stat outbuf;
  int x=stat(path,&outbuf);
  if(x==-1) perror("Stat");
    if((outbuf.st_mode)&S_IFDIR) dir.push_back(true);
    else dir.push_back(false);
    if((outbuf.st_mode)&S_IXUSR) exec.push_back(true);
    else exec.push_back(false);
}

void detailed(bool showhid)
{
	vector<const char*> nl;
	minilistmaker(nl);
  for(int i=0;i<nl.size();i++)
  {
		dprinter(nl.at(i));
	}
}
int main()
{
	//normal(0);
	
	return 0;
}
