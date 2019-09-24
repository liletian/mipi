// the code is written by LILETIAN. The code translates the MIPI command into machanine language. In the current version, it supports the read and write command.
// the extend read and write command is not supported in the current version while it can be added.

#define reg1_0 0x00
#define reg1_1 0x01
#define reg2_0 0x02
#define reg2_1 0x03
#define reg2_2 0x04
#define reg2_3 0x05
#define reg2_4 0x06
#define reg2_5 0x07
#define reg3_0 0x08
#define reg3_1 0x09
#define reg4_1 0x0a
#define reg5 0x1c
#define reg6 0x1d
#define reg7 0x1e
#define reg8 0x1f
#define reg9  0x3
#define reg_write 0x2
#define reg_read 0x3
#define MAXLINELENGTH 1000
#define MAXNUMLABELS 10000
#define MAXLABELLENGTH 16


#define ntoi(str)	strtol(str, NULL, 0)

#include <stdio.h>
#include <string.h>
#include <math.h>





unsigned int countSetBits(unsigned int n)
{
  unsigned int count = 0;
  while(n)
  {
    count += n & 1;
    n >>= 1;
  }
  return count;
}
 


char * readAndParse(FILE *inFilePtr, char *lineString,
	char **labelPtr, char **opcodePtr, char **arg0Ptr,
	char **arg1Ptr, char **arg2Ptr)
{
	/* read and parse a line
	note that lineString must point to allocated memory,
		so that *labelPtr, *opcodePtr, and *argXPtr
		won't be pointing to readAndParse's memory
	also note that *labelPtr, *opcodePtr, and *argXPtr
		only point to memory in lineString.
	When lineString changes, so will *labelPtr,
		*opcodePtr, and *argXPtr
	returns NULL if at end-of-file */

	char *statusString, *firsttoken;
	statusString = fgets(lineString, MAXLINELENGTH, inFilePtr);
	if (statusString != NULL) {
		firsttoken = strtok(lineString, " \t\n");
		if (firsttoken == NULL || firsttoken[0] == '#') {
			return readAndParse(inFilePtr, lineString, labelPtr, opcodePtr, arg0Ptr, arg1Ptr, arg2Ptr);
		} else if (firsttoken[strlen(firsttoken) - 1] == ':') {
			*labelPtr = firsttoken;
			*opcodePtr = strtok(NULL, " \t\n");
			firsttoken[strlen(firsttoken) - 1] = '\0';
		} else {
			*labelPtr = NULL;
			*opcodePtr = firsttoken;
		}
		*arg0Ptr = strtok(NULL, ", \t\n");
		*arg1Ptr = strtok(NULL, ", \t\n");
		*arg2Ptr = strtok(NULL, ", \t\n");
	}
	return(statusString);
}




//Parse and update
void printString(char * string) { 
    int index = 0;
    char command[256];
    //printf("Original string size: %zu\n", strlen(string));
    
    while (index < strlen(string) && string[index]!=' ') {
        command[index] = string[index];
        ++index;
    }
	command[index] = '\0';
    printf("Command: %s\n", command);

	if (strcmp(command, "repeat") && strcmp(command, "reg9")) {
		printf("Invalid command: %s\n", command);
		return;
	}

	string[0] = '0';
	if (!strcmp(command, "repeat")) {
		string[1] = '1';
	}
	else string[1] = '0';

    int diff = index-2;
    while (index < strlen(string)) {
        string[index-diff] = string[index];
        ++index;
    }
	string[index-diff] = '\0';
	printf("New string: %s\n", string);
}
	



void main()
{
  //  printf("hello world\n");
  
  //  int a;
  //  a=reg9;

  //    printf("%d\n", reg4_0);

    const double VIO=1.8;
    const double vio_slope=6.5; //ns 

    const int vio_rise_time=1414;  //ns

    const double freq=13e6;  //clock frequency

    char name;
    int  number;

    int sdata_enb_time,clk_time,sdata_time,real_time,time_interval;

    int real_time_new;

    int parity1,parity2;
    

    FILE *f_vio, *f_sdata,*f_sclk,*f_sdata_enb;

    FILE *infile;

    char line[1000];
	
     int tmp_sdata_enb_time, tmp_sclk_time,tmp_sdata_time;

     char *firsttoken;

     char *label, *opcode, *arg0, *arg1, *arg2;
     
     int i,j,reg_addr,data;

     int id_value,tmp_id_value;




    f_vio = fopen("pwlf_vio", "w");

    f_sdata = fopen("pwlf_sdata", "w");


    f_sdata_enb = fopen("pwlf_sdata_enb", "w");




   f_sclk = fopen("pwlf_sclk", "w");

   //file generation for vio
   fprintf(f_vio, "radix 1 \n io i \n vname vio \n slope %f \n vih %f \n tunit ns\n", vio_slope, VIO);    

   fprintf(f_vio, "%d \t %d \n", 0, 0);  

   fprintf(f_vio, "%d \t %d \n", 1414, 1);  


    //file generation for sclk, sdata, sdata_enb

   fprintf(f_sdata, "radix 1 \n io i \n vname sdata \n slope %f \n vih %f \n tunit ns\n", vio_slope, VIO);    

   fprintf(f_sdata, "%d \t %d \n", 0, 0);  


   fprintf(f_sdata_enb, "radix 1 \n io i \n vname sdata_enb \n slope %f \n vih %f \n tunit ns\n", vio_slope, VIO);    

   fprintf(f_sdata_enb, "%d \t %d \n", 0, 1);  


   fprintf(f_sclk, "radix 1 \n io i \n vname sclk \n slope %f \n vih %f \n idelay %f \n tunit ns\n", vio_slope, VIO, 0.0, VIO); //0.0 is the delay    

   fprintf(f_sclk, "%d \t %d \n", 0, 0);  


    sdata_enb_time=0;
    clk_time=0;
    sdata_time=0;
    
    real_time=0;

    infile = fopen ( "test_code.txt" , "r" );
  
    if (infile==NULL) {fputs ("File error\n",stderr); return;}
    while(readAndParse(infile, line, &label, &opcode, &arg0, &arg1, &arg2) != NULL) {
      //      printf("hehe %s\n", line);
      if (!strcmp(line, "repeat")) {
	
	time_interval=ntoi(arg0);
	real_time=real_time+round((time_interval+12*time_interval/1000+14)*1e9/freq/2);
	//	real_time=real_time+round((time_interval)*1e9/freq/2);
	printf("repeat time %d\n",real_time);
	//	fprintf(f_sdata_enb, "%d \t %d \n", real_time, 1);  
	//			num = (ADD << OP_SHIFT) | (reg(arg0) << A_SHIFT) | (reg(arg1) << B_SHIFT) | reg(arg2);
      } 

      else if ((!strcmp(opcode, "reg9") && !strcmp(arg0, "reg_write"))||(!strcmp(opcode, "reg9") && !strcmp(arg0, "reg_read"))) {

	/*
	//write data to sdata_enb
       	  fprintf(f_sdata_enb, "%d \t %d \n", real_time, 0);
	  tmp_sdata_enb_time=real_time+round(25*1e9/freq);
	  fprintf(f_sdata_enb, "%d \t %d \n", tmp_sdata_enb_time, 1);
	*/

	  //write data to sclk
	  if(!strcmp(arg0, "reg_write")){
	    fprintf(f_sdata_enb, "%d \t %d \n", real_time, 0);
	    tmp_sdata_enb_time=real_time+round(25*1e9/freq);
	    fprintf(f_sdata_enb, "%d \t %d \n", tmp_sdata_enb_time, 1);
	  for (i=0;i<23;i++){
	    tmp_sclk_time=real_time+round((i+2.5)*1e9/freq);
	    fprintf(f_sclk, "%d \t %d \n", tmp_sclk_time, 1);
	    tmp_sclk_time=real_time+round((i+3)*1e9/freq);
	    fprintf(f_sclk, "%d \t %d \n", tmp_sclk_time, 0);
	  } 
	  }
	  else{
	    fprintf(f_sdata_enb, "%d \t %d \n", real_time, 0);
	    tmp_sdata_enb_time=real_time+round(16*1e9/freq);
	    fprintf(f_sdata_enb, "%d \t %d \n", tmp_sdata_enb_time, 1);
	    for (i=0;i<14;i++){
	      tmp_sclk_time=real_time+round((i+2.5)*1e9/freq);
	      fprintf(f_sclk, "%d \t %d \n", tmp_sclk_time, 1);
	      tmp_sclk_time=real_time+round((i+3)*1e9/freq);
	      fprintf(f_sclk, "%d \t %d \n", tmp_sclk_time, 0);
	    }
	    for (i=14;i<24;i++){
	      tmp_sclk_time=real_time+round((1*(i-14)+16.5)*1e9/freq);
	      fprintf(f_sclk, "%d \t %d \n", tmp_sclk_time, 1);
	      tmp_sclk_time=real_time+round((1*(i-14)+17)*1e9/freq);
	      fprintf(f_sclk, "%d \t %d \n", tmp_sclk_time, 0);
	    }
	  }
	  

	  //write data to sdata
	  tmp_sdata_time=real_time+round(0.5*1e9/freq);
	  fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);

	  tmp_sdata_time=real_time+round(1.5*1e9/freq);
	  fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	  printf("id %d\n", reg9);

	  //          getchar();

	  /*
	  if(reg9==3){
	    
	  tmp_sdata_time=real_time+round(4.5*1e9/freq);
	  fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);

	  tmp_sdata_time=real_time+round(6.5*1e9/freq);
	  fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	  }
	    else{
	      printf("error id\n");
	      exit(0);
	    }
	  */

	  id_value=reg9;
	  //	  printf("id_value=%d\n",id_value);
	  //	  getchar();
	  if(id_value>7){
	    tmp_sdata_time=real_time+round((2.5)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	    id_value=id_value-16;
	  }
	  else{
	    tmp_sdata_time=real_time+round((2.5)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    id_value=id_value;
	  }


	  //	  id_value=reg9;
	  for(i=3;i>0;i--)
	    {
	  if((id_value>>i-1)==1){
	    tmp_sdata_time=real_time+round((2.5+4-i)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	    //	    tmp_sdata_time=real_time+round((17.5+7-i)*1e9/freq);
	    //    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	  }
	  else{
	    tmp_sdata_time=real_time+round((2.5+4-i)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    //    tmp_sdata_time=real_time+round((17.5+7-i)*1e9/freq);
	    // fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    }
	  id_value=id_value-((id_value >> (i-1))<<(i-1));
	  //	 	  printf("new id_value=%d\n",id_value);
	  // getchar();
	    }




	  

	  if(!strcmp(arg0, "reg_write"))
	    {
	  //	printf("1hehe1 %s\n", opcode);
	  printf("mipi_command %d\n", reg_write);

	  tmp_sdata_time=real_time+round(6.5*1e9/freq);
	  fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);

	  tmp_sdata_time=real_time+round(7.5*1e9/freq);
	  fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	  
	  tmp_sdata_time=real_time+round(8.5*1e9/freq);
	  fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    }

	  else {
	    	  printf("mipi_command %s\n", arg0);

	  tmp_sdata_time=real_time+round(6.5*1e9/freq);
	  fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);

	  tmp_sdata_time=real_time+round(7.5*1e9/freq);
	  fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	  
	  tmp_sdata_time=real_time+round(8.5*1e9/freq);
	  fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	  }
	  



	  //	  reg_addr=ntoi(arg1);
	  
	  //	  printf("register address %d\n", reg_addr);


	  if(!strcmp(arg1, "reg1_0")){
	    reg_addr=0x00;
	    //	    printf("register address %d\n", reg_addr);
	    //    getchar();
	  }
	  else if(!strcmp(arg1, "reg1_1")){
	    reg_addr=0x01;
	    // printf("register address %d\n", reg_addr);
	    // getchar();
	  }
	  else if(!strcmp(arg1, "reg2_0")){
	    reg_addr=0x02;
	    // printf("register address %d\n", reg_addr);
	    // getchar();
	  }
	  else if(!strcmp(arg1, "reg2_1")){
	    reg_addr=0x03;
	    // printf("register address %d\n", reg_addr);
	    // getchar();
	  }
	  else if(!strcmp(arg1, "reg2_2")){
	    reg_addr=0x04;
	    // printf("register address %d\n", reg_addr);
	    // getchar();
	  }
	  else if(!strcmp(arg1, "reg2_3")){
	    reg_addr=0x05;
	    // printf("register address %d\n", reg_addr);
	    // getchar();
	  }
	  else if(!strcmp(arg1, "reg2_4")){
	    reg_addr=0x06;
	    // printf("register address %d\n", reg_addr);
	    // getchar();
	  }
	  else if(!strcmp(arg1, "reg2_5")){
	    reg_addr=0x07;
	    // printf("register address %d\n", reg_addr);
	    // getchar();
	  }
	  else if(!strcmp(arg1, "reg3_0")){
	    reg_addr=0x08;
	    //  printf("register address %d\n", reg_addr);
	    // getchar();
	  }

	  else if(!strcmp(arg1, "reg3_1")){
	    reg_addr=0x09;
	    //  printf("register address %d\n", reg_addr);
	    // getchar();
	  }

	  else if(!strcmp(arg1, "pa_pwr_config1")){
	    reg_addr=0x0a;
	    // printf("register address %d\n", reg_addr);
	    // getchar();
	  }
	  else if(!strcmp(arg1, "temp_sensor")){
	    reg_addr=0x10;
	  }
	  else if(!strcmp(arg1, "reg5")){
	    reg_addr=0x1c;
	    // printf("register address %d\n", reg_addr);
	    // getchar();
	  }
	  else if(!strcmp(arg1, "reg6")){
	    reg_addr=0x1d;
	    // printf("register address %d\n", reg_addr);
	    // getchar();
	  }
	  else if(!strcmp(arg1, "reg7")){
	    reg_addr=0x1e;
	    // printf("register address %d\n", reg_addr);
	    // getchar();
	  }
	  else if(!strcmp(arg1, "reg8")){
	    reg_addr=0x1f;
	    // printf("register address %d\n", reg_addr);
	    // getchar();
	  }
	  else{
	    printf("address error\n");
	    exit(0);
	  }
	  //	  printf("register address %s\n", arg1);
	  //	  printf("old register address %d\n", reg_addr);




	  int reg_addr_old;
	  reg_addr_old=reg_addr;
	  printf("reg_addr %d\n", reg_addr);
	  //	  getchar();
	 
	  if(reg_addr>15){
	    tmp_sdata_time=real_time+round((9.5)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	    reg_addr=reg_addr-16;
	  }
	  else{
	           tmp_sdata_time=real_time+round((9.5)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    reg_addr=reg_addr;
	  }

	  for(i=4;i>0;i--)
	    {
	  if((reg_addr>>i-1)==1){
	    tmp_sdata_time=real_time+round((9.5+5-i)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	    //	    tmp_sdata_time=real_time+round((17.5+7-i)*1e9/freq);
	    //    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	  }
	  else{
	    tmp_sdata_time=real_time+round((9.5+5-i)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    //    tmp_sdata_time=real_time+round((17.5+7-i)*1e9/freq);
	    // fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	  }
	  reg_addr=reg_addr-((reg_addr >> (i-1))<<(i-1));
	    }
	  
	 

	  

	  if(!strcmp(arg0, "reg_write")){
	  if((countSetBits(reg9)+countSetBits(reg_write)+countSetBits(reg_addr_old))%2==1) {
	    parity1=0;
	    tmp_sdata_time=real_time+round(14.5*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    //	    tmp_sdata_time=real_time+round(15.5*1e9/freq);
	    //	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	  }
	    else 
	      {
	    parity1=1;
	    tmp_sdata_time=real_time+round(14.5*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	    //	    tmp_sdata_time=real_time+round(15.5*1e9/freq);
	    //	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	      }
	  }
	  else{
	    	  if((countSetBits(reg9)+countSetBits(reg_read)+countSetBits(reg_addr_old))%2==1) {
	    parity1=0;
	    tmp_sdata_time=real_time+round(14.5*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    //	    tmp_sdata_time=real_time+round(15.5*1e9/freq);
	    //	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	  }
	    else 
	      {
	    parity1=1;
	    tmp_sdata_time=real_time+round(14.5*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	    //	    tmp_sdata_time=real_time+round(15.5*1e9/freq);
	    //	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	      }
	    
	  }
	
	  printf("parity1= %d\n", parity1);


	  data=ntoi(arg2);
	  printf("data %d\n", data);
	  //	  getchar();
	  	  if(ntoi(arg2)>127){
	    tmp_sdata_time=real_time+round((15.5)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	    data=data-128;
	  }
	  else{
	           tmp_sdata_time=real_time+round((15.5)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    data=data;
	  }

	  for(i=7;i>0;i--)
	    {
	      //	      	      printf("data %d\n", data);
	      //   printf("i= %d, data_shift=%d", i, (data>>i-1));
	      //  getchar();
	  if((data>>i-1)==1){
	    tmp_sdata_time=real_time+round((15.5+8-i)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	    //	    tmp_sdata_time=real_time+round((17.5+7-i)*1e9/freq);
	    //    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	  }
	  else{
	    tmp_sdata_time=real_time+round((15.5+8-i)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    //    tmp_sdata_time=real_time+round((17.5+7-i)*1e9/freq);
	    // fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    }
	  data=data-((data >> (i-1))<<(i-1));
	    }
	  /*
	  if(ntoi(arg2)%2==1){
	    tmp_sdata_time=real_time+round((22.5)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	  }
	  else{
	           tmp_sdata_time=real_time+round((22.5)*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	  }
      
	  */
	  if(countSetBits(ntoi(arg2))%2==1){
	    parity2=0;
	    tmp_sdata_time=real_time+round(23.5*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    tmp_sdata_time=real_time+round(24.5*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	  }
	  else
	    {
	    parity2=1;
	    tmp_sdata_time=real_time+round(23.5*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 1);
	    tmp_sdata_time=real_time+round(24.5*1e9/freq);
	    fprintf(f_sdata, "%d \t %d \n", tmp_sdata_time, 0);
	    }
	  //	  printf("old data %s\n",arg2);
	  printf("parity2= %d\n", parity2);


	//	getchar();
	//	num = (reg9 << OP_SHIFT) | (reg(arg0) << A_SHIFT) | (reg(arg1) << B_SHIFT) | reg(arg2);
	  real_time=real_time+round(31*1e9/freq);
      }
      
      else{
	printf("%s\n",line);
	printf("unknown command\n");
	exit(0);
      }
    }

    fclose(infile);

    fclose(f_vio);

    fclose(f_sdata);

    fclose(f_sdata_enb);

    fclose(f_sclk);

  //  return 0;
}
