#include "common.h"
#include "memory_manager.h"
#include "virtual_memory.h"
#include "memory_pool.h"
#include "fs.h"
#include "initrd.h"
#include "sched.h"
#include "syscall.h"
#include "keyboard.h"
#include "monitor.h"
int haha=0;
extern fs_node_t *fs_root; // The root of the filesystem.
static void keyboard_handler();//键盘处理函数
static void exe_cmd(char *todo);//执行命令行函数
static char keytable[0x55] = {0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08, '\t','Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',   0,   'A', 'S','D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', 0,0,   '\\', 'Z', 'X', 'C', 'V','B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0, 0,0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1','2', '3', '0', '.',8};
static int caps;//0时为小写字母
//用于存储命令行
static char cmd[64];
//专门用于存储读出的信息
static char message[256];
//记录cmd的长度
static int cmd_len;
static void init_cmd();
//////////******命令程序*********//////////
static void show_memory();
static void help();
static void show_clean();
static void show_pool();
static void read_only(char *todo, int print);
static void read_file(char *todo, int print);
static void show_copy(char *todo);
static void list_file();
static void show_file();
static void make_file();
static void write_only(char *todo, int print, char* contents);
static void write_file(char *todo, int print);
static void rename_file(char *todo);
static void show_task();
static void make_memory(char *todo);	
static void show_delete(char *todo);
//*********************************************************
//*********************************************************
//*****************************************多任务的展示函数
void stest(){
	
	unsigned int i=0;
	while(1)
	{
	i++;
	if(haha==0)
	{
		printf("p1:",2);
		printf("a\n",2);
		haha=1;
	}
	if(i==0x1000000)
		break;
	}
	return ;
}

void stest2(){
	unsigned int i=0;
	while(1){
		i++;
		if(haha==1){
			printf("p2:",0);
			printf("c\n",0);
			haha=0;
		}
		if(i==0x1000000)
			break;
	}
	return ;
}
/////////////////////////////////////
//键盘初始化,将键盘处理函数注册到
//IRQ1,33号中断

void init_cmd(){
	int k = 0; 
	for(; k<64; k++){
		cmd[k] = NULL;	
	}
}

void init_keyboard(){
	printf("root-> $",0);
	init_cmd();
	register_int_handler(33,keyboard_handler);
	caps=0;
	cmd_len=0;
}

void keyboard_handler(){
	unsigned char ret;
	ret = inb(0x60);//读入键盘输入
	if(ret&0x80){//松开按键时的编码
		if(ret==0xba){//caps lock键
			caps^=1;
		}
	}
	else{/*按下按键时的编码*/
		if(caps==0&&keytable[ret]>=65&&keytable[ret]<=90){
			monitor_put(keytable[ret]+32);
		}
		else{
			/*if(keytable[ret]==0x08){
				cmd[cmd_len]=NULL;		
			}*/
			monitor_put(keytable[ret]);
		}
		//执行命令行
		if(keytable[ret]!='\n')
			cmd[cmd_len++]=keytable[ret];
		else{
			exe_cmd(cmd);
			printf("\n",0);
			cmd_len=0;
			cmd[cmd_len]=' ';
			printf("root-> $",0);
			init_cmd();
		}
	}

}

void exe_cmd(char *todo){
	if(todo[0]=='L'&&todo[1]=='S'){//ls
		list_file();
	}
	else if(todo[0]=='M'&&todo[1]=='E'&&todo[2]=='M'&&todo[3]=='O'&&todo[4]=='R'&&todo[5]=='Y'){//memory
		show_memory();	
	}
	else if(todo[0]=='H'&&todo[1]=='E'&&todo[2]=='L'&&todo[3]=='P'){//help
		help();
	}
	else if(todo[0]=='P'&&todo[1]=='O'&&todo[2]=='O'&&todo[3]=='L'){//pool
		show_pool();	
	}
	else if(todo[0]=='L'&&todo[1]=='S'){//ls
		list_file();	
	}
	else if(todo[0]=='V'&&todo[1]=='I'&&todo[2]=='M'){//vim
		write_file(todo,1);
	}
	else if(todo[0]=='R'&&todo[1]=='U'&&todo[2]=='N'){//task
		show_task();
	}
	else if(todo[0]=='C'&&todo[1]=='A'&&todo[2]=='T'){//cat
		read_file(todo,1);
	}
	else if(todo[0]=='C'&&todo[1]=='L'&&todo[2]=='E'&&todo[3]=='A'&&todo[4]=='N'){//clean
		show_clean();
	}
	else if(todo[0]=='C'&&todo[1]=='P'){//cp
		show_copy(todo);	
	}
	else if(todo[0]=='R'&&todo[1]=='M'){//rm
		show_delete(todo);	
	}
	else if(todo[0]=='T'&&todo[1]=='O'&&todo[2]=='U'&&todo[3]=='C'&&todo[4]=='H'){//touch
		make_memory(todo);	
	}
	else if(todo[0]=='R'&&todo[1]=='E'&&todo[2]=='N'&&todo[3]=='A'&&todo[4]=='M'&&todo[5]=='E'){
		rename_file(todo);
	}
	else{
		printf("undefinded command!\n",0);
		printf("please cin 'help' for detailing!\n",0);	
	}
	memset(todo,NULL,64);
	
}

void show_clean(){
	monitor_clear();
	printf("PeterRichOS !\n",0);
	printf("CopyRight (R) 2016 \n",0);
}

void show_memory(){
	printf("\n--------------memory-------------------\n",0);
	printf("code start: ",0);
	printf("%h",kern_start);
	printf("\n",0);
	printf("code end:   ",0);
	printf("%h",kern_end);
	printf("\n",0);
	printf("code's memory use(KB):  ",0);
	printf("%h",(kern_end-kern_start+1023)/1024);
	printf("\n",0);
}

void help(){
	printf("---------------command list------------------\n",0);
	printf("memory:   show memory use\n",0);
	printf("pool  :   show malloc and free\n",0);
	printf("run  :   show multi-process, print 'a' and 'c' at the same time\n",0);
	printf("ls    :   list all file \n",0);
	printf("cat (filename)  :   read file write\n",0);
	printf("vim (filename) (contents)  :   write file\n",0);
	printf("clean :   show the screen after clean\n",0);
	printf("rename old_file new_file:   show the screen after clean\n\n",0);
	printf("touch (filename)  :   create new file",0);
	printf("cp (oldfile) (newfile):  copy file\n",0);
	printf("rm (filename)  :   delete file\n",0);
	
	printf("\n",0);
}

void show_pool(){
	printf("\n----------------memory pool-----------------------\n",0);
	pool_test();
	printf("\n",0);
}

void list_file(){
	int i = 0;
    	struct dirent *node = 0;
//	printf("%h",(unsigned int)glb_mboot_ptr->mods_count);
//	printf("loaded\n",0);
    	while ( (node = readdir_fs(fs_root, i)) != 0)
	//读出根目录下的所有文件，从0号开始
    	{
        	printf(node->name,0);
        	fs_node_t *fsnode = finddir_fs(fs_root, node->name);

        	if ((fsnode->flags&0x7) == FS_DIRECTORY)
        	{
            		printf("/",0);
        	}
            	printf("\n",0);
        	i++;
    	}

}
void read_only(char *todo, int print) {

	int miss = 1;

	int i = 0;
    	struct dirent *node = 0;
	
	int j = 0;
    	while ( (node = readdir_fs(fs_root, i)) != 0)//读出根目录下的所有文件，从0号开始
    	{
		miss = 0;
		int k=1;
		for(j = 0 ; j < strlen(node->name) && j < strlen(todo); j++) {
			if( node->name[j] != '.' && node->name[j] != (todo[j])) {
				k = 0;break;			
			}		
		}
		if( k==1) {
        		fs_node_t *fsnode = finddir_fs(fs_root, node->name);

            		char buf[256];
			int tt = 0;
			memset(buf,NULL,256);
            		unsigned int sz = read_fs(fsnode, 0, 256, buf);
			//printf("buf:",0);
			//printf(buf,0);
			//printf("\n",0);
			if (print == 1) {
		       		printf(node->name,0);
				printf("\n",0);
				printf("read size:  ",0);
				printf("%d",sz);
				printf("\n",0);
				printf("\ncontents:  ",0);
	            		int j;
			
	            		for (j = 0; j < sz+15; j++)
	               			monitor_put(buf[j]);
           
	       			printf("\n",0);					
			}
			else {
				int j = 0;
				memset(message,NULL,256);
				strcpy(message,buf);
			}

		}
		i++;
    	}
	if(miss) {
		printf("Sorry,there is no file ->",0);
		printf(todo,0);
		printf("\n",0);
 	}
}

void read_file(char *todo, int print) {
	char read_name[15];
	memset(read_name,NULL,15);
	int i = 0;
	while(todo[i] != ' ' ) {
		i=i+1;
	}
	i = i+1;
	int j = i;
	while(todo[i] != NULL ) {
		if('.'==todo[i]){
			read_name[i-j]=todo[i];
		}
		else{
			read_name[i-j]=todo[i]+32;			
		}
		
		i=i+1;
	}
	//printf("name:",0);
	//printf(read_name,0);
	//printf("\n",0);
	read_only(read_name, 1);
}

void write_only(char *todo, int print, char* contents) {
	printf("todo_name:",0);
	printf(todo,0);
	printf("\n",0);
	struct dirent *node = 0;
	int i = 0;
	int j = 0;
	int miss = 1;
	while ( (node = readdir_fs(fs_root, i)) != 0)//读出根目录下的所有文件，从0号开始
    	{
		miss = 0;
		int k = 1;
		for(j = 0 ; j < strlen(node->name) && j < strlen(todo); j++){
			if(node->name[j]!='.' && node->name[j] != (todo[j])){
				k = 0;break;			
			}		
		}
		if( k == 1){

        		fs_node_t *fsnode = finddir_fs(fs_root, node->name);

            		unsigned int wz;
            		char buf[256];
			//在每次写之前先刷新一下buf，以免留下从前的文字！
			int tt = 0;
			
			if(print == 1) {
	        		printf(node->name,0);
				printf("\n",0);

				printf("old_buf:",0);
				printf(buf,0);
				printf("\n",0);

			}
	
			memset(buf,NULL,256);

			tt=0;
			for(;tt < strlen(contents);tt++){
				buf[tt] = contents[tt];
			}
			wz = write_fs(fsnode, 0, 256, buf);

			if(print == 1) {
				printf("buf:",0);
				printf(buf,0);
				printf("\n",0);
            		
				printf("write size:  ",0);
				printf("%d",wz);
				printf("\n",0);
            		
				printf("\ncontents:  ",0);
				int j;
	            		for (j = 0; j < wz+15; j++)
	               			monitor_put(buf[j]);
           
	       			printf("\n",0);	
			}	

		}
		i++;
    	}
	if(miss) {
		printf("Sorry,there is no file ->",0);
		printf(todo,0);
		printf("\n",0);
 	}
}

void write_file(char *todo, int print) {

	
    	
	char write_name[15];
	memset(write_name,NULL,15);
	char contents[256];
	memset(contents,NULL,256);
	int p = 0;
	for( ;p< 20;p++){
		if(todo[p]==' '){break;}		
	}
	p = p+1;
	int q = p;
	while(todo[p]!=' '){
		if('.'==todo[p]){
			write_name[p-q]=todo[p];
		}
		else{
			write_name[p-q]=todo[p]+32;			
		}
		p=p+1;
	}
	p = p+1;
	q = p;
	while(todo[p] != NULL){
		if('.'==todo[p]){
			contents[p-q]=todo[p];
		}
		else{
			contents[p-q]=todo[p]+32;			
		}
		p=p+1;
	}	

	write_only(write_name, 1, contents);
}

void rename_file(char *todo)
{
	char new_name[15];
	char name[15];
	
	int i=0;
	memset(name,NULL,15);
	memset(name,NULL,15);
	i=0;
	while(todo[i] != ' ' ) {
		i=i+1;
	}
	i=i+1;
	int j=i;
	while(todo[i] != ' ' ) {
		if('.'==todo[i]){
			name[i-j]=todo[i];
		}
		else{
			name[i-j]=todo[i]+32;			
		}
		
		i=i+1;
	}
	i=i+1;
	j=i;
	while(todo[i] != NULL ) {
		if('.'==todo[i]){
			new_name[i-j]=todo[i];
		}
		else{
			new_name[i-j]=todo[i]+32;		
		}
		
		i=i+1;
	}

	fs_root = rename_initrd(name , new_name);
}

void show_copy(char *todo) // function for cp
{
	char name[15];
	char new_name[15];
	memset(name,NULL,15);
	memset(new_name,NULL,15);
	int i=0;
	while(todo[i] != ' ' ) {
		i=i+1;
	}
	i=i+1;
	int j=i;
	while( todo[i] != ' ' ) {
		if('.'==todo[i]){
			name[i-j]=todo[i];
		}
		else{
			name[i-j]=todo[i]+32;			
		}
		
		i=i+1;
	}
	i=i+1;
	j=i;
	while(todo[i] != NULL) {
		if('.'==todo[i]){
			new_name[i-j]=todo[i];
		}
		else{
			new_name[i-j]=todo[i]+32;			
		}
		
		i=i+1;
	}

	fs_root = add_initrd(new_name);
	read_only(name,0);
	printf("in_message:",0);
	printf(message,0);
	printf("\n",0);
	write_only(new_name,0,message);
}

void make_memory(char *todo)//touch
{
	char name[10];
	int i=0;
	memset(name,NULL,10);
	while(todo[i] != ' ' ) {
		i=i+1;
	}
	i=i+1;
	int j=i;
	while(todo[i] != NULL ) {
		if('.'==todo[i]){
			name[i-j]=todo[i];
		}
		else{
			name[i-j]=todo[i]+32;			
		}
		
		i=i+1;
	}
	
	fs_root = add_initrd(name);
}

void show_delete(char *todo)
{
	int miss = 1;
	char delete_name[15];
	memset(delete_name,NULL,15);
	int i = 0;
	while(todo[i] != ' ' ) {
		i=i+1;
	}
	i = i+1;
	int j = i;
	while(todo[i] != NULL ) {
		if('.'==todo[i]){
			delete_name[i-j]=todo[i];
		}
		else{
			delete_name[i-j]=todo[i]+32;			
		}
		
		i=i+1;
	}
	fs_root = delete_initrd(delete_name,miss);

	if(miss) {
		printf("Sorry,there is no file ->",0);
		printf(todo,0);
		printf("\n",0);
 	}
}

//同时读写文件的模板mod
void show_file(){
	printf("\n----------------read file------------------------\n",0);
	int i = 0;
    	struct dirent *node = 0;
//	printf("%h",(unsigned int)glb_mboot_ptr->mods_count);
//	printf("loaded\n",0);
    	while ( (node = readdir_fs(fs_root, i)) != 0)
	//读出根目录下的所有文件，从0号开始
    	{
        	printf("file:  ",0);
        	printf(node->name,0);
        	fs_node_t *fsnode = finddir_fs(fs_root, node->name);

        	if ((fsnode->flags&0x7) == FS_DIRECTORY)
        	{
            		printf("\n\t(directory)\n",0);
        	}
        	else
        	{
            		printf("\n\t contents:  ",0);
            		char buf[256];
			char *change="We Can do it!";
			unsigned int zs = write_fs(fsnode,0,256,change);
            		unsigned int sz = read_fs(fsnode, 0, 256, buf);
			printf("write size:  ",0);
			printf("%d",zs);
			printf("read size:  ",0);
			printf("%d",sz);
			printf("\n",0);
            		int j;
            		for (j = 0; j < sz+15; j++)
                		monitor_put(buf[j]);
            
            		printf("\n",0);
        	}
        	i++;
    	}

}

void show_task(){
	printf("---------------kernel process test-------------------\n",0);
	init_timer(200);
	asm volatile ("sti");
	kernel_thread(stest);
	kernel_thread(stest2);

}
