#include "globals.h"
#include "util.h"

int Error=0;
int Parse = 0; // 模式flag：0-lex 1-parse
int lineno = 0;
FILE * myin;
FILE * myout;

// argv[0]-可执行程序名称
// argv[1]-待分析源码文件
// argv[2]-结果(单词编码/语法树)输出文件
// argv[3]-分析模式（0-词法分析 1-语法分析）
int main(int argc, char * argv[])
{ 
  TreeNode * syntaxTree;

  if (argc != 4) {
    fprintf(stderr,"4 Arguments Required!\n");
    exit(1);
  }
  
  if(argc == 4 && atoi(argv[3])) {
    Parse = 1; // flag切换为语法分析
  }

  myin = fopen(argv[1],"r");
  myout = stdout;

  if (myin == NULL) { // 打开文件失败
    fprintf(stderr,"File %s 找不到\n",argv[1]);
    exit(1);
  }

  freopen(argv[2],"w",stdout); // 标准输出重定向至输出文件
  if(!Parse) { // 执行词法分析
    TokenOut(); 
  } else { // 执行语法分析
    syntaxTree = parse();
    fprintf(myout,"Syntax Tree:\n");
    printTree(syntaxTree);
  }
  fclose(myin);
  fclose(myout);
  fclose(stdout);
  return 0;
}
