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

	 if ((fp_final = fopen("final.ocn","w")) == NULL){
	   printf("Error! opening sampel file");
	   // Program exits if the file pointer returns NULL.
	   exit(1);
	 }

   for( i=1;i<28;i++){
     //     fp_w = fopen("read_data_sample1.ocn", "w"); // "w" defines "writing mode"


     	   char s[256]="oceanScript";
	   my_itoa(i,tmp);
	   strcat(s,tmp);
	   strcat(s,".ocn");
	   printf("%s\n",s);
	   //	   getchar();

	   

	   char s_file[2560]="vecFile( \"/home/bl888078/digital/4HL6A/IPQ32_8/ANALOG_SIM_vectors/IPQ32_8_NTHK_DOHERTY_CASE3_FULL_SWEEP_TESTCASE";
	   strcat(s_file,tmp);
	   strcat(s_file,"/pwlf_sclk /home/bl888078/digital/4HL6A/IPQ32_8/ANALOG_SIM_vectors/IPQ32_8_NTHK_DOHERTY_CASE3_FULL_SWEEP_TESTCASE");
	   strcat(s_file,tmp);
	   strcat(s_file,"/pwlf_sdata /home/bl888078/digital/4HL6A/IPQ32_8/ANALOG_SIM_vectors/IPQ32_8_NTHK_DOHERTY_CASE3_FULL_SWEEP_TESTCASE");
	   
	   strcat(s_file,tmp);
	   strcat(s_file,"/pwlf_sdata_enb /home/bl888078/digital/4HL6A/IPQ32_8/ANALOG_SIM_vectors/IPQ32_8_NTHK_DOHERTY_CASE3_FULL_SWEEP_TESTCASE");

	   strcat(s_file,tmp);
	   strcat(s_file,"/pwlf_vio\")\n");

	   //	   printf("%s\n",s_file);
	   //	   getchar();

	   


	   

	 if ((fp = fopen("oceanScript.ocn","r")) == NULL){
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
	 

	 	   

	 fprintf(fp_w, "%s", "simulator( 'spectre )\n hostMode( 'distributed )\n design(	 \"/scratch/noclean/wsd/cmos_soi_projects/bl888078/simulation/4HL6A_2_gpiopeak/spectre/schematic/netlist/netlist\")\n resultsDir( \"/scratch/noclean/wsd/cmos_soi_projects/bl888078/simulation/ptmppp_4HL6A_2_gpiopeak/spectre/schematic\" ) \n modelFile( \n '(\"/local/cadtree/cad.hp/tech/tsmc18gpIIa/OA.v0.6m1k2f20k5vnp/tsmc18/../models/spectre/cmn018_gp2a_5v_v1d4_usage.scs\" \"tt_lib\") \n '(\"/local/cadtree/cad.hp/tech/tsmc18gpIIa/OA.v0.6m1k2f20k5vnp/tsmc18/../models/spectre/c018_efuse_v1d0.scs\" \"tt_efuse\") \n )\n");
	 fprintf(fp_w, "%s", s_file);
	 
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
