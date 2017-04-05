void printString(char*);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void readString(char*);
int div(int, int);
int mod(int, int);
void readSector(char*, int);
void writeSector(char*, int);
void readFile(char*, char*);
void executeProgram(char*, int);
void terminate();
void deleteFile(char*);
void writeFile(char*,char*,int);

int main()
{
    /*int i=0;
	char buffer1[13312];
	char buffer2[13312];
	buffer2[0]='h';
    buffer2[1]='e';
	buffer2[2]='l';
	buffer2[3]='l';
	buffer2[4]='o';
	for(i=5; i<13312; i++)
	{
		buffer2[i]=0x0;
	}
	makeInterrupt21();
	interrupt(0x21,8, "testW\0", buffer2, 1); //write file testW
	interrupt(0x21,3, "testW\0", buffer1, 0); //read file testW
	interrupt(0x21,0, buffer1, 0, 0); // print out contents of testW*/

	/*makeInterrupt21();
	interrupt(0x21, 4, "tstpr2\0", 0x2000, 0);
	printString("u shouldnt be here\0: \0");
	while(1);*/

	makeInterrupt21();
	interrupt(0x21, 4, "shell\0", 0x2000, 0);
	terminate();
	while(1);

	/*
    char buffer[13312];
	makeInterrupt21();
	interrupt(0x21, 7, "messag\0", 0, 0); //delete messag
	interrupt(0x21, 3, "messag\0", buffer, 0); // try to read messag
	interrupt(0x21, 0, buffer, 0, 0); //print out the contents of buffer
	*/
	
	//TEST CODE FOR writeSector

	/*char buffer[512];
	char map[512];
	readSector(map, 1);
	buffer[0] = 't';
	buffer[1] = 'r';
	writeSector(buffer, 50);
	map[40] = 0xFF;
	writeSector(map, 1);
	readSector(buffer, 50);
	printString(buffer);*/

    
    /*char line[80];
  	makeInterrupt21();
    interrupt(0x21,1,line,0,0);
  	interrupt(0x21,0,line,0,0);*/

    /*char buffer[512];
  	readSector(buffer, 30);
 	printString(buffer);*/

	/*char line[80];
	printString("Enter a line: \0");
	readString(line);
	printString(line);*/
        
	/*int a =0;
	a = mod(18,10);
	if(a == 8)
	interrupt(0x10,0xE*256+'T',0,0,0);
	else
	interrupt(0x10,0xE*256+'F',0,0,0);*/
   
	while(1);
	return 0;
}

int div(int a,int b)
{
	int result = 0;
	while( a >= b)
	{
		a = a-b;
        result++;
    }
   	return result;
}

int mod(int a,int b)
{
   	int d = 0;
   	d = div(a,b);
   	return a - ( b * d) ;
}

void readString(char* line)
{
    int i =0;
   	char in = 0x0;
	
   	while( in != 0xd)
	{
		in = interrupt(0x16,0,0,0,0);
    	if(in == 0x8)
		{
    		if( i > 0)
        	{i--;
        	interrupt(0x10,0xE*256+in,0,0,0);
      		interrupt(0x10,0xE*256+'\0',0,0,0);
      		interrupt(0x10,0xE*256+in,0,0,0);}
    	}else{
			line[i] = in;
			i++;
			interrupt(0x10,0xE*256+in,0,0,0);
    	}
    }
  	line[i] = 0x0;
}

void printString(char* s)
{
	int x = 0;
	while(s[x] != '\0')
	{
      	interrupt(0x10, 0xE*256+s[x], 0, 0, 0);
      	x++;
	}
}

void handleInterrupt21(int ax, char* bx, int cx, int dx)
{
	if( ax == 0)
		printString(bx);
	else if (ax == 1)
		readString(bx);
	else if(ax ==2)
		readSector(bx, cx);
	else if(ax == 3)
		readFile(bx, cx);
	else if(ax == 4)
		executeProgram(bx,cx);
	else if(ax == 5)
		terminate();
	else if(ax == 6)
		writeSector(bx, cx);
	else if(ax == 7)
		deleteFile(bx);
	else if(ax == 8)
		writeFile(bx,cx,dx); 
    else
       	printString("error \0");
}

void readSector(char* buffer,int sector)
{
    int relative_sector = 0; 
    int head = 0;
    int track = 0; 
    relative_sector = mod(sector,18) + 1;
    head = mod(div(sector,18),2);
    track = div(sector,36);
    interrupt(0x13, 2*256+1, buffer, track*256+relative_sector, head*256+0);
}

void writeSector(char* buffer,int sector)
{
    int relative_sector = 0; 
    int head = 0;
    int track = 0;
    relative_sector = mod(sector,18) + 1;
    head = mod(div(sector,18),2);
    track = div(sector,36);
    interrupt(0x13, 3*256+1, buffer, track*256+relative_sector, head*256+0);
}


void readFile(char* fileName, char* buffer)
{
    int i = 0;
    int j = 0;
    int k = 0;
    char dir[512];
    int add = 0;
    int start = 0;
	char not[11];
    readSector(dir,2);

    i = 0;
    /*In case of error check for zero padding in fileName*/
    while(i < 512)
    {
        j = 0;
        while(j < 6)
        {
            if(dir[i+j] != fileName[j])
            {
                break;
            }
            j++;
        }
                 
        if(  j == 6)
        {

                start = i+6;
		for(i=0;i<26;i++){
			readSector(buffer+add,dir[start+i]);
			add += 512;
		}
        }

        i += 32;
    }
	if(i==512)
	{
		//printString("\nfile not found!\n\0");
		not[0]=' ';
		not[1]='n';
		not[2]='o';
		not[3]='t';
		not[4]='f';
		not[5]='o';
		not[6]='u';
		not[7]='n';
		not[8]='d';
		not[9]='\n';
		not[10]=0;
		printString(not);
		//interrupt(0x10,0xE*256+'\n',0,0,0);
		terminate();
	}  
}

void executeProgram(char* name, int segment)
{
	int i = 0;
	char buffer[13312];
	readFile(name,buffer);

	while(i<13312)
	{
		putInMemory(segment,i,buffer[i]);
		i++;
	}

	launchProgram(segment);
}

void terminate()
{
	char shell[6];
    shell[0] = 's';
    shell[1] = 'h';
    shell[2] = 'e';
    shell[3] = 'l';
    shell[4] = 'l';
    shell[5] = 0x0;
    interrupt(0x21,4,shell,0x2000,0);
}

void deleteFile(char* name)
{
	int i;
	int j;
	char sector;
	char dir[512];
	char map[512];
    char not [11];
	readSector(map, 1);
	readSector(dir, 2);

	for(i = 0 ; i < 512 ; i++)
	{
		for(j = 0 ; j < 6 ; j++)
		{
			if (dir[i+j] != name[j])
		 	{
				 break;
			}
		}

		if (j == 6)
		{ //file found
			dir[i] = 0x00;
		 	//interrupt(0x10,0xE*256+dir[i],0,0,0);
		 	break;
		}else{
		 	i+=31;
		}
	}
    
	if(i==512)
	{
		//printString("\nfile not found!\n\0");
		not[0]=' ';
		not[1]='n';
		not[2]='o';
		not[3]='t';
		not[4]='f';
		not[5]='o';
		not[6]='u';
		not[7]='n';
		not[8]='d';
		not[9]='\n';
		not[10]=0;
		printString(not);
        //interrupt(0x10,0xE*256+'\n',0,0,0);
		terminate();
	}else{
		i += 6;
		for(j = 0 ; j < 26 && dir[i + j] != 0x00 ; j++)
		{
			sector = dir[i + j];
			map[sector + 1] = 0x00;
		}

		writeSector(map, 1);
		writeSector(dir, 2);
    }
}

void writeFile(char* name, char* buffer, int secNum)
{
	int i = 0;
	int j = 0;
    int k = 0;
    int entry_name = 0;
    int entry_sectors = 0;
    int sectors_num = 0;
    char error [5]; 
    char map[512];
    char dir[512];
    char tmp [512];
    error[0] = 'E';
    error[1] = 'R';
    error[2] = 'R';
    error[3] = '\n';
    error[4] = 0;          

    readSector(map,1);    // reading both dir and map sectors
    readSector(dir,2);  
          
    for (i=0; i<512; i=i+32) // finding an empty entry in dir
	{
		if (dir[i]==0)
		{
			break;
		}
	}
	if (i==512)
	{
		printString(error);
		terminate();
	}
	
	entry_name = i;
    entry_sectors = i+6; 
    for( i =0 ; i < 32; i++) 
    {
		dir[entry_name+i] = 0x00;
	}

	for( j =0; j < secNum; j++)              // finding empty sectors
	{
		if (sectors_num==26)
		{
			printString(error);
			terminate();
		}
                 
		for (i=0; i<512; i++)   // checking if there still emprt sectors in map
		{
			if (map[i]==0)
			{
				break;
			}
		}
		
		if (i== 512)
		{
			printString(error);
			terminate();
		}
		for( k = j*512 ; k < (j*512) + 512 ; k++) // patrtition of data from buffer
        {
			tmp[k-j*512] = buffer[k];
		}
		
		writeSector(tmp,i);
		map[i]=0xFF;
		dir[entry_sectors]=i;
		sectors_num++;
		entry_sectors++;
	}
                                       
 	for (i=0; i<6; i++)   	//putting the name
	{
		if( name[i]==0)
		{
			break;
		}
		dir[entry_name+i]= name[i];
	}

    writeSector(map,1);
    writeSector(dir,2); 
}