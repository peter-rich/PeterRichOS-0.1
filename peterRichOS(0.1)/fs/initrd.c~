#include "initrd.h"
#include "memory.h"
#include "monitor.h"
initrd_header_t *initrd_header;     // The header.
initrd_file_header_t *file_headers; // 指向文件们
initrd_file_header_t *file_headers_temp; // 指向临时文件们
fs_node_t *initrd_root;             // root文件夹
fs_node_t *initrd_dev;              // 挂载点文件夹
fs_node_t *root_nodes;              // 文件数组的头地址
int nroot_nodes;                    // 文件个数
int global_location;
struct dirent dirent;

static unsigned int initrd_read(fs_node_t *node, unsigned int offset, unsigned int size, unsigned char *buffer)
{
	initrd_file_header_t header = file_headers[node->inode];
	//找到对应inode号的文件头信息
	if (offset > header.length)
        	return 0;
    	if (offset+size > header.length)
		//读取的位置不能超过文件的范围
        	size = header.length-offset;
    	memcpy(buffer, (unsigned char*) (header.offset+offset), size);
	//header.offset是文件的起始地址
    	return size;
}

static unsigned int initrd_write(fs_node_t *node,unsigned int offset,unsigned int size,unsigned char *buffer)
{
	initrd_file_header_t header = file_headers[node->inode];
	if(offset>header.length)
		return 0;
	if(size>strlen(buffer))
		size=strlen(buffer);
	file_headers[node->inode].length=offset+strlen(buffer);
	root_nodes[node->inode].length=offset+strlen(buffer);
	memcpy((unsigned char*)(header.offset+offset),buffer,size);
	return size;
}

static struct dirent *initrd_readdir(fs_node_t *node, unsigned int index)
{
	if (node == initrd_root && index == 0)
	//创建根节点下/dev目录文件的目录项
	{
		strcpy(dirent.name, "dev");
		dirent.name[3] = 0;
	      	dirent.ino = 0;
	      	return &dirent;
	}

    	if (index-1 >= nroot_nodes)
		//超过了文件数，没有indoe号为index的文件
        	return 0;
    	//新建一个目录项，将找到的文件的信息存进去
	//printf("index_:",0);
	//printf(root_nodes[index-1].name,0);
	//printf("\n",0);
    	strcpy(dirent.name, root_nodes[index-1].name);
    	dirent.name[strlen(root_nodes[index-1].name)] = 0;
    	dirent.ino = root_nodes[index-1].inode;
    	return &dirent;
}

static fs_node_t *initrd_finddir(fs_node_t *node, char *name)
{
	//如果是找/dev的话
    if (node == initrd_root &&
        !strcmp(name, "dev") )
        return initrd_dev;

    int i;
	//找普通文件initrd_header->nfiles
    for (i = 0; i < nroot_nodes; i++)
        if (!strcmp(name, root_nodes[i].name))
            return &root_nodes[i];
    return 0;
}

fs_node_t *initialise_initrd(unsigned int location)
{
	global_location = location;
    /*
	因为在创建的initrd.img文件中，数据是按以下
	方式创建的：
	1.initrd_header_t,有多少文件
	2.initrd_file_header_t,文件的信息文件
	  从这个文件中，可以读出包括文件实际位置在内的信息
	3.文件实在的数据
	所以在一下的初始化中，按照multiboot指出的起始位置,
	可以依次读出相关信息
	*/
    initrd_header = (initrd_header_t *)location;
    file_headers = (initrd_file_header_t *) (location+sizeof(initrd_header_t));

    // Initialise the root directory.
    initrd_root = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    strcpy(initrd_root->name, "initrd");
    initrd_root->mask = initrd_root->uid = initrd_root->gid = initrd_root->inode = initrd_root->length = 0;
    initrd_root->flags = FS_DIRECTORY;
    initrd_root->read = 0;
    initrd_root->write = 0;
    initrd_root->open = 0;
    initrd_root->close = 0;
    initrd_root->readdir = &initrd_readdir;
    initrd_root->finddir = &initrd_finddir;
    initrd_root->ptr = 0;
    initrd_root->impl = 0;

    // Initialise the /dev directory (required!)
    initrd_dev = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    strcpy(initrd_dev->name, "dev");
    initrd_dev->mask = initrd_dev->uid = initrd_dev->gid = initrd_dev->inode = initrd_dev->length = 0;
    initrd_dev->flags = FS_DIRECTORY;
    initrd_dev->read = 0;
    initrd_dev->write = 0;
    initrd_dev->open = 0;
    initrd_dev->close = 0;
    initrd_dev->readdir = &initrd_readdir;
    initrd_dev->finddir = &initrd_finddir;
    initrd_dev->ptr = 0;
    initrd_dev->impl = 0;

    root_nodes = (fs_node_t*)kmalloc(sizeof(fs_node_t) * initrd_header->nfiles);
    nroot_nodes = initrd_header->nfiles;

    // For every file...
    int i;
    for (i = 0; i < initrd_header->nfiles; i++)
    {
        // Edit the file's header - currently it holds the file offset
        // relative to the start of the ramdisk. We want it relative to the start
        // of memory.
        file_headers[i].offset += location;
        // Create a new file node.
        strcpy(root_nodes[i].name, &file_headers[i].name);
        root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
        root_nodes[i].length = file_headers[i].length;
        root_nodes[i].inode = i; 
        root_nodes[i].flags = FS_FILE;
        root_nodes[i].read = &initrd_read;
        root_nodes[i].write = &initrd_write;
        root_nodes[i].readdir = 0;
        root_nodes[i].finddir = 0;
        root_nodes[i].open = 0;
        root_nodes[i].close = 0;
        root_nodes[i].impl = 0;
    }
    return initrd_root;
}

fs_node_t * rename_initrd(char *name,char *new_name)
{
	
	int i = 0;
	int j = 0;
	printf("old_name:",0);
	printf(name,0);
	printf("\n",0);
    	for (i = 0; i < initrd_header->nfiles; i++){
		int judge = 1;
		int k = 0;
		for ( k = 0; k < strlen(name) && k < strlen(root_nodes[i].name) ; k ++){
			if(name[k] != root_nodes[i].name[k]){ 
				judge = 0;
			}
		}
		printf(root_nodes[i].name,0);
    		if(judge){
			printf("\nlength:",0);
			printf("%d",strlen(root_nodes[i].name));
			for(j = 0; j < strlen(root_nodes[i].name) ; j ++){
				root_nodes[i].name[j] = NULL;
				file_headers[i].name[j] = NULL;
			}
			j=0;
			while(new_name[j]!=NULL){
				root_nodes[i].name[j] = new_name[j];
				file_headers[i].name[j] = new_name[j];
				j++;
			}
			root_nodes[i].name[j]=NULL;
			file_headers[i].name[j]=NULL;
		}
    }
	return initrd_root;
}

//add a node 
fs_node_t *add_initrd(char *name)
{
    	int location = global_location;
        initrd_header = (initrd_header_t *)location;
    	file_headers_temp = (initrd_file_header_t *)kmalloc((initrd_header->nfiles)*sizeof(initrd_file_header_t));
	int l=0;
	for (l = 0; l < initrd_header->nfiles; l++)
    	{
        
		file_headers_temp[l].length = file_headers[l].length;
		file_headers_temp[l].offset = file_headers[l].offset;
		file_headers_temp[l].magic = file_headers[l].magic;
		int u=0;
		for(; u < 64; u ++) {
			file_headers_temp[l].name[u] = file_headers[l].name[u];
		}
	}
    
    	file_headers = (initrd_file_header_t *)kmalloc((initrd_header->nfiles+1)*sizeof(initrd_file_header_t));
    
    
    	root_nodes = (fs_node_t*)kmalloc(sizeof(fs_node_t) * (initrd_header->nfiles+1));
    	nroot_nodes = initrd_header->nfiles+1;

    	// For every file...
    	int i;
    	for (i = 0; i < initrd_header->nfiles; i++)
    	{
        
		file_headers[i].length = file_headers_temp[i].length;
		file_headers[i].offset = file_headers_temp[i].offset;
		file_headers[i].magic = file_headers_temp[i].magic;
		int u=0;
		for(; u < 64; u ++) {
			file_headers[i].name[u] = file_headers_temp[i].name[u];
		}
		//printf("My file_name:",0);
		//printf(file_headers[i].name,0);
		//printf("\n",0);
    	    	// Create a new file node.
    	    	strcpy(root_nodes[i].name, &file_headers_temp[i].name);
    	    	root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
        	root_nodes[i].length = file_headers_temp[i].length;
        	root_nodes[i].inode = i;
        	root_nodes[i].flags = FS_FILE;
        	root_nodes[i].read = &initrd_read;
        	root_nodes[i].write = &initrd_write;
        	root_nodes[i].readdir = 0;
        	root_nodes[i].finddir = 0;
        	root_nodes[i].open = 0;
        	root_nodes[i].close = 0;
        	root_nodes[i].impl = 0;
		//printf("file_offset:",0);
		//printf("%d",file_headers[i].offset);
		//printf("\n",0);
    	}	
    
    	root_nodes[initrd_header->nfiles].mask = root_nodes[initrd_header->nfiles].uid = root_nodes[initrd_header->nfiles].gid = 0;
  	root_nodes[initrd_header->nfiles].inode = initrd_header->nfiles;
  	root_nodes[initrd_header->nfiles].flags = FS_FILE;
  	root_nodes[initrd_header->nfiles].read = &initrd_read;
  	root_nodes[initrd_header->nfiles].write = &initrd_write;
    	root_nodes[initrd_header->nfiles].readdir = 0;
    	root_nodes[initrd_header->nfiles].finddir = 0;
    	root_nodes[initrd_header->nfiles].open = 0;
    	root_nodes[initrd_header->nfiles].close = 0;
    	root_nodes[initrd_header->nfiles].impl = 0;
    	root_nodes[initrd_header->nfiles].length = 0;

     	//printf("My this_name:",0);
	//printf(name,0);
	//printf("\n",0);
    	//printf("My file_name:",0);
    	int k = 0;
    	for(; k < strlen(name); k ++){
		file_headers[initrd_header->nfiles].name[k]=name[k];
		root_nodes[initrd_header->nfiles].name[k] = name[k];
    	}
    	printf(file_headers[initrd_header->nfiles].name,0);
    	printf("\n",0);
    	file_headers[initrd_header->nfiles].length = root_nodes[initrd_header->nfiles].length ;
    	file_headers[initrd_header->nfiles].offset = file_headers[initrd_header->nfiles-1].offset + (file_headers[initrd_header->nfiles-1].length+10)*4 ;
    	file_headers[initrd_header->nfiles].magic = initrd_header->nfiles;
	//最后再加一下，以保证正确性
	//printf("node_num:",0);
	char t = 48 + initrd_header->nfiles;
	//printf(&t,0);
	//printf("\n",0);
    	initrd_header->nfiles=initrd_header->nfiles+1;
	//printf("node_num:",0);
	t = 48 + initrd_header->nfiles;
	//printf(&t,0);
	//printf("\n",0);
    	return initrd_root;
}

//add a node 
fs_node_t *delete_initrd(char *name ,int miss)
{
	int l=0;
	for (l = 0; l < initrd_header->nfiles; l++)
    	{
        	if(same(name,file_headers[l].name))
		{
			miss = 0;break;
		}
	}
	if(miss == 0){
		int place = l;
		for (l = place + 1 ; l < initrd_header->nfiles; l++)
	    	{
        
			file_headers[l-1].length = file_headers[l].length;
			file_headers[l-1].offset = file_headers[l].offset;
			file_headers[l-1].magic = file_headers[l].magic;
			int u=0;
			for(; u < 64; u ++) {
				file_headers[l-1].name[u] = file_headers[l].name[u];
			}

	    	    	strcpy(root_nodes[l-1].name, root_nodes[l].name);
	    	    	root_nodes[l-1].mask = root_nodes[l-1].uid = root_nodes[l-1].gid = 0;
	        	root_nodes[l-1].length = file_headers[l-1].length;
	        	root_nodes[l-1].inode = l-1;
	        	root_nodes[l-1].flags = FS_FILE;
	        	root_nodes[l-1].read = &initrd_read;
        		root_nodes[l-1].write = &initrd_write;
        		root_nodes[l-1].readdir = 0;
			root_nodes[l-1].finddir = 0;
			root_nodes[l-1].open = 0;
        		root_nodes[l-1].close = 0;
        		root_nodes[l-1].impl = 0;
	
		}
    
    		nroot_nodes = initrd_header->nfiles-1;
		initrd_header->nfiles=initrd_header->nfiles-1;
	

 	
	}
	else {
		printf("Sorry,there is no file to delete",0);
		printf("\n",0);
	}
    	return initrd_root;
}
