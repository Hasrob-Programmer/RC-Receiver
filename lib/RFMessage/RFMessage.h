#include <math.h>

typedef struct pseudo_10_bit_type
{
    unsigned char bottom_bit;
    unsigned char top_bit;
}tchar_t;

int convert_to_10_bit(unsigned char bottom_bit, unsigned char top_bit)
{
    unsigned char bit_9 = top_bit % 2;
    unsigned char bit_10 = (top_bit/2) % 2;

    return bottom_bit + pow(2, 8) * bit_9 + pow(2, 9) * bit_10;
}

tchar_t convert_from_10_bit(int value)
{
    tchar_t tc;
    unsigned char bits[10];
    
    for(int i = 0; i < 10; i++)
    {
        bits[i] = value % 2;
        value = value /2;
    }
    
    /* get bottom bit (8bit) and top bot (2bit) */
    tc.bottom_bit = 0;
    tc.top_bit = 0;
    for(int i = 0; i < 8; i++)
        tc.bottom_bit += pow(2, i) * bits[i]; 
    for(int i = 8; i < 10; i++)
        tc.top_bit += pow(2, i - 8) * bits[i]; 
    
    return tc;
}

#define RFMESSAGE_LEN 4 /* byte */

/**
 * @brief Message protocol
 * 
 * Message structure (byte) :
 * => x_axis(2) , y_axis(2)
 * 
 */

class RFMessage
{
    public:
        int x_axis; /* 10-bit ADC*/
        int y_axis; /* 10-bit ADC*/
        
        RFMessage() : 
        x_axis(0),
        y_axis(0)
        {}

        unsigned char* getBuffer()
        {
            tchar_t x_axis_buf = convert_from_10_bit(x_axis);
            tchar_t y_axis_buf = convert_from_10_bit(y_axis);

            static unsigned char buf[RFMESSAGE_LEN];

            buf[0] = x_axis_buf.bottom_bit;
            buf[1] = x_axis_buf.top_bit;
            buf[2] = y_axis_buf.bottom_bit;
            buf[3] = y_axis_buf.top_bit;
           
           return buf;
        }
        
        void getValueFromBuffer(unsigned char* buffer, int len = RFMESSAGE_LEN)
        {
            x_axis = convert_to_10_bit(buffer[0], buffer[1]);
            y_axis = convert_to_10_bit(buffer[2], buffer[3]);
        }
};