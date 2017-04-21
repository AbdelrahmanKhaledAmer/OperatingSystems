void printString(char*);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void readString(char*);
int div(int, int);
int mod(int, int);
void readSector(char*, int);
void writeSector(char*, int);
void readFile(char*, char*);
void executeProgram(char*);
void terminate();
void deleteFile(char*);
void writeFile(char*,char*,int);
void handleTimerInterrupt(int , int);
int active[8];
int stack_pointers[8];
int currentProcess;
int handleTimerInterrupt_counter;
int main()
{
        int i = 0;
    for(i=0;i<8;i++){

		active[i]=0;
		stack_pointers[i]= 0xff00;
	}
    currentProcess =0;
    handleTimerInterrupt_counter=1;
	makeInterrupt21();
	makeTimerInterrupt();
    interrupt(0x21, 4, "shell\0", 0x2000, 0);

	//interrupt(0x21, 4, "hello1\0", 0, 0);
	//interrupt(0x21, 4, "hello2\0", 0, 0);
	terminate();
	while(1);



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

void executeProgram(char* name)
{
	int i = 0;
        int index = 0;
        int segment = 0;
	char buffer[13312];
	readFile(name,buffer);
    setKernelDataSegment();
	for(index = 0; index < 8; index++)
   		if(active[index] == 0){
   	              active[index] = 1;
                      break;

             }

	segment = (index* 0x1000) + 0x2000;
	restoreDataSegment();
        if(index == 8){
          printString("no available space in the process table\0");
          while(1);
        }
	 while(i<13312)
	{
		putInMemory(segment,i,buffer[i]);
		i++;
	}

	initializeProgram(segment);
}

void terminate()
{
	 setKernelDataSegment();

	active[currentProcess-1]=0;
	while(1);
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

void handleTimerInterrupt(int segment, int sp){

    int nextProcess = currentProcess+1;
	  handleTimerInterrupt_counter++;
	  if(handleTimerInterrupt_counter>100)
		 {
		     if(nextProcess>8)
			      nextProcess=1;
		     while(nextProcess!=currentProcess)
		       {


			          if(active[nextProcess-1]==1){
				              stack_pointers[currentProcess-1] = sp;
				              segment = ((nextProcess-1)+2)* 0x1000;
				              sp = stack_pointers[nextProcess-1];
				              currentProcess = nextProcess;
                      break;
			            }
			           nextProcess++;
			           if(nextProcess>8)
				             nextProcess=1;
		         }

		       handleTimerInterrupt_counter=1;
	    }

	returnFromTimer(segment,sp);
}
