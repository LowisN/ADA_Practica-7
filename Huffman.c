#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 100
#define MAX_CODE_LEN 100
#define MAX_CHAR 256

// Estructura para el nodo del árbol Huffman
struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

// Estructura para el MinHeap
struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

// Estructura para almacenar un carácter y su frecuencia
struct CharFreq {
    char caracter;
    int frecuencia;
};

// Estructura para almacenar códigos Huffman
struct HuffmanCode {
    char caracter;
    char codigo[MAX_CODE_LEN];
};

// Funciones para crear nodos
struct MinHeapNode* newNode(char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// Crear un Min Heap
struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

// Función para intercambiar nodos
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Función de minHeapify
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Función para extraer el nodo mínimo
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// Función para insertar un nuevo nodo
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// Función para construir el MinHeap
void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

// Función para imprimir los códigos
void printCodes(struct MinHeapNode* root, int arr[], int top, FILE* codificado) {
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1, codificado);
    }
    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1, codificado);
    }
    if (!(root->left) && !(root->right)) {
        // Imprimir en archivo
        fprintf(codificado, "%c: ", root->data);
        for (int i = 0; i < top; ++i)
            fprintf(codificado, "%d", arr[i]);
        fprintf(codificado, "\n");
        
        // Imprimir en consola
        printf("%c: ", root->data);
        for (int i = 0; i < top; ++i)
            printf("%d", arr[i]);
        printf("\n");
    }
}

// Función para guardar códigos en un array
void guardarCodigos(struct MinHeapNode* root, int arr[], int top, struct HuffmanCode codigos[], int* numCodigos) {
    if (root->left) {
        arr[top] = 0;
        guardarCodigos(root->left, arr, top + 1, codigos, numCodigos);
    }
    if (root->right) {
        arr[top] = 1;
        guardarCodigos(root->right, arr, top + 1, codigos, numCodigos);
    }
    if (!(root->left) && !(root->right)) {
        codigos[*numCodigos].caracter = root->data;
        char tempCode[MAX_CODE_LEN] = "";
        for(int i = 0; i < top; i++) {
            char bit[2] = {'0' + arr[i], '\0'};
            strcat(tempCode, bit);
        }
        strcpy(codigos[*numCodigos].codigo, tempCode);
        (*numCodigos)++;
    }
}

// Función para codificar el texto
void codificarTexto(FILE* entrada, FILE* salida, struct HuffmanCode codigos[], int numCodigos) {
    char c;
    fprintf(salida, "\nTexto codificado:\n");
    
    // Regresar al inicio del archivo
    fseek(entrada, 0, SEEK_SET);
    
    while ((c = fgetc(entrada)) != EOF) {
        if (c != ' ' && c != '\n' && c != '\r') {
            for (int i = 0; i < numCodigos; i++) {
                if (codigos[i].caracter == c) {
                    fprintf(salida, "%s", codigos[i].codigo);
                    break;
                }
            }
        }
    }
    fprintf(salida, "\n");
}

// Función para decodificar texto
char decodificarBit(struct MinHeapNode** nodo, char bit) {
    if (bit == '0') {
        *nodo = (*nodo)->left;
    } else {
        *nodo = (*nodo)->right;
    }
    
    if (!((*nodo)->left) && !((*nodo)->right)) {
        char c = (*nodo)->data;
        return c;
    }
    return '\0';
}

void decodificarTexto(FILE* codificado, struct MinHeapNode* root) {
    char linea[1024];
    struct MinHeapNode* nodoActual = root;
    FILE* decodificado = fopen("decodificado.txt", "w");
    
    if (decodificado == NULL) {
        printf("Error al crear archivo decodificado.txt\n");
        return;
    }
    
    // Buscar la línea que contiene el texto codificado
    while (fgets(linea, sizeof(linea), codificado)) {
        if (strstr(linea, "Texto codificado:") != NULL) {
            // Leer la siguiente línea que contiene el texto codificado
            if (fgets(linea, sizeof(linea), codificado)) {
                printf("\nTexto decodificado:\n");
                fprintf(decodificado, "Texto decodificado:\n");
                
                for (int i = 0; linea[i] != '\0' && linea[i] != '\n'; i++) {
                    char c = decodificarBit(&nodoActual, linea[i]);
                    if (c != '\0') {
                        printf("%c", c);
                        fprintf(decodificado, "%c", c);
                        nodoActual = root;
                    }
                }
                printf("\n");
                fprintf(decodificado, "\n");
            }
            break;
        }
    }
    
    fclose(decodificado);
}

// Función principal de Huffman
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    while (minHeap->size != 1) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

int main(int argc, char* argv[]) {
    FILE *archivo, *codificado;
    char caracter;
    struct CharFreq *caracteresUnicos = NULL;
    int capacidad = 10;  // Capacidad inicial
    int size = 0;
    int totalCaracteres = 0;

    // Inicializar arreglo dinámico
    caracteresUnicos = (struct CharFreq*)malloc(capacidad * sizeof(struct CharFreq));
    if (caracteresUnicos == NULL) {
        printf("Error: No se pudo asignar memoria\n");
        return 1;
    }

    // Abrir el archivo en modo lectura
    archivo = fopen("texto.txt", "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo texto.txt\n");
        free(caracteresUnicos);
        return 1;
    }

    // Contar frecuencias
    while ((caracter = fgetc(archivo)) != EOF) {
        if (caracter != ' ' && caracter != '\n' && caracter != '\r') {
            // Buscar si el carácter ya existe
            int encontrado = 0;
            for (int i = 0; i < size; i++) {
                if (caracteresUnicos[i].caracter == caracter) {
                    caracteresUnicos[i].frecuencia++;
                    encontrado = 1;
                    break;
                }
            }
            
            // Si no se encontró, agregar nuevo carácter
            if (!encontrado) {
                // Verificar si necesitamos más espacio
                if (size == capacidad) {
                    capacidad *= 2;
                    struct CharFreq *temp = (struct CharFreq*)realloc(caracteresUnicos, capacidad * sizeof(struct CharFreq));
                    if (temp == NULL) {
                        printf("Error: No se pudo reasignar memoria\n");
                        free(caracteresUnicos);
                        fclose(archivo);
                        return 1;
                    }
                    caracteresUnicos = temp;
                }
                
                caracteresUnicos[size].caracter = caracter;
                caracteresUnicos[size].frecuencia = 1;
                size++;
            }
            totalCaracteres++;
        }
    }

    // Preparar arrays para Huffman
    char *data = (char*)malloc(size * sizeof(char));
    int *freqs = (int*)malloc(size * sizeof(int));
    
    if (data == NULL || freqs == NULL) {
        printf("Error: No se pudo asignar memoria para los arrays\n");
        free(caracteresUnicos);
        free(data);
        free(freqs);
        fclose(archivo);
        return 1;
    }

    // Copiar datos al formato requerido por Huffman
    for (int i = 0; i < size; i++) {
        data[i] = caracteresUnicos[i].caracter;
        freqs[i] = caracteresUnicos[i].frecuencia;
    }

    // Construir árbol de Huffman
    struct MinHeapNode* root = buildHuffmanTree(data, freqs, size);

    // Generar códigos y guardarlos
    int arr[MAX_TREE_HT], top = 0;
    codificado = fopen("codificado.txt", "w");
    if (codificado == NULL) {
        printf("Error al crear archivo codificado.txt\n");
        free(caracteresUnicos);
        free(data);
        free(freqs);
        fclose(archivo);
        return 1;
    }

    // Imprimir estadísticas y códigos
    printf("Diccionario de frecuencias:\n");
    for (int i = 0; i < size; i++) {
        printf("'%c': %d\n", caracteresUnicos[i].caracter, caracteresUnicos[i].frecuencia);
    }

    printf("\nCodigos de Huffman generados:\n");
    fprintf(codificado, "Codigos de Huffman:\n");
    printCodes(root, arr, top, codificado);

    // Guardar códigos en array para codificación
    struct HuffmanCode codigos[MAX_CHAR];
    int numCodigos = 0;
    int huffmanArr[MAX_CODE_LEN];  // Cambiado a int
    guardarCodigos(root, huffmanArr, 0, codigos, &numCodigos);

    // Codificar el texto y guardarlo
    codificarTexto(archivo, codificado, codigos, numCodigos);

    // Calcular tamaño original en bits (asumiendo ASCII de 8 bits)
    int tamanoOriginal = totalCaracteres * 8;
    
    // Calcular tamaño comprimido usando los códigos generados
    int tamanoComprimido = 0;
    fseek(archivo, 0, SEEK_SET);
    while ((caracter = fgetc(archivo)) != EOF) {
        if (caracter != ' ' && caracter != '\n' && caracter != '\r') {
            for (int i = 0; i < numCodigos; i++) {
                if (codigos[i].caracter == caracter) {
                    tamanoComprimido += strlen(codigos[i].codigo);
                    break;
                }
            }
        }
    }

    // Imprimir tasas de compresión
    printf("\nEstadísticas de compresión:\n");
    printf("Tamaño original: %d bits\n", tamanoOriginal);
    printf("Tamaño comprimido: %d bits\n", tamanoComprimido);
    printf("Tasa de compresión: %.2f%%\n", 100.0 * (tamanoOriginal - tamanoComprimido) / tamanoOriginal);

    // Decodificar el texto
    fclose(codificado);
    codificado = fopen("codificado.txt", "r");
    if (codificado != NULL) {
        decodificarTexto(codificado, root);
    }

    // Liberar memoria y cerrar archivos
    free(caracteresUnicos);
    free(data);
    free(freqs);
    fclose(archivo);
    fclose(codificado);

    return 0;
}