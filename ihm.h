#ifndef IHM
#define IHM

class Ihm {
    public:
    Ihm();

    void setup(int port);

    private:
    char ssid[100] = "";
    char password[100] = "";
    uint32_t ip[4];
};


#endif // IHM
