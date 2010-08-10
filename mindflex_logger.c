/* Copyright Matthew O'Gorman <mog@rldn.net>
 * Licensed under GPLv3 or later
 * inspired from example code for arduino and documentation for mindset
 * takes different dev as first argument /dev/rfcomm0 is used by default.
 */

#include <stdio.h>
#include <time.h>

#define MINDSET_SYNC 170
#define MINDSET_PACKET_LENGTH 32
#define MINDSET_POWER_BANDS 8

void parse_packet(unsigned char *packet)
{
        int i = 0, j = 0;
        int signal = 0, attention = 0, meditation = 0, eeg_length = 0;
        unsigned long eeg_power[MINDSET_POWER_BANDS];
        time_t tv;
        tv = time(NULL);

        for(i = 0; i < MINDSET_PACKET_LENGTH; i++) {
		switch (packet[i]) {
                case 2:
                        signal = packet[++i];
                        break;
                case 4:
                        attention = packet[++i];
                        break;
                case 5:
                        meditation = packet[++i];
                        break;
                case 131:
                        eeg_length = packet[++i];
                        for(j = 0; j < MINDSET_POWER_BANDS; j++) {
                                eeg_power[j] = ((unsigned long)packet[++i] << 16) | ((unsigned long)packet[++i] << 8) | (unsigned long)packet[++i];
                        }
                        break;
                default:
                        printf("huh what %02X\n", packet[i]);
                        return;
                }
        }
        printf("%ld,%d,%d,%d,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu\n",
               tv,signal, attention, meditation,
               eeg_power[0],
               eeg_power[1],
               eeg_power[2],
               eeg_power[3],
               eeg_power[4],
               eeg_power[5],
               eeg_power[6],
               eeg_power[7]);
        //pretty print
        /* printf("signal: %d\t", signal); */
        /* printf("attention: %d\t", attention); */
        /* printf("meditation: %d\t", meditation); */
        /* printf("delta: %d\t", eeg_power[0]); */
        /* printf("theta: %d\t", eeg_power[1]); */
        /* printf("low alpha: %d\t", eeg_power[2]); */
        /* printf("high alpha: %d\t", eeg_power[3]); */
        /* printf("low beta: %d\t", eeg_power[4]); */
        /* printf("high beta: %d\t", eeg_power[5]); */
        /* printf("low gamma: %d\t", eeg_power[6]); */
        /* printf("mid gamma: %d\n", eeg_power[7]); */

}
int main(int argc, char **argv) {
        FILE *fd;
        int i;
        int in_packet = 0;
        int in_sequenece = 0;
        int flag = 0;
        unsigned char byte;
        int checksum = 0;
        unsigned char packet[32];
        int packet_length;
        int bytes_read;
	if(argc == 2)
		fd = fopen(argv[1], "rb");
	else
	        fd = fopen("/dev/rfcomm0", "rb");
        if (!fd) {
                fprintf(stderr, "error couldnt open your mind.\n");
                return 1;
        }
        while(1) {
                checksum = 0;

                byte = fgetc(fd);
                if(byte == 255 ) {
                        fprintf(stderr, "error could no longer read your mind.\n");
                        return 1;
                }
                if (byte  != MINDSET_SYNC) {
                        continue;
                }
                byte = fgetc(fd);
                if (byte  != MINDSET_SYNC) {
                        continue;
                }
                byte = fgetc(fd);
                if(byte != MINDSET_PACKET_LENGTH) {
                        fprintf(stderr, "packet claims wrong length %d not %d\n", byte, MINDSET_PACKET_LENGTH);
                        continue;
                }
                bytes_read = fread( packet, 1, MINDSET_PACKET_LENGTH, fd);
                if(bytes_read != MINDSET_PACKET_LENGTH)  {
                        fprintf(stderr, "didnt get full packet got %d bytes.", bytes_read);
                        continue;
                }
                for( i=0; i<MINDSET_PACKET_LENGTH; i++ )  {
                        checksum += packet[i];
                }
                checksum &= 0xFF;
                checksum = ~checksum & 0xFF;
                byte = fgetc(fd);
                if(byte == checksum) {
                        parse_packet(packet);
                        /* debugging */
                        /* for(i=0; i< MINDSET_PACKET_LENGTH; i++) { */
                        /*         printf("%02X,", packet[i]); */
                        /* } */
                        /* printf("\t %02X\n", byte); */
                        continue;
                } else {
                        /* printf("--"); */
                        /* for(i=0; i< MINDSET_PACKET_LENGTH; i++) { */
                        /*         printf("%02X,", packet[i]); */
                        /* } */
                        /* printf("\t %02X\n", byte); */
                        continue;
                }
        }
        fclose(fd);
        return 0;
}
