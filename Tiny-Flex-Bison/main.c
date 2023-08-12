#include "globals.h"
#include "util.h"

/* allocate global variables */
int Error=0;
int Parse = 0; // 模式flag：0-lex 1-parse
int lineno = 0;
FILE * source;
FILE * listing;

int main(int argc, char * argv[])
{
  if (argc != 4) {
    fprintf(stderr,"4 Arguments Required!\n");
    exit(1);
  }
  if(argc == 4 && atoi(argv[3])) {
    Parse = 1; // flag切换为语法分析
  }

  TreeNode * syntaxTree;

   source = fopen(argv[1], "r");
   listing = stdout;

  if (source == NULL) { // 打开文件失败
    fprintf(stderr,"File %s 找不到\n",argv[1]);
    exit(1);
  }
  freopen(argv[2],"w",stdout);

  if(!Parse) { /* yylex(); */
    TokenOut();
  } else { /* yyparse(); */
    syntaxTree = parse();
    fprintf(source,"Syntax Tree:\n");
    printTree(syntaxTree);
  }

  fclose(source);
  fclose(listing);
  fclose(stdout);
  return 0;
}