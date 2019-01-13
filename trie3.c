//                      gcc trie3.c -o trie3;./trie3
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define MASK 31
#define LEN 26
#define MAX 100
#define NODE_NUM 10000000

typedef struct Node
{
    char ch;
    unsigned int children;
    unsigned int sibling;
} Node;

int deleteStack[MAX], deleteTop;
unsigned int end, availableTop;

FILE *fp, *fpStack, *fpCnt;
int trie_fd, cnt_fd, stack_fd;
Node *trie;
int *cnt, *stack;

Node root;

int absolute(int x)
{
    return x - 2 * (x < 0) * x;
}

int newNode()
{
    //if stack is empty
    if (end >= NODE_NUM)
    {
        ; //printf("Trie memory full\n");
        exit(0);
    }
    if (availableTop == 0)
    // return ++end;
    {
        return ++end;
    }
    int val;
    val = stack[availableTop - 1 + 1];

    if (val == 1)
    {
        val = stack[availableTop - 2 + 1];
        availableTop -= 2;
        return val;
    }
    availableTop--;
    return val; 
}

int child(int curr, char c)
{
    if(trie[curr].children)
    {
        curr = trie[curr].children;
        if((trie[curr].ch & MASK) == (c-'a'))
            return curr;
        while(trie[curr].sibling)
        {
            curr = trie[curr].sibling;
            if((trie[curr].ch & MASK) == (c-'a'))
                return curr;
        }
        trie[curr].sibling = newNode();
        trie[trie[curr].sibling].ch = c-'a';
        return trie[curr].sibling;
    }
    else
    {
        trie[curr].children = newNode();
        trie[trie[curr].children].ch = c-'a';
        return trie[curr].children;
    }
}

void insertTrie(char *key)
{
    int curr = 1;
    int i;
    for(i=0; key[i]; i++)
        curr = child(curr, key[i]);
    trie[curr].ch = trie[curr].ch | (MASK+1);
}

int searchchild(int curr, char c)
{
    if(trie[curr].children)
    {
        curr = trie[curr].children;
        if((trie[curr].ch & MASK) == c-'a')
            return curr;
        while(trie[curr].sibling)
        {
            curr = trie[curr].sibling;
            if((trie[curr].ch & MASK) == c -'a')
                return curr;
        }
        return 0;
    }
    else
        return 0;
}

void searchTrie(char *key)
{
    int curr = 1;
    int i;
    for(i=0;key[i];i++)
    {
        curr = searchchild(curr, key[i]);
        if(!curr)
            {
                printf("%s not found\n",key);
                return;
            }
    }
    if(trie[curr].ch & (MASK+1))
        printf("%s found\n",key);
    else
        printf("%s not found\n",key);
}

void deleteTrie(char *key)
{
    int curr = 1;
    int i;
    for(i=0;key[i];i++)
    {
        curr = searchchild(curr, key[i]);
        if(!curr)
            {
                printf("%s not found\n",key);
                return;
            }
    }
    if(trie[curr].ch & (MASK+1))
        {
            trie[curr].ch = (trie[curr].ch) & (!(MASK+1));      //unset end of flag bit
            printf("%s deleted\n",key);
        }
    else
        printf("%s not found\n",key);
}

int main()
{
    trie_fd = open("TrieBinary", O_RDWR);
    if (trie_fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    trie = mmap(0, NODE_NUM * sizeof(int) * LEN, PROT_READ | PROT_WRITE, MAP_SHARED, trie_fd, 0);
    if (trie == MAP_FAILED)
    {
        close(trie_fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    end = trie[0].sibling;
    availableTop = 0;

    printf("Initial end = %d\n", end);

    // FILE *fpin;
    // char key[MAX];
    // fpin = fopen("words_10", "r");
    // while (fscanf(fpin, "%s", key) != -1)
    //     {
    //         printf("%s\n",key);
    //         insertTrie(key);
    //     }
    // fclose(fpin);
    // fpin = fopen("words_10", "r");
    // while (fscanf(fpin, "%s", key) != -1)
    //     {
    //         searchTrie(key);
    //     }
    // fclose(fpin);
    // fpin = fopen("words_10", "r");
    // while (fscanf(fpin, "%s", key) != -1)
    //     {
    //         deleteTrie(key);
    //     }
    // fclose(fpin);
    // fpin = fopen("words_10", "r");
    // while (fscanf(fpin, "%s", key) != -1)
    //     {
    //         searchTrie(key);
    //     }
    // fclose(fpin);

    // printf("end = %d\n",end);



    FILE *fpin;
    char choice='a', str[MAX];
    //;//printf("Enter choice:\ninsert: i\nsearch: s\nd\ndelete: d\nexit: e\n");
    while (choice != 'e')
    {
        scanf("%c", &choice);
        switch (choice)
        {
        case 'i':
            scanf(" %s\n", str);
            if (str[0] == '#')
            {
                fpin = fopen(str + 1, "r");
                while (fscanf(fpin, "%s", str) != -1)
                    insertTrie(str);
                fclose(fpin);
            }
            else
                insertTrie(str);
            break;
        case 's':
            scanf(" %s\n", str);
            if (str[0] == '#')
            {
                fpin = fopen(str + 1, "r");
                while (fscanf(fpin, "%s", str) != -1)
                    searchTrie(str);
                fclose(fpin);
            }
            else
                searchTrie(str);
            break;
        case 'd':
            scanf(" %s\n", str);
            if (str[0] == '#')
            {
                fpin = fopen(str + 1, "r");
                while (fscanf(fpin, "%s", str) != -1)
                    deleteTrie(str);
                fclose(fpin);
            }
            else
                deleteTrie(str);
            break;
        case 'e':
            break;
        default:
            ;//printf("Invalid choice\n");
        }
    }
    printf("\nend=%d\n", end);
    printf("availableTop=%d\n\n", availableTop);



    trie[0].sibling = end;
    if (munmap(trie, NODE_NUM * sizeof(int) * LEN) == -1)
    {
        perror("Error un-mmapping the file");
    }
    close(trie_fd);
    return 0;
}
