#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TREE_HT 100
#define MAX_CHAR 257
#define MAX_CODE_LEN                                                           \
  30 // Or another appropriate value based on your Huffman codes

// Structure Definitions
struct MinHeapNode {
  char item;
  unsigned freq;
  struct MinHeapNode *left, *right;
};

struct MinHeap {
  unsigned size;
  unsigned capacity;
  struct MinHeapNode **array;
};

// Function Prototypes
struct MinHeapNode *newNode(char item, unsigned freq);
struct MinHeap *createMinHeap(unsigned capacity);
void swapMinHeapNode(struct MinHeapNode **a, struct MinHeapNode **b);
void minHeapify(struct MinHeap *minHeap, int idx);
void buildMinHeap(struct MinHeap *minHeap);
struct MinHeap *createAndBuildMinHeap(char data[], int freq[], int size);
struct MinHeapNode *extractMin(struct MinHeap *minHeap);
struct MinHeapNode *buildHuffmanTree(char data[], int freq[], int size);
void insertMinHeap(struct MinHeap *minHeap, struct MinHeapNode *minHeapNode);
int isSizeOne(struct MinHeap *minHeap);
int isLeaf(struct MinHeapNode *root);
void generateHuffmanCodes(struct MinHeapNode *root, int arr[], int top,
                          char codes[MAX_CHAR][MAX_TREE_HT]);
void compress();
void decompress();

struct MinHeapNode *newNode(char item, unsigned freq) {
  struct MinHeapNode *temp =
      (struct MinHeapNode *)malloc(sizeof(struct MinHeapNode));
  temp->left = temp->right = NULL;
  temp->item = item;
  temp->freq = freq;
  return temp;
}

struct MinHeap *createMinHeap(unsigned capacity) {
  struct MinHeap *minHeap = (struct MinHeap *)malloc(sizeof(struct MinHeap));
  minHeap->size = 0;
  minHeap->capacity = capacity;
  minHeap->array = (struct MinHeapNode **)malloc(minHeap->capacity *
                                                 sizeof(struct MinHeapNode *));
  return minHeap;
}

void swapMinHeapNode(struct MinHeapNode **a, struct MinHeapNode **b) {
  struct MinHeapNode *t = *a;
  *a = *b;
  *b = t;
}

void minHeapify(struct MinHeap *minHeap, int idx) {
  int smallest = idx;
  int left = 2 * idx + 1;
  int right = 2 * idx + 2;

  if (left < minHeap->size &&
      minHeap->array[left]->freq < minHeap->array[smallest]->freq)
    smallest = left;

  if (right < minHeap->size &&
      minHeap->array[right]->freq < minHeap->array[smallest]->freq)
    smallest = right;

  if (smallest != idx) {
    swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
    minHeapify(minHeap, smallest);
  }
}
void buildMinHeap(struct MinHeap *minHeap) {
  int n = minHeap->size - 1;
  int i;
  for (i = (n - 1) / 2; i >= 0; --i)
    minHeapify(minHeap, i);
}

struct MinHeap *createAndBuildMinHeap(char data[], int freq[], int size) {
  struct MinHeap *minHeap = createMinHeap(size);
  for (int i = 0; i < size; ++i)
    minHeap->array[i] = newNode(data[i], freq[i]);
  minHeap->size = size;
  buildMinHeap(minHeap);
  return minHeap;
}

struct MinHeapNode *extractMin(struct MinHeap *minHeap) {
  struct MinHeapNode *temp = minHeap->array[0];
  minHeap->array[0] = minHeap->array[minHeap->size - 1];
  --minHeap->size;
  minHeapify(minHeap, 0);
  return temp;
}

struct MinHeapNode *buildHuffmanTree(char data[], int freq[], int size) {
  struct MinHeapNode *left, *right, *top;

  struct MinHeap *minHeap = createAndBuildMinHeap(data, freq, size);

  while (!isSizeOne(minHeap)) {
    left = extractMin(minHeap);
    right = extractMin(minHeap);

    top = newNode('$', left->freq + right->freq);
    top->left = left;
    top->right = right;

    insertMinHeap(minHeap, top);
  }

  return extractMin(minHeap);
}

int isSizeOne(struct MinHeap *minHeap) { return (minHeap->size == 1); }

void insertMinHeap(struct MinHeap *minHeap, struct MinHeapNode *minHeapNode) {
  ++minHeap->size;
  int i = minHeap->size - 1;

  while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
    minHeap->array[i] = minHeap->array[(i - 1) / 2];
    i = (i - 1) / 2;
  }
  minHeap->array[i] = minHeapNode;
}
int isLeaf(struct MinHeapNode *root) { return !(root->left) && !(root->right); }
void storeTreeStructure(struct MinHeapNode *node, FILE *stream) {
    if (node == NULL) return;
    if (node->left == NULL && node->right == NULL) {
        fprintf(stream, "1%c", node->item);
    } else {
        fputc('0', stream);
        storeTreeStructure(node->left, stream);
        storeTreeStructure(node->right, stream);
    }
}
struct MinHeapNode *reconstructTree(FILE *stream, int *pos) {
    int ch = fgetc(stream);
    (*pos)++;

    if (ch == EOF) return NULL;
    if (ch == '1') {  // Leaf node
        ch = fgetc(stream);
        (*pos)++;
        return newNode((char)ch, 0);
    } else if (ch == '0') {  // Internal node
        struct MinHeapNode *left = reconstructTree(stream, pos);
        struct MinHeapNode *right = reconstructTree(stream, pos);
        struct MinHeapNode *node = newNode('$', 0);
        node->left = left;
        node->right = right;
        return node;
    }
    return NULL;
}

void printArr(int arr[], int n) {
  for (int i = 0; i < n; ++i)
    printf("%d", arr[i]);
  printf("\n");
}

void generateHuffmanCodes(struct MinHeapNode *root, int arr[], int top,
                          char codes[MAX_CHAR][MAX_TREE_HT]) {
  if (root->left) {
    arr[top] = 0;
    generateHuffmanCodes(root->left, arr, top + 1, codes);
  }

  if (root->right) {
    arr[top] = 1;
    generateHuffmanCodes(root->right, arr, top + 1, codes);
  }

  if (isLeaf(root)) {
    codes[(unsigned char)root->item][top] = '\0';
    for (int i = 0; i < top; ++i) {
      codes[(unsigned char)root->item][i] = arr[i] + '0';
    }
  }
}

void compress() {
    int freq[MAX_CHAR] = {0};
    char arr[MAX_CHAR];
    int uniqueCharCount = 0;

    // Read sample.txt and calculate frequency of each character
    FILE *file = fopen("sample.txt", "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (freq[(unsigned char)ch] == 0) {
            arr[uniqueCharCount++] = ch;
        }
        freq[(unsigned char)ch]++;
    }
    fclose(file);

    // Build Huffman Tree and generate codes
    struct MinHeapNode *root = buildHuffmanTree(arr, freq, uniqueCharCount);
    char codes[MAX_CHAR][MAX_TREE_HT];
    int codeArr[MAX_TREE_HT];
    generateHuffmanCodes(root, codeArr, 0, codes);

    // Open compress.txt to write
    FILE *outputStream = fopen("compress.txt", "w");
    if (!outputStream) {
        perror("Failed to open output file");
        return;
    }

    // Store Huffman tree structure
    storeTreeStructure(root, outputStream);
    fprintf(outputStream, "\n----\n");  // Delimiter

    // Write compressed data
    file = fopen("sample.txt", "r");
    if (!file) {
        perror("Failed to open input file");
        fclose(outputStream);
        return;
    }

    while ((ch = fgetc(file)) != EOF) {
        fprintf(outputStream, "%s", codes[(unsigned char)ch]);
    }

    fclose(file);
    fclose(outputStream);
}

void decompress() {
    // Open the compressed file
    FILE *inputFile = fopen("compress.txt", "r");
    if (!inputFile) {
        perror("Failed to open input file");
        return;
    }

    // Reconstruct the Huffman tree
    int pos = 0;
    struct MinHeapNode *root = reconstructTree(inputFile, &pos);

    // Find the delimiter
    fseek(inputFile, pos, SEEK_SET);
    while (fgetc(inputFile) != '\n');  // Skip until the end of the line

    // Open the output file for the decompressed text
    FILE *outputFile = fopen("decompressed.txt", "w");
    if (!outputFile) {
        perror("Failed to open output file");
        fclose(inputFile);
        return;
    }

    // Decode the compressed data using the Huffman tree
    struct MinHeapNode *current = root;
    char bit;
    while ((bit = fgetc(inputFile)) != EOF) {
        if (bit == '0') {
            current = current->left;
        } else if (bit == '1') {
            current = current->right;
        }

        if (isLeaf(current)) {
            fputc(current->item, outputFile);
            current = root;  // Go back to the root for the next character
        }
    }

    // Close the files
    fclose(inputFile);
    fclose(outputFile);
}



int main() {
  int choice;

  printf("Enter 1 for compression, 2 for decompression: ");
  scanf("%d", &choice);

  switch (choice) {
  case 1:
    compress();
    break;
  case 2:
    decompress();
    break;
  default:
    printf("Invalid choice.\n");
  }

  return 0;
}
