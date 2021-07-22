#ifndef REMOTE_CONTROL
#define REMOTE_CONTROL

class RemoteControl {
    public:
    RemoteControl();

    void setup(int pino);
    void acionar(void);

    private:
    int pin;
};


#endif // REMOTE_CONTROL