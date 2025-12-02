#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// структура для дерева
typedef struct Node {
    unsigned char symv;
    int freq;
    struct Node *left, *right;
} Node;
// функция для создания узлов, пустых и конечных
Node* new_node(unsigned char symv,int freq){
    Node* node=(Node*)malloc(sizeof(Node));
    node->symv=symv;
    node->freq=freq;
    node->left=node->right=NULL;
    return node;
}
// подсчет часток встречающихся симпволов в файле в отдельный массив
void count_freq(char* ffile,int* freq){
    FILE* file=fopen(ffile,"rb");
    for (int i=0;i<256;i++) {
        freq[i]=0;
    }
    int c;
    while ((c=fgetc(file))!=EOF){
        freq[c]++;
    }
    fclose(file);
}
// функция построения дерева
Node* new_tree(int* freq){
    //массив для узлов
    Node* nodes[256];
    //счетчик узлов
    int count=0;
    // создаем концы дерева для каждого символа из файла
    for (int i=0;i<256;i++){
        if (freq[i]>0){
            nodes[count++]=new_node(i,freq[i]);
        }
    }
    //ищем минимумы и объединяем их в один узел
    while (count>1){
        //предполагаем что первые два элемента минимумы
        int min1=0,min2=1;
        if (nodes[min1]->freq > nodes[min2]->freq){
            int temp=min1;
            min1=min2;
            min2=temp;
        }
        //проверяем есть ли меньше в остальном массиве
        for (int i=2;i<count;i++){
            if (nodes[i]->freq < nodes[min1]->freq){
                min2=min1;
                min1=i;
            } 
            else if (nodes[i]->freq < nodes[min2]->freq){
                min2=i;
            }
        }
        //создаем узел из двум минимуальных элементов, его freq равна сумме левого и правого
        Node* up_node=new_node(0,nodes[min1]->freq + nodes[min2]->freq);
        up_node->left=nodes[min1];
        up_node->right=nodes[min2];
        // в меньший из минимумов записываем получившийся узел
        nodes[min1]=up_node;
        // в другой минимум записываем последний и сокращаем весь массив
        nodes[min2]=nodes[count-1];
        count--;
        //делаем до тех пор пока не создадим так все дерево
    }
    //функция возвращает указатель на вершину дерева
    return nodes[0];
}
//функция присваивает каждому символу свой уникальный код, начиная с вершины
//передаются указатель на текущую вершину, текущее состояние кода, текущая длина кода и массив хранения кодов
void generateCodes(Node* root,char* code,int len,char(*codes)[256]){
    // если дошли до одного из концов дерева то сохраняем код
    if (root->left==NULL && root->right==NULL){
        code[len]='\0';
        strcpy(codes[root->symv],code);
        return;
    }
    //влево приписываем нули
    code[len]='0';
    generateCodes(root->left,code,len+1,codes);
    //вправо приписываем единицы
    code[len]='1';
    generateCodes(root->right,code,len+1,codes);
}
// функция где заменяем все символы на их коды
void fcode(char* inputFile,char* outputFile,char(*codes)[256]){
    FILE* input=fopen(inputFile,"rb");
    FILE* output=fopen(outputFile,"wb");
    int c;
    // считываем посимвольно до конца файла и записываем в выходной файл уже коды
    while ((c=fgetc(input))!=EOF){
        fprintf(output,"%s",codes[c]);
    }
    fclose(input);
    fclose(output);
}
// функция декодирования файла из кода обратно в символы
void decode(const char* inputFile,const char* outputFile,Node* tree){
    FILE* input=fopen(inputFile,"rb");
    FILE* output=fopen(outputFile,"wb");
    // начинаем с вершины дерева
    Node* current=tree;
    int c;
    // считывем посимвольно ноль или единицу
    //если ноль то идем дальше читать налево
    // если единица то идем направо
    while ((c=fgetc(input))!=EOF){
        if (c=='0'){
            current=current->left;
        } 
        else {
            current=current->right;
        }
        // если дошли до одного из концов дерева то записываем символ и возвращаемся в начало
        if (current->left==NULL && current->right==NULL){
            fputc(current->symv,output);
            current=tree;
        }
    }
    fclose(input);
    fclose(output);
}
int main(){
    // считываем имена файлов
    char input_file[100];
    scanf("%s",input_file);
    char encode_file[100];
    scanf("%s",encode_file);
    char decode_file[100];
    scanf("%s",decode_file);
    // считаем частоты символов во входном файле
    int freq[256];
    count_freq(input_file, freq);
    // создаем дерево
    Node* tree=new_tree(freq);
    // создаем массив для кодов
    char codes[256][256]={0};
    // переменная для создания кодов
    char code[256];
    // создаем коды для всех символов
    generateCodes(tree,code,0,codes);
    // кодируем символы из исходного файла в другой
    fcode(input_file,encode_file,codes);
    // декодируем файл чтобы проверить сходство
    decode(encode_file,decode_file,tree);
    return 0;
}