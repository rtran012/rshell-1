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

void breaker(char ** args, char ** curargs, int& index, int& stopstat)
{
	initializer(curargs);
	int curargnum=0;
	while(1)
	{
		if(args[index]==0)
		{
			stopstat=0;
			//status will be set to 0 if the command is fully intrprretted.
			break;
		}
			//status if 0 when the argument list reaches normal conclusion.
		if(strcmp(args[index],";")==0)
		{
			stopstat=1;
			//status will be set to 1 if ; connector is detected.
			index++;
			break;
			//end the loop in next iteration.
		}
		else if(strcmp(args[index],"&&")==0)
		{
			stopstat=2;
			//status will be set to 2 if && connector is detected.
			index++;
			break;
			//end the loop on next iteration.
		}
		else if(strcmp(args[index],"||")==0)
		{
			stopstat=3;
			//status will be set to 3 if || is detected.
			index++;;
			break;
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
		int pid=fork();
		cout << "PId 1 is " << pid << endl;
		if(0==pid)
		{
			cout << "rshell: " << flush;
			getline(cin, input);
			char * args[30];
			initializer(args);
			int targs=0;
			parser(input, args, targs);
			int stopstat=-1;
			int index=0;
			while(stopstat!=0)
			{
				char * curargs[30];
				breaker(args,curargs,index,stopstat);
				cout << "curr stopstatus is " << stopstat << endl;
				for(int j=0; args[j]!=0;j++)
				{
					cout << curargs[j] << " " << j  <<  endl;
				}
				cout << endl;
				cout << "pre check"<< endl;
				int pid2=fork();
				cout << "pid 2 " <<  pid2 << endl;
				if(0==pid2)
				{
					cout << "I am child" << endl;
					if(execvp(curargs[0], curargs)==-1)
					perror("Execution failed");
					exit(0);
				}
				else if(pid2>0)
				{
					cout << "Tester" << endl;
					int *status=new int;
					*status=-1;
					if(wait(status)==-1) perror("Wait failed");
				   cout << "I am parent" << endl;
					cout << "Status is: " << *status << endl;
					if(*status!=0)
					{
						cout << "Error: invalid command" << endl;
						if(stopstat==2) break;
					}
					else
					{
						if(stopstat==3) break;
					}
				}
			}
			exit(0);
		}
		else if(pid>0)
		{
			wait(NULL);
			continue;
		}
		else
		{
			cout << "Error: could not fork process" << endl;
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
/*	string input;
	char* args[30];
	initializer(args);
	int targs=0;
	cout << "Input: " << flush;
	getline(cin, input);
	parser(input, args, targs);
	for(int ii=0;args[ii]!=0;ii++)
	{
		cout << args[ii] << " " << flush; 
	}
	cout << "Check 1 " << endl;
	int stopstat=-1;
	int index=0;
	while(stopstat!=0)
	{
		char * curargs[30];
		breaker(args,curargs,index,stopstat);
		for(int j=0; args[j]!=0;j++)
		{
			cout << curargs[j] << " " << flush;
		}
		cout << endl;
		int pid4=fork();
		if(0==pid4)
		{
			cout << "I am child" << endl;
			int x=execvp(curargs[0], curargs);
			cout << "Error" << endl;
			exit(0);

		}
		else if(pid4>0)
		{
			int *status=new int;
			*status=-1;
			if(wait(status)==-1) perror("Wait failed");
			cout << "Status is: " << *status << endl;
			if(*status!=0) cout << "Error: invalid command" << endl;
		}
	}*/
	
	return 0;
}
