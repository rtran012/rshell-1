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
#include <stdlib.h>
using namespace std;

bool is_dir(char * path)
{
	struct stat outbuf;
  int x=lstat(path,&outbuf);
  if(x==-1) perror("Stat");
	if(S_ISDIR(outbuf.st_mode)) return true;;
  return false;
}

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

void listmakerpro(vector<char*> &nl,char * direc)
{
  DIR* currdir=opendir(direc);
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

void nprint(char * path)
{
	struct stat outbuf;
	int x=lstat(path,&outbuf);
	if(x==-1) perror("Stat");

	if((outbuf.st_mode)&S_IXUSR) cout << "\x1b[32;1m";
	if((outbuf.st_mode)&S_IFDIR) cout << "\x1b[34m";
	if(S_ISLNK(outbuf.st_mode))	cout << "\x1b[36;1m";
	if(path[0]=='.') cout << "\x1b[40;1m";
	cout << path;	
	cout << "\x1b[0m";
	cout << "  ";
}

void ndprint(bool showhid,char * cdir)
{
	struct winsize w;
	int used=0;
	if(ioctl(0,TIOCGWINSZ,&w)==-1) perror("ioctl");
	vector<char*> nl;
	listmakerpro(nl,cdir);
  for(int i=0;i<nl.size();i++)
  {
		if(nl.at(i)[0]=='.' && !showhid) continue;
		if(((w.ws_col-used)<(strlen(nl.at(i)))) && used!=0)
		{
			cout << endl;
			used=0;
		}
		nprint(nl.at(i));
		used+=(strlen(nl.at(i))+2);
	}
	cout << endl;
}
void lprint(char * path)
{
	struct stat outbuf;
  int x=lstat(path,&outbuf);
  if(x==-1) perror("Stat");
  
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
	cout << " " << at->tm_hour << ":";
	if(at->tm_min<10) cout << '0';
	cout << at->tm_min;

	cout << " ";
	if((outbuf.st_mode)&S_IXUSR) cout << "\x1b[32;1m";
  if((outbuf.st_mode)&S_IFDIR) cout << "\x1b[34m";
	if(S_ISLNK(outbuf.st_mode))	cout << "\x1b[36;1m";
  if(path[0]=='.') cout << "\x1b[40;1m";
	cout << path;	
	cout << "\x1b[0m";
	cout << endl;
}

void ldprint(bool showhid, char * cdir)
{
	vector<char*> nl;
	listmakerpro(nl,cdir);
  for(int i=0;i<nl.size();i++)
  {
		if(nl.at(i)[0]=='.')
		{
			if(showhid) lprint(nl.at(i));
		}
		else lprint(nl.at(i));
	}
}

int parser(int c,char ** args, vector<char*> &pnl)
{
	int flag=0;
	for(int i=1;i<c;i++)
	{
		if(args[i][0]=='-')
		{
			for(int j=1;j<strlen(args[i]);j++)
			{
				switch(args[i][j])
				{
					case 'a':
						flag=flag|1;
						break;
					
					case 'l':
						flag=flag|2;
						break;
					
					case 'R':
						flag=flag|4;
						break;

					default:
						cerr << "Invalid flag. Try -a, -l or -R" << endl;
						exit(1);
				}
			}
		}
		else
		{
			pnl.push_back(0);	
			pnl.back()=new char[strlen(args[i])+1];
			strcpy(pnl.back(),args[i]);
		}
	}
	return flag;
}

int main(int c, char** args)
{
	vector<char*> pnl;
	int flag=parser(c,args,pnl);
	if(pnl.size()==0)
	{
		char* cdir=new char[2];
		strcpy(cdir,".");
		if(flag&2) ldprint(flag&1,cdir);
		else ndprint(flag&1,cdir);
	}
	//normal(1);
	//detailed(0);
	//char* path=new char[256];
	//strcpy(path,".");
	//cout << "ISDIR?? " << is_dir(path);	
	//ndprint(0,path);
	//ldprint(0,path);
	
	return 0;
}
