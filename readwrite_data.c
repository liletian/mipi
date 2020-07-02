#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *my_itoa(int num, char *str)
{
        if(str == NULL)
        {
                return NULL;
        }
        sprintf(str, "%d", num);
        return str;
}

int main()
{
   int num1=0;
   FILE *fp,*fp_w,*fp_final;
   char c;

   char tmp[256];

   int i;

   //   fp_w = fopen("read_data_sample1.ocn", "w"); // "w" defines "writing mode"
   
   //  char s[256]="read_data_sample";
  

   //   fscanf(fptr,"%d", &num);

	 if ((fp_final = fopen("final_data.ocn","w")) == NULL){
	   printf("Error! opening sampel file");
	   // Program exits if the file pointer returns NULL.
	   exit(1);
	 }

   for( i=1+31;i<28+31;i++){
     //     fp_w = fopen("read_data_sample1.ocn", "w"); // "w" defines "writing mode"


     	   char s[256]="data";
	   my_itoa(i,tmp);
	   strcat(s,tmp);
	   strcat(s,".ocn");
	   printf("%s\n",s);
	   //	   getchar();

	   

	   char s_file[2560]="openResults( \"/scratch/noclean/wsd/cmos_soi_projects/bl888078/simulation/tmppp_4HL6A_2_gpiopeak/spectre/schematic/distributed/job0";
	   strcat(s_file,tmp);
	   strcat(s_file,"/psf\")\n");
	   

	 if ((fp = fopen("sample.ocn","r")) == NULL){
	   printf("Error! opening sampel file");
	   // Program exits if the file pointer returns NULL.
	   exit(1);
	 }

	 
	 if ((fp_w = fopen(s,"w")) == NULL){
	   printf("Error! opening write file");
	   // Program exits if the file pointer returns NULL.
	   exit(1);
	 }
	 
	 fprintf(fp_final, "%s", "load(\"~/");
	 fprintf(fp_final, "%s\")\n", s);
	 

	 	   

	 fprintf(fp_w, "%s", s_file);
	 if(i==32){
	 fprintf(fp_w, "%s", "out = outfile(\"~/final_data.csv\" \"w\")\n");
	 }
	 else{
	   fprintf(fp_w, "%s", "out = outfile(\"~/final_data.csv\" \"a\")\n");
	 }
     while(1)
       {
	 c = fgetc(fp);
	 if(c==EOF){
	   //	   fprintf(fp_w, "%s", s_file);
	   fclose(fp); 
	   fclose(fp_w);
	   break;
	  }
	 else{
	   //	   printf("%c", c);
	   fprintf(fp_w, "%c", c);
	 }
       }
   }
   printf("Value of n=%d", num1);
   //   fclose(fp); 
   //   fclose(fp_w);
   return 0;
}
