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
#include <grp.h>
#include <pwd.h>
#include <time.h>
using namespace std;

void order(vector<char*> &list)
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
			char * temp=list.at(i);
			list.at(i)=list.at(min);
			list.at(min)=temp;
		}
	}
}

void listmaker(vector<char*> &nl, vector<bool> &exec, vector<bool> &dir)
{
	struct stat outbuf;
  DIR* currdir=opendir(".");
  if(currdir==NULL) perror("Opendir");
  dirent * curdiren=readdir(currdir);
  if(curdiren ==0 && errno !=0) perror("read"); 
  while(curdiren!=0)
  { 
		nl.push_back(0);	
		nl.back()=new char[strlen(curdiren->d_name)+1];
		strcpy(nl.back(),curdiren->d_name);
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
	vector<char*> nl;
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

void minilistmaker(vector<char*> &nl)
{
  DIR* currdir=opendir(".");
  if(currdir==NULL) perror("Opendir");
  dirent * curdiren=readdir(currdir);
  if(curdiren ==0 && errno !=0) perror("read"); 
  while(curdiren!=0)
  { 
		nl.push_back(0);	
		nl.back()=new char[strlen(curdiren->d_name)+1];
		strcpy(nl.back(),curdiren->d_name);
    curdiren=readdir(currdir);
    if(curdiren ==0 && errno !=0) perror("read"); 
  }
	order(nl); 
	if(closedir(currdir)==-1) perror("Closedir");
}

void dprinter(char * path)
{
	struct stat outbuf;
  int x=lstat(path,&outbuf);
  if(x==-1) perror("Stat");
  
//	cout << "dir: " << S_ISDIR(outbuf.st_mode) << " link: " << S_ISLNK(outbuf.st_mode) << " reg: "<< S_ISREG(outbuf.st_mode) <<  endl;
	if(S_ISDIR(outbuf.st_mode)) cout << 'd';
  else if(S_ISLNK(outbuf.st_mode)) cout << 'l';
	else cout << '-';
  
	if((outbuf.st_mode)&S_IRUSR) cout << 'r';
	else cout << '-';
	if((outbuf.st_mode)&S_IWUSR) cout << 'w';
	else cout << '-';
	if((outbuf.st_mode)&S_IXUSR) cout << 'x';
	else cout << '-';

	if((outbuf.st_mode)&S_IRGRP) cout << 'r';
	else cout << '-';
	if((outbuf.st_mode)&S_IWGRP) cout << 'w';
	else cout << '-';
	if((outbuf.st_mode)&S_IXGRP) cout << 'x';
	else cout << '-';

	if((outbuf.st_mode)&S_IROTH) cout << 'r';
	else cout << '-';
	if((outbuf.st_mode)&S_IWOTH) cout << 'w';
	else cout << '-';
	if((outbuf.st_mode)&S_IXOTH) cout << 'x';
	else cout << '-';

	cout << " ";
	cout << outbuf.st_nlink;

	cout << " ";
	struct passwd * pwd=getpwuid(outbuf.st_uid);
	cout << pwd->pw_name;

	cout << " ";
	struct group * grp=getgrgid(outbuf.st_gid);
	cout << grp->gr_name;
	
	cout << " ";
	cout << outbuf.st_size;

	cout << " ";
	struct tm *at=localtime(&outbuf.st_mtime);
	cout << at->tm_mon << '/' << at->tm_mday << '/' << at->tm_year+1900;
	cout << " " << at->tm_hour << ":" << at->tm_min;

	cout << " ";
	if((outbuf.st_mode)&S_IXUSR) cout << "\x1b[32;1m";
  if((outbuf.st_mode)&S_IFDIR) cout << "\x1b[34m";
	if(S_ISLNK(outbuf.st_mode))	cout << "\x1b[36;1m";
  if(path[0]=='.') cout << "\x1b[40;1m";
	cout << path;	
	cout << "\x1b[0m";
	cout << endl;
}

void detailed(bool showhid)
{
	vector<char*> nl;
	minilistmaker(nl);
  for(int i=0;i<nl.size();i++)
  {
		if(nl.at(i)[0]=='.')
		{
			if(showhid) dprinter(nl.at(i));
		}
		else dprinter(nl.at(i));
	}
}
int main()
{
	//normal(1);
	detailed(1);
	return 0;
}
