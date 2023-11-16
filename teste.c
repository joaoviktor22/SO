#define MAX_ITERATIONS 40000000000 // Ajuste este valor conforme necessário

int main(){
    long long int i;
    for (i = 0;i < MAX_ITERATIONS ; i++) {
    //multiplicação simples para consumir CPU
        int result = i * i;
    }

    return 0;
}