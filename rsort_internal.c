#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define MAX_RECORD_COUNT 1000000
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

long get_size_of_file(char *FilePath)
{//得到檔案總共的大小
	FILE *fp;
	long sizeOfFile;
	fp = fopen(FilePath,"rb");
	assert(fp != NULL); //確定有開檔案成功
	fseek(fp, 0, SEEK_END); //移動到尾巴(seek_end)在這移動0個byte
	sizeOfFile = ftell(fp); //呼叫 ftell() 得到當前檔案指標在第幾個 byte, 把值存到 sizeOfFile
	fclose(fp);
	return sizeOfFile;
}

int compare(const void *a, const void *b)
{
	return strcmp( ((recData *)a)->keyBuffer,\
						((recData *)b)->keyBuffer);
}


void recExtraction(char *FilePath, char *recBegin,long sizeOfFile, char *p2key)
{
	int recCounter;
	int recCnt;
	char *buffer;
	char *p2char;
	char *p2rec;
	char *p2cutPoint[MAX_RECORD_COUNT];
	FILE *fp,*fp1;
	size_t returnOfRead;

	// 用 malloc() 宣告一個字串, 長度為 file_bytes_size + 1, 多 1 是為了放反斜線零 '\0'
	buffer = (char*) malloc(sizeof(char) * sizeOfFile+1) ; 
	assert(buffer != NULL);

	// 開檔, 然後把整個檔案讀取存放到 buffer 指向的記憶體位置中
	fp = fopen(FilePath, "rb");
	returnOfRead = fread(buffer, 1, sizeOfFile, fp); //讀檔
	// printf("%s",buffer);
	// puts("修改前：");
	// printf("returnOfRead = %zu\n",returnOfRead);
	// printf("strlen(buffer) = %lu\n",strlen(buffer));
	if(returnOfRead != strlen(buffer))
	{
		replace_null_char(returnOfRead, buffer);
		//  printf("returnOfRead = %zu\n",returnOfRead);
		//  printf("strlen(buffer) = %lu\n",strlen(buffer));
		assert(returnOfRead == strlen(buffer));
	} //偵測錯誤，不寫可能會少一些資料

	// 開始找切割點, 存在 p2cutpoints 的陣列裡
	recCnt = 0;
	recCounter = 0;
	p2rec = strstr(buffer,recBegin);

	while (p2rec != NULL)
	{
		recCounter++ ;
		p2cutPoint[recCnt++] = p2rec;
		//已經指到找到的地方，加上strlen(recordBegin)，確定下一次不會找到同一個
        //原本想用strtok()，但是strtok也讀取只包含空格的空白行，故不使用此方法
		p2rec = strstr(p2rec + strlen(recBegin), recBegin); 
	}
	p2cutPoint[recCnt] = &buffer[strlen(buffer) - 1];//把所有都記下來
	printf("Total record count is %d\r\n",recCounter);

	// 印出第一筆rec
    // for(p2char = p2cutPoint[0]; p2char < p2cutPoint[1]; p2char++)
	//   	putchar(*p2char);
	// printf("\n\n");

	recData *p2data = malloc(sizeof(recData)*recCounter);
	for (int i = 0; i < recCounter ; i++)
	{
		p2data[i].beginOfSet = p2cutPoint[i] - p2cutPoint[0];
		p2data[i].recSize = p2cutPoint[i+1] - p2cutPoint[i];
		p2data[i].keyBuffer[MAX_KEY_LENGHT] = '\0'; 
		// strncpy(p2data[i].keyBuffer, p2cutPoint[i], MAX_KEY_LENGHT);
		strncpy(p2data[i].keyBuffer, strstr(p2cutPoint[i], p2key) == NULL ? p2cutPoint[i] : strstr(p2cutPoint[i], p2key), MAX_KEY_LENGHT);
		//如果key不存在的話就從頭開始當key（不然對NULL取值會segmentation fault），如果存在就從key的位址開始當key
		// printf("%s",p2data -> keyBuffer);
	}
	//printf("%s",p2data[recCounter-1].keyBuffer);

	// firstRec.beginOfSet = p2cutPoint[0] - p2cutPoint[0];
	// firstRec.recSize = p2cutPoint[1] - p2cutPoint[0];
	// firstRec.keyBuffer[MAX_KEY_LENGHT] = '\0';
	// strncpy(firstRec.keyBuffer,p2cutPoint[0],MAX_KEY_LENGHT);
	// printf("Key buffer of first record is \n\n%s\n", firstRec.keyBuffer);

	qsort(p2data, recCounter, sizeof(recData), compare);
	
	char *str;
	fp1  = fopen("result.txt","wb");
	for(int i = 0; i < 1000; i++)
	{
		printf("[%d]\n\n", i);
		for(int j = p2data[i].beginOfSet, k =0; k < p2data[i].recSize; k++)
		{
			printf("%c", buffer[j + k]);
		}
		printf("\n\n");
	}
	fclose(fp1);
	fclose(fp);
	free(buffer); //沒有要用了, 釋放記憶體
	free(p2data);
}



int main(int argc, char *argv[] )
{
	int TotalRecCnt;
	// char FilePath[] = "10k_td.rec";
	char FilePath[] = "ettoday.rec";
	char recBegin[] = "@\n"; //record begin pattern
	char *p2key;
	long sizeOfFile;
	clock_t time;
	time = clock();
	sizeOfFile =  get_size_of_file(FilePath);	
	// printf("%ld",sizeOfFile);
	for (int i = 1; i < argc; i++) 
	{
		if (!strcmp(argv[i],"-k")) 
		{
			p2key = argv[i+1];
		}
	}	
	
	recExtraction(FilePath, recBegin, sizeOfFile,p2key);
	time = clock()-time;
	printf("execute time = %f\n",(double)time/CLOCKS_PER_SEC);
}