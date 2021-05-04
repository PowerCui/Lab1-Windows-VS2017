#include <stdio.h>
#include <string.h>

#include "protocol.h"
#include "datalink.h"

#define DATA_TIMER  2000
#define DATA 0
#define ACK  1
#define NAK  2
#define MAX_SEQ  7
#define INC(n) (n = n < MAX_SEQ ? n+1 : 0) 

struct FRAME
{
	unsigned char kind;
	unsigned char ack;
	unsigned char seq;
	unsigned char data[PKT_LEN];
	unsigned int  padding;
};

typedef enum {true,false} boolean;

static boolean between(unsigned char a, unsigned char b, unsigned char c)
{
	if ((a <= b) && (b < c) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

static void send_frame_G(unsigned char frame_kind, unsigned char frame_nr, unsigned char frame_expected, unsigned char *packet, unsigned int len)
{
	struct FRAME f;
	f.kind = frame_kind;
	if (frame_kind == DATA)
	{
		strncpy(f.data, packet, len);
	}
	f.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);
	f.seq = frame_nr;
	*(unsigned int *)(packet + len) = crc32(packet, len);

	send_frame(&f, 3 + len + sizeof(unsigned int));

	start_timer(f.seq, DATA_TIMER);
}

void Go_back_n()
{
	unsigned char next_frame_to_send = 0;
	unsigned char frame_expected = 0;
	unsigned char ack_expected = 0;

	unsigned char nbuffer = 0;
	int event, timeout_seq;

	char buffer[MAX_SEQ + 1][PKT_LEN];

	int pkt_len[MAX_SEQ + 1];

	struct FRAME f;

}
void selective();
int main(int argc, char** argv)
{
	if (argc < 2)
	{
		lprintf("Lack of argument.\n");
		return 0;
	}

	switch (argv[1][1])
	{
	case 'G':
	case 'g':
		Go_back_n();
	case 'S':
	case 's':
		selective();
	default:
		lprintf("Please input the right argument.\n-g -G:\tGo_back_n\n-s -S:\tseletive");
	}
}

/*

�����ӿ�
#define PKT_LEN 256
void enable_network_layer(void);
void disable_network_layer(void);
int get_packet(unsigned char *packet);
void put_packet(unsigned char *packet, int len);

�¼�����
int wait_for_event(int *arg);
#define NETWORK_LAYER_READY 0
#define PHYSICAL_LAYER_READY 1
#define FRAME_RECEIVED 2
#define DATA_TIMEOUT 3
#define ACK_TIMEOUT 4

�����ӿ�
void send_frame(unsigned char *frame, int len);
int recv_frame(unsigned char *buf, int size);
int phl_sq_len(void);

CRCУ���
unsigned int crc32(unsigned char *buf, int len);

��ʱ��
unsigned int get_ms(void);
void start_timer(unsigned int nr, unsigned int ms);
void stop_timer(unsigned int nr);
void start_ack_timer(unsigned int ms);
void stop_ack_timer(void);

���̸���
extern void dbg_event(char *fmt, ...);
extern void dbg_frame(char *fmt, ...);
extern void dbg_warning(char *fmt, ...);
char *station_name(void);

struct FRAME { 
    unsigned char kind; 
    unsigned char ack;
    unsigned char seq;
    unsigned char data[PKT_LEN]; 
    unsigned int  padding;
};

static unsigned char frame_nr = 0, buffer[PKT_LEN], nbuffered;
static unsigned char frame_expected = 0;
static int phl_ready = 0;

static void put_frame(unsigned char *frame, int len)
{
    *(unsigned int *)(frame + len) = crc32(frame, len);
    send_frame(frame, len + 4);
    phl_ready = 0;
}

static void send_data_frame(void)
{
    struct FRAME s;

    s.kind = FRAME_DATA;
    s.seq = frame_nr;
    s.ack = 1 - frame_expected;
    memcpy(s.data, buffer, PKT_LEN);

    dbg_frame("Send DATA %d %d, ID %d\n", s.seq, s.ack, *(short *)s.data);

    put_frame((unsigned char *)&s, 3 + PKT_LEN);
    start_timer(frame_nr, DATA_TIMER);
}

static void send_ack_frame(void)
{
    struct FRAME s;

    s.kind = FRAME_ACK;
    s.ack = 1 - frame_expected;

    dbg_frame("Send ACK  %d\n", s.ack);

    put_frame((unsigned char *)&s, 2);
}

int main(int argc, char **argv)
{
    int event, arg;
    struct FRAME f;
    int len = 0;

    protocol_init(argc, argv); 
    lprintf("Designed by Jiang Yanjun, build: " __DATE__"  "__TIME__"\n");

    disable_network_layer();

    for (;;) {
        event = wait_for_event(&arg);

        switch (event) {
        case NETWORK_LAYER_READY:
            get_packet(buffer);
            nbuffered++;
            send_data_frame();
            break;

        case PHYSICAL_LAYER_READY:
            phl_ready = 1;
            break;

        case FRAME_RECEIVED: 
            len = recv_frame((unsigned char *)&f, sizeof f);
            if (len < 5 || crc32((unsigned char *)&f, len) != 0) {
                dbg_event("**** Receiver Error, Bad CRC Checksum\n");
                break;
            }
            if (f.kind == FRAME_ACK) 
                dbg_frame("Recv ACK  %d\n", f.ack);
            if (f.kind == FRAME_DATA) {
                dbg_frame("Recv DATA %d %d, ID %d\n", f.seq, f.ack, *(short *)f.data);
                if (f.seq == frame_expected) {
                    put_packet(f.data, len - 7);
                    frame_expected = 1 - frame_expected;
                }
                send_ack_frame();
            } 
            if (f.ack == frame_nr) {
                stop_timer(frame_nr);
                nbuffered--;
                frame_nr = 1 - frame_nr;
            }
            break; 

        case DATA_TIMEOUT:
            dbg_event("---- DATA %d timeout\n", arg); 
            send_data_frame();
            break;
        }

        if (nbuffered < 1 && phl_ready)
            enable_network_layer();
        else
            disable_network_layer();
   }
}
*/