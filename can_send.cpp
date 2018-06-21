#include "can_send.h"
#include <iostream>

int caninit()
{
  int s;
  if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Error while opening socket");
		return -1;
	}
  else
    return s;

}





int cansend(float x,float y ,float z,int s)
{


	int nbytes;
	struct sockaddr_can addr;
	struct can_frame frame;
	struct ifreq ifr;
    char data[8];

	const char *ifname = "can0";


	strcpy(ifr.ifr_name, ifname);
	ioctl(s, SIOCGIFINDEX, &ifr);

	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Error in socket bind");
		return -2;
	}

	frame.can_id  = 0x5A1;
	frame.can_dlc = 8;


//sprintf(data,"%.1f,%.1f,%.1f",x,y,z);
       for(int i=0;i<=2;i++)
        {
           switch (i)
           {
             case 0:
                 sprintf(data,"%.4f",x);
                 break;

             case 1:
                 sprintf(data,"%.4f",y);
                 break;
             case 2:
                 sprintf(data,"%.4f",z);
                 break;

           }
           std::cout<<data<<std::endl;

           for(int j=0;j<=7;j++)
             { frame.data[j] = data[j];}

	       nbytes = write(s, &frame, sizeof(struct can_frame));
        }
	//printf("Wrote %d bytes\n%s", nbytes,frame.data);

}
