#include<bits/stdc++.h>
#include <fcntl.h>
#include<unistd.h>
#include<fstream>
using namespace std;
/*
void process_command(string in_cmd)
{
	//char *cmd=(char*)in_cmd.c_str();

	istringstream ss(in_cmd);
	string cmd,para1,para2,para_i;

	ss>>cmd;

	if(cmd=="delete_file" || cmd=="delete_dir" || cmd=="goto" || cmd=="search" )	//command with 1 parameter
	{
		ss>>para1;
		//cout<<para1<<" ";
		if(cmd=="delete_file")
		{
			para1.erase(0,1);
			string temp=get_current_dir_name();
			para1=temp + "/" + para1;
			
			remove(para1.c_str());
		}
		else if(cmd=="delete_dir")
		{
			para1.erase(0,1);
			string temp=get_current_dir_name();
			para1=temp + "/" + para1;
			
			cout<<rmdir(para1.c_str());
		}
		
		
	}
	else if(cmd=="create_file" || cmd=="create_dir" )									//command with 2 parameters
	{
		ss>>para1;
		//cout<<para1<<" ";

		ss>>para2;
		//cout<<para2<<" ";
		if(para2==".")
		{
			para2=get_current_dir_name();
			para2=para2+"/"+para1;
		}
		else
		{
			para2.erase(0,1);
			para2=get_current_dir_name();
			para2=para2 + "/" + para1;
		}
		mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
		int fd;
		fd = creat(para2.c_str(), mode);
	}
	else if(cmd=="copy" || cmd=="move" )												//command with multiple parameters
	{
		ss>>para1;
		ss>>para2;
	}

}
*/

/*
void copyfile(string fname,string dest)
{
	ifstream fs;
	ofstream ft;
	char ch, fname1[200], fname2[200];
}
*/
int main()
{
	//string s=get_current_dir_name();
	//cout<<s<<"\n";ha f
	string in_cmd="aiugfa o ab.txt~/sduh",para1;
	istringstream ss(in_cmd);
	//string path=get_current_dir_name();
	vector<string> args;
	while(ss>>para1)
	{
			//ss>>para1;
			args.push_back(para1);
	}
	//process_command("delete_dir  ~/Sample_dir/abc.txt");
	for(int i=0;i<args.size();i++)
		cout<<args[i]<<"\n";
	cout<<args.size()<<"\n";
}



/*

cvoid removeDupWord(string s) 
{ 
	// Returns first token 
  	char *str=(char*)s.c_str();
	char *token = strtok(str, ":"); 

	// Keep printing tokens while one of the 
	// delimiters present in str[]. 
	while (token != NULL) 
	{ 
		printf("%s\n", token); 
		token = strtok(NULL, ":"); 
	} 
} 
*/
/*

if(fork()==0)	//child process
{
	Array=[vi, filepath, NULL]
	execvp(array[0], array)
}
else		// process
{
	
	
}
wait(NULL);


nt maxr=win_row;
			int cur_pos=cursor;			//this will not affect old cursor value
			while(cur_pos != maxr)
			{
				cout<<"\033[1H";		// down
				cur_pos++;
			}
*/