    #ifndef RTC
    #define RTC

    class Rtc {
        public:
        Rtc();

        void setup(int pinos[]);
        String receberData(void);
        String receberHora(void);

        private:
        int pinSDA;
        int pinSCL;
    };

    #endif // RTC
