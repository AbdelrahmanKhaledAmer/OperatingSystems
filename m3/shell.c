int validCommand(char*, char*);
int div(int, int);
int mod(int, int);




int main()
{
    while(1)
    {
        char buffer[13312];                //Buffer to hold file
        char parameter[7];                 //Buffer to hold fileName
        char parameter2[7];
        char line[80];                     //Buffer to hold command entered

        // next 2 attributes for dir listing command

        char file_name[7];                 // file name
        int file_Sector_Count=0;           // number of sector for each file
        char file_Sector_Count_print[3];   // buffer to print the file_Sector_Count
        char input_file[100];            // buffer for the created file
        int kill_value;
        int i = 0;
        int j = 0;

        while(i < 80)
        {
            line[i++] = 0x0;
        }
          i = 0;
        interrupt(0x21,0,"SHELL>\0",0,0);  //Display prompt
        interrupt(0x21,1,line,0,0);        //Take Input
        interrupt(0x21, 0,"\n\0", 0, 0);
        //parameter[0]=0x0;
        while(i < 7)
        {
            parameter[i++] = 0x0;
        }
        i = 0;
        while(i < 7)
        {
            parameter2[i++] = 0x0;
        }
        i = 0;

        while(i < 13312)
        {
            buffer[i++] = 0x0;
        }
        i = 0;
        //////////////////////////////////////////////////
        //check if len(parameter)>6 then invalid command//
        //////////////////////////////////////////////////
        if(validCommand("view \0",line))
        {//View file

        // handle infinite loop when typing "view \0"
            if(line[6]==0){
                 interrupt(0x21,0," not found!\n",0,0);
                 continue;
            }
            while(line[i+5] != 0xD)
            {
                parameter[i] = line[i+5];
                i++;
            }

            if( i > 6)
            {
                interrupt(0x21,0," SHELL> file name too large\0",0,0);
                interrupt(0x21, 0,"\n\0", 0, 0);
            }else{
                interrupt(0x21, 3, parameter, buffer, 0);
                interrupt(0x21, 0, buffer, 0, 0);
            }
        }else if(validCommand("execute \0",line)){//Execute program

            if(line[9]==0){
                 interrupt(0x21,0," bad command!\n",0,0);
                 continue;
            }

            while(line[i+8] != 0xD)
            {
                parameter[i] = line[i+8];
                i++;
            }
            if( i > 6)
            {
                interrupt(0x21,0," SHELL> file name too large\n\0",0,0);
                interrupt(0x21, 0,"\n\0", 0, 0);
            }else{
                interrupt(0x21, 4, parameter, 0x2000, 0);
            }
        }else if(validCommand("delete \0",line)){//Delete file
             if(line[8]==0){
                 interrupt(0x21,0," bad command!\n",0,0);
                 continue;
            }
            while(line[i+7] != 0xD)
            {
                parameter[i] = line[i+7];
                i++;
            }
            if(i > 6)
            {
                interrupt(0x21,0,"SHELL> file name too large\n\0",0,0);
                interrupt(0x21, 0,"\n\0", 0, 0);
            }else{
                interrupt(0x21, 7, parameter, 0, 0);
            }
        }else if(validCommand("copy \0",line)){//Copy file
             if(line[6]==0){
                 interrupt(0x21,0," bad command!\n",0,0);
                 continue;
            }
            while(line[i+5] != 0x20) //ascii of space
            {
                parameter[i] = line[i+5];
                i++;
            }
            if(i > 6)
            {
                interrupt(0x21,0,"SHELL> First file name too large\n\0",0,0);
                interrupt(0x21, 0,"\n\0", 0, 0);
            }else{
                i++;
                while(line[i+5] != 0xD)
                {
                    parameter2[j] = line[i+5];
                    i++;
                    j++;
                }
                if(j > 6)
                {
                    interrupt(0x21,0,"SHELL> Second file name too large\n\0",0,0);
                    interrupt(0x21, 0,"\n\0", 0, 0);
                }else{
                    interrupt(0x21, 3, parameter, buffer, 0);
                    i = 0;
                    j = 0;
                    while(buffer[i] != 0)
                    {
                        i++;
                    }
                    j=div(i,512);
                    i=mod(i,512);
                    if(i!=0)
                    j++;
                    interrupt(0x21, 8, parameter2, buffer, j);
                }
            }

        }else if(validCommand("dir\0",line)){//Show all files
            interrupt(0x21, 2, buffer, 2, 0);
            //Handle the buffer
            i=0;
            while(i<512){
             if(buffer[i]!=0)
                {
                    //reset file_name buffer
                    j=0;
                    while(j<7){
                        file_name[j++]=0;
                    }
                    //

                    //copy the name of the file to the file_name buffer
                    j=0;
                    while(j<6){
                        file_name[j]=buffer[i+j];
                        j++;
                    }
                    //reset file_Sector_Count of the file
                    file_Sector_Count=0;
                    //
                    // compute the new file_Sector_Count
                    while(j<32){
                        if(buffer[i+j]==0){
                            //handle the >10 problem
                            if(file_Sector_Count>=10){
                            int tenth=div(file_Sector_Count,10);
                            int rem=mod(file_Sector_Count,10);
                            file_Sector_Count_print[0]='0'+tenth;
                            file_Sector_Count_print[1]='0'+rem;
                            file_Sector_Count_print[2]='\0';
                            }
                            else{
                            file_Sector_Count_print[0]='0'+file_Sector_Count;
                            file_Sector_Count_print[1]='\0';
                             file_Sector_Count_print[2]='\0';
                            }
                            break;
                            }
                        file_Sector_Count++;
                        j++;
                    }//
                    //Display output
                    interrupt(0x21,0,"\n\0",0,0);
                    interrupt(0x21,0,file_name,0,0);
                    interrupt(0x21,0,"  \0",0,0);
                    interrupt(0x21,0,file_Sector_Count_print,0,0);
                    interrupt(0x21,0," \n\0",0,0);

                }
                i+=32;
            }
        }else if(validCommand("create \0",line)){
            if(line[8]==0){
                interrupt(0x21,0," bad command!\n\0",0,0);
                continue;
            }
            while(line[i+7] != 0xD)
            {
                parameter[i] = line[i+7];
                i++;
            }
            if( i > 6)
            {
                interrupt(0x21,0," SHELL> file name too large\n\0",0,0);
                interrupt(0x21, 0,"\n\0", 0, 0);
            }else{
                j = 0;

                while(1)
                {
                    //read the input to input_file buffer it then store
                    i=0;
                    while(i<100)input_file[i++]=0;
                    interrupt(0x21,0,"\nin>\0",0,0);
                    interrupt(0x21,0,"\n\0",0,0);
                    interrupt(0x21,1,input_file,0,0);
                    i = 0;
                    while(input_file[i] != 0xD)
                    {
                        buffer[j++] = input_file[i];
                        i++;
                    }
                    if(i==0)
                    {
                        break;
                    }
                    buffer[j++] = '\n';
                }
                buffer[j] = 0x0;
                interrupt(0x21, 0,"\n\0", 0, 0);
                i = 0;
                j = 0;
                while(buffer[i] != 0)
                {
                    i++;
                }
                j=div(i,512);
                i=mod(i,512);
                if(i!=0)
                    j++;
                interrupt(0x21, 8, parameter, buffer, j);
            }
        }else if(validCommand("kill \0",line)){//Kill process
            //handle numbers larger than 8 TODO
            kill_value=line[5]-'0';
            if(kill_value > 0 && kill_value < 9){
                interrupt(0x21, 9, parameter, line[5], kill_value);
            }else{
                interrupt(0x21,0,"You must enter a number between 1 and 8!\n",0,0);
            }
        }else{//Unknown command
            interrupt(0x21,0,"Bad Command!\n",0,0);
        }
    }
    return 0;
}

int validCommand(char* string1, char* string2)
{
    int i = 0;
    int length = 0;

    while(string1[i] != 0)
    {
        length++;
        i++;
    }

    i = 0;
    while(i < length)
    {
        if(string1[i] != string2[i])
        {
            return 0;
        }
        i++;
    }
    return 1;
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