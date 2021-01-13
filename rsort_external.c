#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define MAX_MEM 1<<30
#define MAX_RECORD_COUNT 500000
#define MAX_KEY_LENGHT 1024

typedef struct recData	
{
	long beginOfSet;
	size_t recSize;
	char keyBuffer[MAX_KEY_LENGHT + 1];
}recData;

void replace_null_char(size_t returnOfRead,char *buffer)
{//解決strlen遇到'\0'會停止的問題
	size_t index;
	for(index = 0; index < returnOfRead ; index++)
		if(buffer[index] == '\0')
			buffer[index] = ' ';
}

int compare(const void *a, const void *b)
{
	return strcmp( ((recData *)a)->keyBuffer,\
						((recData *)b)->keyBuffer);
}


find_start_key_pos(size_t returnOfRead, char *recBegin, char *buffer, char *p2key,int segId,off_t byteBase)
{
    FILE *fp;
    int recCnt;
    int recCounter;
    char fname[256];
    char *p2rec;
    char *p2cutPoint[MAX_RECORD_COUNT];

    if(returnOfRead != strlen(recBegin))
	{
		replace_null_char(returnOfRead, buffer);
		assert(returnOfRead == strlen(buffer));
	}

    // 開始找切割點, 存在 p2cutpoints 的陣列裡
	recCnt = 0;
	recCounter = 0;
	p2rec = strstr(buffer,recBegin);

	while (p2rec != NULL)
	{
		recCounter++ ;
		p2cutPoint[recCnt++] = p2rec;
		p2rec = strstr(p2rec + strlen(recBegin), recBegin); 
	}
	p2cutPoint[recCnt] = &buffer[strlen(buffer) - 1];//把所有都記下來
	printf("The record%d count is %d\r\n",segId,recCounter);

	recData *p2data = malloc(sizeof(recData)*recCounter);
	for (int i = 0; i < recCounter ; i++)
	{
		p2data[i].beginOfSet = p2cutPoint[i] - p2cutPoint[0] + byteBase;
		p2data[i].recSize = p2cutPoint[i+1] - p2cutPoint[i];
		p2data[i].keyBuffer[MAX_KEY_LENGHT] = '\0'; 
		strncpy(p2data[i].keyBuffer, strstr(p2cutPoint[i], p2key) == NULL ? p2cutPoint[i] : strstr(p2cutPoint[i], p2key), MAX_KEY_LENGHT);
	}
	qsort(p2data, recCounter, sizeof(recData), compare);

    sprintf(fname,"File_%d", segId);
    fp = fopen(fname,"wb");

    for(int i = 0; i < recCounter; i++)
	{
        fwrite(&p2data[i], 1, sizeof(p2data[i]), fp); //1040個byte
	}

    fclose(fp);
}

int main(int argc, char *argv[])
{
    int segId = 0;
    char filePath[] = "ettoday.rec";
	char filePath0[] = "File_0";
	char filePath1[] = "File_1";
    char recBegin[] = "@\n";
    char *buffer,*chunk;
    char *p2key;
	int arrSize = 1000, counter = 0;
	int i, j;
	int nrec1, nrec2; //number of record
	size_t retread1, retread2, retread3;
	recData recArr1[arrSize], recArr2[arrSize],temp;
	clock_t time;
	off_t byteBase;
    FILE *fp;
	FILE *fp1, *fp2, *fw, *fp3, *fp4;
    size_t returnOfRead,returnOfRead0,returnOfRead1;

	time = clock();

    fp = fopen(filePath,"rb");
    assert(fp != NULL);

    buffer = (char*) malloc(sizeof(char) * MAX_MEM + 1) ; 
	chunk = (char*) malloc(sizeof(char) *  MAX_MEM +1);
    
	for (int i = 1; i < argc; i++) 
	{
		if (!strcmp(argv[i],"-k")) 
			p2key = argv[i+1];
	}	
	
	byteBase = 0;
    while(!feof(fp))
    {
        returnOfRead = fread(buffer,1,MAX_MEM,fp);
        buffer[returnOfRead] = '\0';
        find_start_key_pos(returnOfRead,recBegin,buffer,p2key,segId,byteBase);
		byteBase += returnOfRead;
        segId++;
    }
	
    fclose(fp);

	// open two sorted file
	fp1 = fopen("File_0", "rb");
	fp2 = fopen("File_1", "rb");
	// open answer file (final file)
	fw = fopen("final_file", "wb");
	// initial value = -1
	nrec1 = -1;
	nrec2 = -1;	

	for(i = 0, j = 0; !(feof(fp1) && feof(fp2) && (i == nrec1) && (j == nrec2)); )
	{
		// arr1 is empty or is all written out
		// read next chunk
		if( (nrec1 == -1 || nrec1 == i) && !feof(fp1))
		{
			retread1 = fread(recArr1, 1, arrSize * sizeof(recData), fp1);
			nrec1 = retread1 / sizeof(recData);
			i = 0;
		}

		// arr2 is empty or is all written out
		if( (nrec2 == -1 || nrec2 == j) && !feof(fp2))
		{
			retread2 = fread(recArr2, 1, arrSize * sizeof(recData), fp2);
			nrec2 = retread2 / sizeof(recData);
			j = 0;
		}

		// first file is clear and done
		if(feof(fp1) && i == nrec1)
		{
			fwrite(&recArr2[j], 1, sizeof(recData), fw);
			j++;
		}
		// second file is clear and done
		else if(feof(fp2) && j == nrec2)//看誰先寫完
		{
			fwrite(&recArr1[i], 1, sizeof(recData), fw);
			i++;
		}

		else if(strcmp(recArr1[i].keyBuffer, recArr2[j].keyBuffer) >= 0)//array都還有東西 看keybuffer誰大誰小
		{//小到大
			fwrite(&recArr2[j], 1, sizeof(recData), fw);
			j++;			
		}
		else
		{
			fwrite(&recArr1[i], 1, sizeof(recData), fw);
			i++;
		}
		counter++;
		assert(counter<1000000);
	}

	// close file descriptors
	fclose(fp1);
	fclose(fp2);
	fclose(fw);

	fp = fopen(filePath,"rb");
	fp3 = fopen("final_file","rb");
	fp4 = fopen("sortedrec.txt","wb");
	while(!feof(fp3))
	{
	fread(&temp, 1, sizeof(recData), fp3);
	fseek(fp, SEEK_SET, temp.beginOfSet);
	retread3=fread(chunk, 1, temp.recSize, fp);
	chunk[retread3]='/0';
	fwrite(chunk, 1, retread3, fp4);
	}
	

	fclose(fp);
	fclose(fp3);
	fclose(fp4);


	time = clock()-time;
	printf("Execute time = %f\n",(double)time/CLOCKS_PER_SEC);

}