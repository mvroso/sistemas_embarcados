#ifndef MEMORY_CARD
#define MEMORY_CARD

class MemoryCard {
    public:
    MemoryCard();

    void setup(int pino);
    void escrever_log(String mensagem);
    String ler_log(void);

    private:
    int pin;
};


#endif // MEMORY_CARD
