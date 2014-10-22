#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <errno.h>
#include <cstdio>
#include <string>

using namespace std;

void initializer(char ** ptr);

// this function takes in a string and returns it as tokens in the form of a
// char **, it also reutrns the number of arguments including the connectors
void parser(string inp, char ** args, int& count)
{
   count=0; 
	unsigned i=0;
	while(i<inp.size())
	{
		string s;
		while(i<inp.size())
		{
			if(inp.at(i)==' ')
			{
				i++;
				break;
			}
			else if(inp.at(i)=='#')
			{
				if(s.size()>0)
				{
					args[count]=new char[s.size()+1];
					strcpy(args[count],s.c_str());
					count++;
				}
				i=inp.size();
				break;
			}
			else if(inp.at(i)==';')
			{
				if(s.size()>0)
				{
					args[count]=new char[s.size()+1];
					strcpy(args[count],s.c_str());
					count++;
				}
				s.assign(";");
				i++;
				break;
			}
			else if(inp.at(i)=='&')
			{
				if(s.size()>0)
				{
					args[count]=new char[s.size()+1];
					strcpy(args[count],s.c_str());
					count++;
				}
				if(i+1>=inp.size() || inp.at(i+1)!='&')
				{
					cout << "Error: & is not defined" << endl;
					exit(1);
				}
				s.assign("&&");
				i+=2;
				break;
			}
			else if(inp.at(i)=='|')
			{
				if(s.size()>0)
				{
					args[count]=new char[s.size()+1];
					strcpy(args[count],s.c_str());
					count++;
				}
				if(i+1>=inp.size() || inp.at(i+1)!='|')
				{
					cout << "Error: | is not defined" << endl;
					exit(1);
				}
				s.assign("||");
				i+=2;
				break;
			}
			s.push_back(inp.at(i));
			i++;
		}
		if(i>inp.size()) break;
		if(s.size()>0)
		{
			args[count]=new char[s.size()+1];
			strcpy(args[count],s.c_str());
			count++;
		}
	}
}

int breaker(char ** args, char ** curargs, int& index)
{
	initializer(curargs);
	int curargnum=0;
	while(1)
	{
		if(args[index]==0)
		{
			return 0;
			//status will be set to 0 if the command is fully intrprretted.
			//status if 0 when the argument list reaches normal conclusion.
		}
		if(strcmp(args[index],";")==0)
		{
			index++;
			return 1;
			//status will be set to 1 if ; connector is detected.
			//end the loop in next iteration.
		}
		else if(strcmp(args[index],"&&")==0)
		{
			index++;
			return 2;
			//status will be set to 2 if && connector is detected.
			//end the loop on next iteration.
		}
		else if(strcmp(args[index],"||")==0)
		{
			index++;;
			return 3;
			//status will be set to 3 if || is detected.
		}
		else if(strcmp(args[index],"exit")==0)
		{
			cout << "Tester" << endl;
		   return 10;
		}
		else
		{
			curargs[curargnum]=new char[strlen(args[index])+1];
			strcpy(curargs[curargnum],args[index]);
			curargnum++;
			index++;
		}
	}
}

void shell()
{
   string input;
	while(1)
	{
		int* mainstat=new int;
		*mainstat=-1;
		int pid=fork();
		if(0==pid)
		{
			char *hostname=new char[100];
			if(gethostname(hostname,50)==-1) perror("Host Name:");
			cout << getlogin()  <<  "@" << hostname << ": " << flush;
			getline(cin, input);
			char * args[30];
			initializer(args);
			int targs=0;
			parser(input, args, targs);
			int index=0;
			while(1)
			{
				char * curargs[30];
				int exst=breaker(args,curargs,index);
				if(exst==10) return;
				int *status=new int;
				*status=-1;
				int pid2=fork();
				if(0==pid2)
				{
					if(execvp(curargs[0], curargs)==-1)
					perror("Execvp");
					exit(1);
				}
				else if(pid2>0)
				{
					if(wait(status)==-1) perror("Wait failed");
					if(exst==0) break;
					if(WIFEXITED(*status))
					{
						if(exst==3) break;
					}
					else
					{
						if(exst==2) break;
					}
				}
				else
				{
					perror("fork:");
				}
			}
		return;
		}
		else if(pid>0)
		{
			cout << "In main parent" << endl;
			if(wait(mainstat)==-1) perror("wait:");
		   cout << "status to par: " << *mainstat << endl;
			if(WIFEXITED(*mainstat)) cout << "wifexited" << endl;
			cout << "Exit status: " << WEXITSTATUS(*mainstat) << endl;
			if(WIFSIGNALED(*mainstat))
			{
				cout << "WIFSIGNALED" << endl;
			}
			continue;
		}
		else
		{
			perror("fork:");
		}
	}
}

void initializer(char ** ptr)
{
	for(int i=0;i<30;i++)
	{
		ptr[i]=0;
	}
}

int main()
{
	shell();
	return 0;
}
