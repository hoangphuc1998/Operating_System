#include <stdio.h>
int main(){
	FILE* fp;
	fp = fopen("a.txt","w");
	fprintf(fp,"This is linux program...");
	fclose(fp);
	return 0;
}
