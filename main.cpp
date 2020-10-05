#include<bits/stdc++.h>
#include<fstream>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>		//creat() 
#include <termios.h>
#include <unistd.h>
#include <time.h>		//last modified
#include <pwd.h>
#include <grp.h>
#include<cstdio>

using namespace std;

///////////////////////////////////global vairables///////////////////////////////////////////////////
static int st=0,ed=19,cursor=0,win_row=30,win_col=120;
vector<string> dir_list;			
string in_cmd="";  					//input command in command mode
int cflag=0;						//command mode flag
string path=get_current_dir_name();	//keep track of currently visible path
stack<string>	back_stack,fwd_stack;									//stack to store parent paths
/////////////////////////////////////////////////////////////////////////////////////////////////////
void clear_screen()
{
	cout<<"\033c";
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

int getch(void)
{
 int ch;
 struct termios oldt;
 struct termios newt;
 tcgetattr(STDIN_FILENO, &oldt); 
 newt = oldt;  
 newt.c_lflag &= ~(ICANON | ECHO); 					//non canonical and no echo
 tcsetattr(STDIN_FILENO, TCSANOW, &newt);
 ch = getchar(); 
 tcsetattr(STDIN_FILENO, TCSANOW, &oldt); 
 return ch; 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
//list directory files
vector<string> list_files(string path)
{
	clear_screen();
    struct dirent *dp;

    char path_arr[path.length()];
    strcpy(path_arr, path.c_str()); 			//char string to char array

    DIR *dir = opendir(path_arr);				

	vector<string> v;
    if (!dir) 
        return v; 
    
    struct stat stats;
    stat(path_arr, &stats);

    while ((dp = readdir(dir)) != NULL)
    {
        string name=(dp->d_name);
        if(name[0] != '.')			//ignore hidden files
        {
        	
        	//name = name +"  ";
        	//name = name + to_string(stats.st_size);
        	v.push_back(name);
        }  	
    }

	
	v.push_back(".");					//current dir
	v.push_back("..");					//back dir
	sort(v.begin(),v.end());
	/*
	for(int i=st;i<(st+lines) && i<v.size();i++)
        	cout<<v[i]<<"\n";
    */	
    closedir(dir);
    return v;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

void print_dir(vector<string> &dir_list,int st,int ed)
{
	cout<<"\033c";			//clear outpur screen
	cout << "\033[H";		//cursor at top

	struct stat stats;
	struct tm dt;
	
        
	for(int i=st;i <= ed && i<dir_list.size() ;i++)
	{
	   
	    //file metadata
        string file_path = path + "/" + dir_list[i];
        if ( (stat(file_path.c_str(), &stats) == 0)  &&  dir_list[i]!="." && dir_list[i]!="..")		//stat() returns 0 on successful operation, otherwise returns -1 if unable to get file properties.
   		{
   			//permissions
            cout<<( (S_ISDIR(stats.st_mode)) ? "d" : "-");
    		cout<<( (stats.st_mode & S_IRUSR) ? "r" : "-");
    		cout<<( (stats.st_mode & S_IWUSR) ? "w" : "-");
    		cout<<( (stats.st_mode & S_IXUSR) ? "x" : "-");
    		cout<<( (stats.st_mode & S_IRGRP) ? "r" : "-");
    		cout<<( (stats.st_mode & S_IWGRP) ? "w" : "-");
    		cout<<( (stats.st_mode & S_IXGRP) ? "x" : "-");
    		cout<<( (stats.st_mode & S_IROTH) ? "r" : "-");
    		cout<<( (stats.st_mode & S_IWOTH) ? "w" : "-");
    		cout<<( (stats.st_mode & S_IXOTH) ? "x" : "-");
   			cout<<"   ";   				

   			//ownership user and gorup
   			struct passwd *usr = getpwuid(stats.st_uid);
			struct group  *grp = getgrgid(stats.st_gid);
   			if(usr)
   				 cout<<usr->pw_name<<" ";
   			if(grp)
   				 cout<<grp->gr_name<<"\t";
   			
   			//modified date time
	        dt = *(gmtime(&stats.st_mtime));		
	        cout<<dt.tm_mday<<"-"<<dt.tm_mon<<"-"<<(dt.tm_year + 1900)<<" "; 
            cout<<dt.tm_hour<<":"<<dt.tm_min<<":"<<dt.tm_sec<<"\t\t";
            //cout<<setw(4);

   			//size
	        cout<<stats.st_size<<" bytes\t\t";
	        //cout<<setw(4);
	        

            //filename
   			cout<<dir_list[i]<<"\t\t";

   			
    	}
    	else if((stat(file_path.c_str(), &stats) == 0))
    	{
    		//filename
   			cout<<dir_list[i]<<"\t\t\t";
    	}	
    	cout<<"\n";
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

void upkey(int &st,int &ed,int &cursor, vector<string> &dir_list)
{
	if(cursor >= ed && cursor<dir_list.size() - 1 )
	{
		cout<<"\033[1B";		// down
	   	st++;
	   	ed++;
	   	cursor++;
	   	cout<<"\033c";
	 	print_dir(dir_list,st,ed);
	 	cout<<"\033[1A";		// up
	}
	else if(cursor<dir_list.size()-1)
	{		
	 	cursor++;
	 	cout<<"\033[1B";		// down
	} 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void downkey(int &st,int &ed,int &cursor, vector<string> &dir_list)
{
	if(cursor <= st && cursor>0 )
	{
		cout<<"\033[1A";		// up
	   	st--;
	   	ed--;
	   	cursor--;
	   	cout<<"\033c";			//clear outpur screen
	 	print_dir(dir_list,st,ed);
	 	cout<<"\033[1H";		//cursor at position 0
	}
	else if(cursor>0)
	{		
	 	cursor--;
	 	cout<<"\033[1A";		// up
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void leftkey(int &st, int &ed, int &cursor, vector<string> &dir_list, stack<string> &back_stack, stack<string> &fwd_stack, string &path)
{
	if(!back_stack.empty())
	{
		fwd_stack.push(path);
		path=back_stack.top();							
		back_stack.pop();								//remove entry from back stack

		dir_list = list_files(path);					//update current directory list

		st=0,ed=19,cursor=0;							//update pointers
		print_dir(dir_list,st,ed);						//print diretory
		cout << "\033[H";								//cursor at position 0
	}								
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void rightkey(int &st, int &ed, int &cursor, vector<string> &dir_list, stack<string> &back_stack , stack<string> &fwd_stack, string &path)
{
	if(!fwd_stack.empty())
	{
		back_stack.push(path);
		path=fwd_stack.top();							
		fwd_stack.pop();								//remove entry from back stack

		dir_list = list_files(path);					//update current directory list

		st=0,ed=19,cursor=0;							//update pointers
		print_dir(dir_list,st,ed);						//print diretory
		cout << "\033[H";								//cursor at position 0
	}								
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

void backkey(int &st, int &ed, int &cursor, vector<string> &dir_list, stack<string> &back_stack , stack<string> &fwd_stack, string &path)				//functionality: up one level
{
	int i;
	if(path == get_current_dir_name())						//if root is reached do not go back
		return;

	while(!fwd_stack.empty())								//empty the forward stack as there is no forward to new directory
		fwd_stack.pop();

	back_stack.push(path);									//previous directory pushed to back_stack


	for(i=path.length()-1 ; i>=0 && path[i]!='/' ; i--)		//find first '/' from end
		;			

	if(i>5)												//check at index 5 as /home/ is 6 characters 
		path.erase(i);
	else
		path="/home/beerus";
	
	if(i>=0)
	{	
		st=0,ed=19,cursor=0;							//update pointers
		dir_list=list_files(path);						//print diretory
		print_dir(dir_list,st,ed);
		cout << "\033[H";								//cursor at position 0
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void enterkey(int &st, int &ed, int &cursor, vector<string> &dir_list, stack<string> &back_stack, stack<string> &fwd_stack, string &path)
{
	struct stat fileStat;								//create a stat strict
	string temp = path + "/" + dir_list[cursor];		//temporarily store path of file pointed by cursor
	char char_array[temp.length()+1];					//convert path to char array to pas to stat function
	strcpy(char_array, temp.c_str()); 
	int stat_flag = stat(char_array, &fileStat);		//check if stat worked. if flag<0 then error		

	if(dir_list[cursor] == "..")
	{
		backkey(st, ed, cursor, dir_list,back_stack, fwd_stack, path);
		return ;
	}
	else if(dir_list[cursor] == ".")
	{
		return;
	}
	else if(stat_flag>=0 && S_ISDIR(fileStat.st_mode))					//if cursor is on  directory
	{
		back_stack.push(path);

		string cur_entry = dir_list[cursor];			//get name pointed by cursor
		path = path + "/" + cur_entry;					//update path
		//up_stack.push(path);							//push  new path in up stack
		dir_list = list_files(path);					//update current directory list

		st=0,ed=19,cursor=0;							//update pointers
		print_dir(dir_list,st,ed);						//print diretory
		cout << "\033[H";								//cursor at position 0
	}
	else if(stat_flag>=0 && !S_ISDIR(fileStat.st_mode))	//cursor points to file
	{
		if(fork()==0)	//child process
		{
			string cmd="vi";
			string file=path + "/" +dir_list[cursor];
			char* arg[3];
			arg[0]=(char*)cmd.c_str();					//args=[vi, filepath, NULL]
			arg[1]=(char*)file.c_str();
			arg[2]=NULL;
			execvp(arg[0], arg);
		}
		wait(NULL);
	}
	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void homekey(int &st, int &ed, int &cursor, vector<string> &dir_list, stack<string> &back_stack, stack<string> &fwd_stack ,string &path)
{
	while(!fwd_stack.empty())								//empty the forward stack as there is no forward to homr directory
		fwd_stack.pop();
	


	path = get_current_dir_name();					//home dir path
	dir_list = list_files(path);					//update current directory list
	st=0,ed=19,cursor=0;							//update pointers
	print_dir(dir_list,st,ed);						//print diretory
	cout << "\033[H";								//cursor at position 0

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void setcommandmode()
{
	cflag=1;						//command mode flag
	int maxr = win_row;
	int cur_pos = cursor-st;			//this will not affect old cursor value
	cout<<"\033[H";
	cout<<"\033["<<(win_row-1)<<"B";
										//command mode keys
	in_cmd="";							//input command

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void createfile(string para1,string para2)
{
	if(para2==".")						//current dir
	{
		para2=path;
		para2=para2+"/"+para1;
	}
	else if(para2[0]=='~')							//relative path
	{
		para2.erase(0,1);							//remove ~ sign
		string cwd=get_current_dir_name();
		para2=cwd + "/" + para2 + "/" + para1;		//create absolute path
	}

	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH ;		//set permissions (byrant PDF page 864)
	int fd;			//store fd
	fd = creat(para2.c_str(), mode);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void createdir(string para1,string para2)
{
	if(para2==".")						//current dir
	{
		para2=path;
		para2=para2+"/"+para1;
	}
	else if(para2[0]=='~')							//relative path
	{
		para2.erase(0,1);							//remove ~ sign
		string cwd=get_current_dir_name();
		para2=cwd + "/" + para2 + "/" + para1;		//create absolute path
	}

	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH ;		//set permissions (byrant PDF page 864)
	int fd;			//store fd
	fd = mkdir(para2.c_str(), mode);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void deletefile(string para1)
{
	para1.erase(0,1);						//remove ~ sign
	string temp=get_current_dir_name();		
	para1=temp + "/" + para1;				//create abs path
	remove(para1.c_str());					//convert to char* and delete
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

void goto_dir(string para1)
{
	while(!fwd_stack.empty())		//empty fwd stack
		fwd_stack.pop();
	back_stack.push(path);			//push cur path to back stack

	para1.erase(0,1);						//remove ~ sign
	string cwd=get_current_dir_name();
	path = cwd + "/" + para1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

					//name        //dir path
void movefile(string para1,string para2)
{

	string cwd = get_current_dir_name();
	string src = path + "/" + para1;
	string dest ;
	if(para2==".")
	{
		dest=path + "/" + para1;
	}
	else if(para2[0]=='~')
	{
		para2.erase(0,1);
		dest =  cwd + "/" + para2 +"/" + para1;
	}
	//createfile(para1,para2);
	if (rename(src.c_str(), dest.c_str()) != 0)
		return;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
				  //oldname   //newname
void renamefile(string para1,string para2)
{
	string cwd = path;
	string src = cwd+"/"+para1;
	string dest = cwd+"/"+para2;
	if (rename(src.c_str(), dest.c_str()) != 0)
		return;


}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void copyfile(string para1,string para2)
{

	string cwd = get_current_dir_name();
	string src = cwd + "/" + para1;
	string dest ;
	if(para2==".")
	{
		dest=path + "/" + para1;
	}
	else if(para2[0]=='~')
	{
		para2.erase(0,1);
		dest =  cwd + "/" + para2 +"/" + para1 ;
	}

	int fd1 = open(src.c_str(), O_RDONLY, 0);				//bryant ch10
    int fd2 = open(dest.c_str(), O_WRONLY | O_CREAT, 0);
    char ch;
    //cout<<endl;
    while(read(fd1, &ch, 1) != 0)
        write(fd2, &ch, 1);
    
    close(fd1);
    close(fd2);

    chmod(dest.c_str(),S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
    struct stat status;

    if (stat(src.c_str(), &status) == -1) 
    {
    	cout<<"afugua";
        perror("stat");
        exit(EXIT_FAILURE);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
 
 ///////////////////////////////BE CAREFUL WHEN RUNNING THIS COMMAND!!!!!!!/////////////////////////


void deletedir(string para1)				//here para 1 is abs path
{
	
	vector<string> delete_files = list_files(para1);

	//DFS for delete
	for(int i=0 ; i<delete_files.size() ; i++)
	{
		struct stat fileStat;								//create a stat strict
		string temp1 = para1 + "/" + delete_files[i];			//temporarily store path of file pointed by cursor
		char char_array[temp1.length()+1];					//convert path to char array to pas to stat function
		strcpy(char_array, temp1.c_str()); 
		int stat_flag = stat(char_array, &fileStat);		//check if stat worked. if flag<0 then error	

		if(delete_files[i]!="." && delete_files[i]!="..")
		{
			if(stat_flag>=0 && S_ISDIR(fileStat.st_mode))					//if cursor is on  directory
			{

				deletedir(para1 + "/" + delete_files[i]);		//path from application run
				rmdir(temp1.c_str());
				
			}
			else if(stat_flag>=0 && !S_ISDIR(fileStat.st_mode))	//cursor points to file
			{
				remove(temp1.c_str());					//convert to char* and delete
			}
		}
	}

	rmdir(para1.c_str());


}

/////////////////////////////////////////////////////////////////////////////////////////////////////
bool searchfile(string fname,string para1)				//here para 1 is abs path
{
	
	vector<string> search_list = list_files(para1);

	//DFS for delete
	for(int i=0 ; i<search_list.size() ; i++)
	{
		

		struct stat fileStat;								//create a stat strict
		string temp1 = para1 + "/" + search_list[i];			//temporarily store path of file pointed by cursor
		char char_array[temp1.length()+1];					//convert path to char array to pas to stat function
		strcpy(char_array, temp1.c_str()); 
		int stat_flag = stat(char_array, &fileStat);		//check if stat worked. if flag<0 then error	


		if(search_list[i]!="." && search_list[i]!="..")
		{
			if(search_list[i]==fname)
				return true;
			else if(stat_flag>=0 && S_ISDIR(fileStat.st_mode))					
			{
				string temp=para1 + "/" + search_list[i];
				if(searchfile(fname,temp))				//recursive call to search dir
					return true;
			}
		}
	}
	return false;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void process_command(string in_cmd)
{
	//char *cmd=(char*)in_cmd.c_str();
	istringstream ss(in_cmd);
	string cmd,para1,para2,para_i;

	ss>>cmd;

	if(cmd=="delete_file" || cmd=="delete_dir" || cmd=="goto" || cmd=="search" )	//command with 1 parameter
	{
		ss>>para1;
		if(cmd=="delete_file")
			deletefile(para1);
		else if(cmd=="goto")
			goto_dir(para1);
		else if(cmd=="delete_dir")
		{
			string cwd = get_current_dir_name();
			para1 = cwd + "/" + para1.erase(0,1);
			deletedir(para1);
		}
		else if(cmd=="search")
		{
			bool search_flag = searchfile(para1,path);
			int len=in_cmd.length()-1;
			while(len--)				//remove printed characters
					cout<<"\b \b";
			if(search_flag)
				
				cout<<"True\nPress any key to continue...";
			else
				cout<<"False\nPress any key to continue...";
			getch();
		}
	}

		
		
	
	else if(cmd=="create_file" || cmd=="create_dir" || cmd=="rename")								//command with 2 parameters
	{
		ss>>para1;
		ss>>para2;
		if(cmd=="create_file")
			createfile(para1,para2);

		else if(cmd=="create_dir")
			createdir(para1,para2);

		else if(cmd=="rename")
			renamefile(para1,para2);
	
	}
	else if(cmd=="copy" || cmd=="move" )											//command with multiple parameters
	{
		vector<string> args;		
		while(ss>>para1)			//push all args  in vector
			args.push_back(para1);
		
		para2=args[args.size()-1];		//directory name

		if(cmd=="copy")
		{
			for(int i=0;i<args.size()-1;i++)
				copyfile(args[i],para2);		//copy files one by one
		}
		else if(cmd=="move")
		{
			for(int i=0;i<args.size()-1;i++)
				movefile(args[i],para2);
		}
	}

}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	cout << "\e[8;"<<win_row<<";"<<win_col<<"t";					//set terminal size

											
	
	
	//back_stack.push(path);				
	dir_list = list_files(path);								//stores the list af files and dir in current path
	print_dir(dir_list,st,ed);
	//cout<<dir_list.size();
	cout << "\033[H";					//cursor at position 0		
        
        
    while(1)
    {
    	int key1,key2,key3;
    	key1 = getch();
    
	    if (key1 == 27) 				//arrow key
		{ 								
			//ch=getch(); // skip the [
			//ch=getch();
			//if (key1== 27 && key2 == 91)

			key2 = getch();
		 	key3= getch();
			
	    	if(key3==66)		// down case
			{	
				upkey( st, ed, cursor, dir_list);			//somehow i named functions oppositely :/
			}

			else if(key3==65)		// up case
			{	
				downkey( st, ed, cursor, dir_list);
			}
			else if(key3==68)		// left case
			{	
				leftkey(st, ed, cursor, dir_list, back_stack, fwd_stack, path);
				//cout<<"\033[1D";	//move left
			}
			else if(key3==67)		// right case
			{	
				rightkey(st, ed, cursor, dir_list, back_stack , fwd_stack, path);
				//cout<<"\033[1C";
			}

		}
		else if(key1==105)			//i key acts as up
		{
			downkey( st, ed, cursor, dir_list);
		}
		else if(key1==107)			//k key acts as down
		{	
			upkey( st, ed, cursor, dir_list);		
		}
		else if(key1==10)				//ENTER key
		{
			enterkey(st, ed, cursor, dir_list, back_stack, fwd_stack, path);

		}	
		else if(key1==127)				//BACKSPACE
		{	
			backkey(st, ed, cursor, dir_list,back_stack, fwd_stack, path);
		}
		else if(key1==104)				//home dir 'h'
		{		
			homekey(st, ed, cursor, dir_list, back_stack , fwd_stack, path);
		}


		//////////////////////////////COMMAND MODE///////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		else if(key1==58)				// colon key - command mode
		{
			setcommandmode();
			while(1)
			{
				
				char ckey1=getch();
				if(ckey1==27)			//esc key (since esc has a function we cannot detect arrow keys)
				{	
					//reset command screen//
					cflag=0;
					int last_ind=in_cmd.length();	//input command length
					while(last_ind--)				//remove printed characters
						cout<<"\b \b";
					
					in_cmd="";						//empty command string
					break;
				}
				else if(ckey1==127)		//backspace
				{
					cout<<"\b \b";
					int last_ind=in_cmd.length()-1;
					if(last_ind>=0)
						in_cmd.erase(last_ind,1);
					//cout<<in_cmd;
				}
				else if(ckey1==65 || ckey1==66)
				{
					//do nothing for up and down key
				}
				else if(ckey1==10)		//ENTER key
				{	
					process_command(in_cmd);

					//reset to normal mode
					cursor=0;
					st=0;
					ed=19;
					dir_list = list_files(path);								//stores the list af files and dir in current path
					print_dir(dir_list,st,ed);
					cout << "\033[H";					//cursor at position 0	

					cflag=0;
					break;
					//in_cmd="";
				}
				else
				{
					cout<<ckey1;
					in_cmd.push_back(ckey1);
				}
			}


			if(!cflag)					//esc form command mode
			{
				in_cmd="";
				cout << "\033[H";		//cursor at top
				//cout<<in_cmd;
				continue;
			}

		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////


		else if(key1==113)				// 'q' key - quit
		{
			cout<<"\033c";				//clear outpur screen
			return 0;
		}


    }	  
   return 0;	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
