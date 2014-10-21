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

void parser(string inp, char ** args)
{
   unsigned count=0; 
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
	memset(curargs,0,30);
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
		else if(args[index]==";")
		{
			stopstat=1;
			//status will be set to 1 if ; connector is detected.
			index++;
			break;
			//end the loop in next iteration.
		}
		else if(args[index]=="&&")
		{
			stopstat=2;
			//status will be set to 2 if && connector is detected.
			index++;
			break;
			//end the loop on next iteration.
		}
		else if(args[index]=="||")
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
		cout << endl << "rshell: " << flush;
		getline(cin, input);
		int pid=fork();
		if(0==pid)
		{
			char * args[30];
			memset(args,0,30);
			parser(input, args);
			bool x=true;
			int argnum=0;
			while(x)
			{
				x=false;
				char* curargs[30];
				memset(args,0,30);
				int curargnum=0;
				bool y=true;
				int status=0;
				while(y)
				{
					if(args[argnum]==0)
					{
						status=4;
						//status will be set to 4 if the command is fully intrprted.
						break;
					}
					//status if 0 when the argument list reaches normal conclusion.
					else if(args[argnum]=="&&")
					{
						status=1;
						//status will be set to 1 if && connector is detected.
						y=false;
						//end the loop in next iteration.
					}
					else if(args[argnum]=="||")
					{
						status=2;
						//status will be set to 2 if || connector is detected.
						y=false;
						//end the loop on next iteration.
					}
					else if(args[argnum]==";")
					{
						status=3;
						//status will be set to 3 if ; is detected.
						y=false;
					}
					else
					{
						strcpy(curargs[curargnum],args[argnum]);
						curargnum++;
					}
					argnum++;
				}
				int pid2=fork();
				if(0==pid2)
				{
					for(int i=0;i<curargnum;i++)
					{
						cout << curargs[i] << endl;
					}
					int exec=execvp(curargs[0],curargs);
					if(exec==-1) cout << "Error: Execution failed" << endl;
				}
				else if(pid2>0)
				{
					int ex=wait(NULL);
				}
				else
				{
					cout << "Error: Forking failed" << endl;
				}
				if(status==4) x=false;
				
			}
		}
		else if(pid>0)
		{
			wait(NULL);
		}
		else
		{
			cout << "Error: could not fork process" << endl;
		}
	}
}

int main()
{
	string input;
	char* args[30];
	memset(args,0,30);
	cout << "Input: " << flush;
	getline(cin, input);
	parser(input, args);
	int stopstat=-1;
	int index=0;
	while(stopstat!=0)
	{
		char * curargs[30];
		breaker(args,curargs,index,stopstat);
		cout << "Exited braker" << endl;
		for(int j=0; curargs[j]!=0;j++)
		{
			cout << curargs[j] << " " << flush;
		}
		cout << endl;
	}
	cout << "Done Breaking" << endl;
	/*for(int i=0;args[i]!=0;i++)
	{
		cout << args[i] << endl;
	}
	int x=execvp(args[0],args);
	cout << x << endl;
	*/
	return 0;
}
